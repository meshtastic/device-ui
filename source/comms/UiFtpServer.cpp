#ifndef ARCH_PORTDUINO

#include "comms/UiFtpServer.h"

#if !__has_include(<MultiFTPServer.h>) || !defined(HAS_SDCARD)

UiFtpServer *UiFtpServer::instance()
{
    return nullptr;
}
bool UiFtpServer::initWiFi(const char *, const char *)
{
    return false;
}
bool UiFtpServer::isWiFiConnected() const
{
    return false;
}
uint32_t UiFtpServer::RSSI() const
{
    return 0;
}
bool UiFtpServer::isTransferInProgress()
{
    return false;
}
bool UiFtpServer::start(fs::FS *)
{
    return false;
}
void UiFtpServer::setCredentials(const char *, const char *) {}
void UiFtpServer::setProgressCallback(ProgressCallback) {}
void UiFtpServer::stop() {}
void UiFtpServer::deinitWiFi() {}
void UiFtpServer::initMDNS() {}
UiFtpServer::UiFtpServer() : filesystem(nullptr), ftpServer(nullptr) {}
UiFtpServer::~UiFtpServer() {}

#else

#include "MultiFTPServer.h"
#include "SPILock.h"
#include "util/ILog.h"
#include <Arduino.h>
#include <ESPmDNS.h>

namespace
{
constexpr TickType_t FTP_NO_WIFI_DELAY = pdMS_TO_TICKS(100);
constexpr TickType_t FTP_IDLE_DELAY = pdMS_TO_TICKS(20);
constexpr TickType_t FTP_SPI_BUSY_DELAY = pdMS_TO_TICKS(2);
constexpr TickType_t FTP_AFTER_HANDLE_DELAY = pdMS_TO_TICKS(1);

constexpr uint16_t FTP_PORT = 21;
constexpr uint16_t PASV_PORT = 5000;
constexpr uint32_t TRANSFER_DONE_HOLD_MS = 1000;

void ftpTransferCallback(FtpTransferOperation operation, const char *filename, uint32_t transferSize)
{
    UiFtpServer *server = UiFtpServer::instance();
    if (!server) {
        return;
    }

    switch (operation) {
    case FTP_UPLOAD_START:
        ILOG_INFO("[FTP] Upload start: %s", filename ? filename : "(unknown)");
        server->onTransferProgress(filename, 0, true);
        break;
    case FTP_UPLOAD:
        // ILOG_DEBUG("[FTP] Upload bytes processed: %u", transferSize);
        server->onTransferProgress(filename, 50, true);
        break;
    case FTP_DOWNLOAD_START:
        ILOG_INFO("[FTP] Download start: %s", filename ? filename : "(unknown)");
        server->onTransferProgress(filename, 0, false);
        break;
    case FTP_DOWNLOAD:
        // ILOG_DEBUG("[FTP] Download bytes processed: %u", transferSize);
        server->onTransferProgress(filename, 50, false);
        break;
    case FTP_TRANSFER_STOP:
        // ILOG_INFO("[FTP] Transfer complete: %s", filename ? filename : "(unknown)");
        server->onTransferProgress(filename, 100, false);
        break;
    case FTP_TRANSFER_ERROR:
        ILOG_WARN("[FTP] Transfer error: %s", filename ? filename : "(unknown)");
        server->onTransferProgress(filename, 100, false);
        break;
    default:
        break;
    }
}
} // namespace

UiFtpServer *UiFtpServer::instance()
{
    static UiFtpServer *ftp = nullptr;
    if (!ftp) {
        ftp = new UiFtpServer();
    }
    return ftp;
}

UiFtpServer::UiFtpServer() : filesystem(nullptr), ftpServer(nullptr) {}

UiFtpServer::~UiFtpServer()
{
    stop();
    deinitWiFi();
}

void UiFtpServer::serverThreadTaskWrapper(void *pvParameters)
{
    UiFtpServer *pThis = static_cast<UiFtpServer *>(pvParameters);
    ILOG_INFO("[FTP] Server thread started");
    pThis->serverThread();
    ILOG_INFO("[FTP] Server thread stopped");
    vTaskDelete(nullptr); // Delete self on exit
}

bool UiFtpServer::initWiFi(const char *ssid, const char *password_)
{
    std::lock_guard<std::mutex> lock(mutex);

    if (!ssid || !ssid[0]) {
        return false;
    }

    ILOG_INFO("[FTP] Initializing WiFi for FTP");

    const char *psk = password_;
    if (!psk || !psk[0]) {
        psk = NULL; // Open network
    }

    if (!wifiInitialized) {
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(FTP_HOSTNAME);
        WiFi.setAutoReconnect(true);
        WiFi.setSleep(false);
        // WiFi.setTxPower(WIFI_POWER_11dBm); // Balance throughput and power while transferring files

        WiFi.onEvent(WiFiEventHandler, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(WiFiEventHandler, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
        WiFi.onEvent(WiFiEventHandler, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

        WiFi.begin(ssid, psk);
        wifiInitialized = true;

        ILOG_INFO("[FTP] WiFi connecting to: %s", ssid);
    }
    return true;
}

void UiFtpServer::deinitWiFi()
{
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (wifiInitialized) {
            ILOG_INFO("[FTP] WiFi deinit");
            WiFi.onEvent((WiFiEventFuncCb) nullptr, ARDUINO_EVENT_WIFI_STA_CONNECTED);
            WiFi.onEvent((WiFiEventFuncCb) nullptr, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
            WiFi.onEvent((WiFiEventFuncCb) nullptr, ARDUINO_EVENT_WIFI_STA_GOT_IP);
            WiFi.disconnect(true);
            wifiInitialized = false;
        }
    }

    notifyStatusChange();
}

void UiFtpServer::initMDNS(void)
{
    if (!MDNS.begin(FTP_HOSTNAME)) {
        ILOG_WARN("[FTP] MDNS start failed");
        return;
    }

    MDNS.addService("ftp", "tcp", FTP_PORT);
    ILOG_INFO("[FTP] MDNS started. Advertising %s.local", FTP_HOSTNAME);
}

void UiFtpServer::setCredentials(const char *username_, const char *password_)
{
    std::lock_guard<std::mutex> lock(mutex);

    if (running) {
        ILOG_WARN("[FTP] Ignoring setCredentials while running");
        return;
    }

    username = (username_ && username_[0]) ? username_ : FTP_DEFAULT_USERNAME;
    if (password_ && password_[0])
        password = password_;
    else
        ILOG_ERROR("setCredentials: too short password");
}

void UiFtpServer::setProgressCallback(ProgressCallback callback)
{
    std::lock_guard<std::mutex> lock(mutex);
    progressCallback = std::move(callback);
}

void UiFtpServer::onTransferProgress(const char *filename, int percent, bool receive)
{
    transferInProgress = true;

    if (percent == 100) {
        lastTransfer100Time = millis();
    }

    ProgressCallback callbackCopy;
    {
        std::lock_guard<std::mutex> lock(mutex);
        callbackCopy = progressCallback;
    }

    if (callbackCopy) {
        callbackCopy(filename, percent, receive);
    }
}

bool UiFtpServer::isTransferInProgress()
{
    if (!transferInProgress) {
        return false;
    }

    uint32_t last100 = lastTransfer100Time;
    if (last100 > 0 && (millis() - last100) > TRANSFER_DONE_HOLD_MS) {
        transferInProgress = false;
        return false;
    }

    return true;
}

bool UiFtpServer::isWiFiConnected() const
{
    std::lock_guard<std::mutex> lock(mutex);
    return WiFi.status() == WL_CONNECTED;
}

uint32_t UiFtpServer::RSSI() const
{
    return WiFi.RSSI();
}

void UiFtpServer::WiFiEventHandler(WiFiEvent_t event, WiFiEventInfo_t info)
{
    (void)info;
    UiFtpServer *server = UiFtpServer::instance();
    if (!server) {
        return;
    }

    if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
        server->onWiFiConnected();
    } else if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
        server->onWiFiDisconnected();
    } else if (event == ARDUINO_EVENT_WIFI_STA_CONNECTED) {
        ILOG_INFO("[WiFi] connected");
    } else {
        ILOG_WARN("[WiFi] event: %d", (int)event);
    }
}

void UiFtpServer::onWiFiConnected()
{
    serverReady = true;
    ILOG_INFO("[FTP] WiFi connected (%s), server ready to accept connections (RSSI=%d)", WiFi.localIP().toString().c_str(),
              WiFi.RSSI());

    notifyStatusChange();
}

void UiFtpServer::onWiFiDisconnected()
{
    serverReady = false;
    ILOG_WARN("[FTP] WiFi disconnected, pausing server");
    notifyStatusChange();
}

void UiFtpServer::notifyStatusChange()
{
    ILOG_DEBUG("[FTP] Status changed: WiFi %sconnected, server %s", WiFi.status() == WL_CONNECTED ? "" : "not ",
               running ? "" : "not ");
    statusChanged = true;
}

bool UiFtpServer::start(fs::FS *filesystem_)
{
    std::lock_guard<std::mutex> lock(mutex);

    if (running) {
        return false;
    }

    if (!filesystem_) {
        ILOG_WARN("[FTP] start() called without filesystem");
        return false;
    }

    if (password.empty()) {
        ILOG_WARN("[FTP] start() called with empty password");
        return false;
    }

    filesystem = filesystem_;
    ftpServer = new FtpServer(FTP_PORT, PASV_PORT);
    ftpServer->setTransferCallback(ftpTransferCallback);
    ftpServer->begin(username.c_str(), password.c_str());

    ILOG_INFO("[FTP] Server listening on port %u", FTP_PORT);

    running = true;
    BaseType_t result = xTaskCreatePinnedToCore(serverThreadTaskWrapper,   // Task function
                                                "UiFtpServer",             // Task name
                                                10240,                     // Stack size in bytes
                                                static_cast<void *>(this), // Parameter
                                                1,                         // Lower priority than loopTask
                                                &serverTaskHandle,         // Task handle output
                                                1                          // Pin to CPU1
    );

    if (result != pdPASS) {
        ILOG_ERROR("[FTP] Failed to create server task");
        running = false;
        delete ftpServer;
        ftpServer = nullptr;
        filesystem = nullptr;
        return false;
    }

    notifyStatusChange();
    return true;
}

void UiFtpServer::stop()
{
    TaskHandle_t taskToDelete = nullptr;
    FtpServer *ftpToCleanup = nullptr;

    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!running) {
            return;
        }

        ILOG_INFO("[FTP] Stopping server");

        running = false;
        serverReady = false;

        taskToDelete = serverTaskHandle;
        serverTaskHandle = nullptr;
        ftpToCleanup = ftpServer;
        ftpServer = nullptr;
        filesystem = nullptr;
    }

    if (taskToDelete) {
        vTaskDelay(pdMS_TO_TICKS(10));
        vTaskDelete(taskToDelete);
    }

    if (ftpToCleanup) {
        ftpToCleanup->end();
        delete ftpToCleanup;
    }

    notifyStatusChange();
}

void UiFtpServer::serverThread()
{
    while (running) {
        if (!serverReady) {
            vTaskDelay(FTP_NO_WIFI_DELAY);
            continue;
        }

        if (!spiLock->lock(500)) {
            vTaskDelay(FTP_SPI_BUSY_DELAY);
            continue;
        }

        bool handledClient = false;
        FtpServer *ftp = nullptr;
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (running && serverReady) {
                ftp = ftpServer;
            }
        }

        // Do not hold the class mutex while handling FTP traffic.
        // FtpServer can invoke transfer callbacks that re-enter UiFtpServer.
        if (ftp) {
            ftp->handleFTP();
            handledClient = true;
        }

        spiLock->unlock();

        if (handledClient) {
            vTaskDelay(FTP_AFTER_HANDLE_DELAY);
        } else {
            vTaskDelay(FTP_IDLE_DELAY);
        }
    }
}

#endif // has_include(MultiFTPServer)

#endif // ARCH_PORTDUINO
