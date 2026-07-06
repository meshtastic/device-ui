#ifndef ARCH_PORTDUINO

#include "comms/WebDAVServer.h"

#if !__has_include(<ESPWebDAV.h>) || !defined(HAS_SDCARD)

class ESPWebDAV
{
};
WebDAVServer *WebDAVServer::instance()
{
    return nullptr;
}
bool WebDAVServer::initWiFi(const char *, const char *)
{
    return false;
}
bool WebDAVServer::isWiFiConnected() const
{
    return false;
}
uint32_t WebDAVServer::RSSI() const
{
    return 0;
}
bool WebDAVServer::isTransferInProgress()
{
    return false;
}
bool WebDAVServer::start(fs::FS *)
{
    return false;
}
void WebDAVServer::stop() {}
void WebDAVServer::deinitWiFi() {}
void WebDAVServer::initMDNS() {}
WebDAVServer::WebDAVServer() {}
WebDAVServer::~WebDAVServer() {}

#else

#include "util/ILog.h"
#include <Arduino.h>
#include <ESPWebDAV.h>
#include <ESPmDNS.h>

// Explicitly defining the missing method for the library class
bool ESPWebDAV::isClientWaiting()
{
    if (!this->server) {
        return false;
    }

    if (this->locClient.fd() >= 0 && this->locClient.available() > 0) {
        return true;
    }

    return this->server->hasClient();
}

WebDAVServer *WebDAVServer::instance()
{
    static WebDAVServer *webdavServer = nullptr;
    if (!webdavServer) {
        webdavServer = new WebDAVServer();
    }
    return webdavServer;
}

WebDAVServer::WebDAVServer() : filesystem(nullptr), tcpServer(nullptr) {}

WebDAVServer::~WebDAVServer()
{
    stop();
    deinitWiFi();
}

void WebDAVServer::serverThreadTaskWrapper(void *pvParameters)
{
    WebDAVServer *pThis = static_cast<WebDAVServer *>(pvParameters);
    ILOG_INFO("[WebDAV] Server thread started.");
    pThis->serverThread();
    ILOG_INFO("[WebDAV] Server thread stopped.");
    vTaskDelete(nullptr); // Delete self on exit
}

bool WebDAVServer::initWiFi(const char *ssid, const char *password)
{
    std::lock_guard<std::mutex> lock(mutex);

    if (!ssid || !ssid[0]) {
        return false;
    }

    ILOG_INFO("[WebDAV] Initializing WiFi for WebDAV");

    // Prepare password
    const char *psk = password;
    if (!psk || !psk[0]) {
        psk = NULL; // Open network
    }

    if (!wifiInitialized) {
        // Configure WiFi
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(WEBDAV_HOSTNAME);
        WiFi.setAutoReconnect(true);
        WiFi.setSleep(false);
        WiFi.setTxPower(WIFI_POWER_11dBm); // wifi + writing SD uses alot of power, so reduce wifi

        // Register event handlers
        WiFi.onEvent(WiFiEventHandler, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(WiFiEventHandler, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
        WiFi.onEvent(WiFiEventHandler, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

        // Start connection
        WiFi.begin(ssid, psk);
        wifiInitialized = true;

        ILOG_INFO("[WebDAV] WiFi connecting to: %s", ssid);
    }
    return true;
}

void WebDAVServer::deinitWiFi()
{
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (wifiInitialized) {
            ILOG_INFO("[WebDAV] WiFi deinit");
            // Unregister event handlers by passing nullptr
            WiFi.onEvent((WiFiEventFuncCb) nullptr, ARDUINO_EVENT_WIFI_STA_CONNECTED);
            WiFi.onEvent((WiFiEventFuncCb) nullptr, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
            WiFi.onEvent((WiFiEventFuncCb) nullptr, ARDUINO_EVENT_WIFI_STA_GOT_IP);
            WiFi.disconnect(true); // true = turn off WiFi radio
            wifiInitialized = false;
        }
    }

    notifyStatusChange();
}

void WebDAVServer::initMDNS(void)
{
    MDNS.begin(WEBDAV_HOSTNAME);
    ILOG_INFO("[WebDAV] MDNS started. Advertising %s", WEBDAV_HOSTNAME ".local");
}

void WebDAVServer::onTransferProgress(const char *filename, int percent, bool receive)
{
    transferInProgress = true;

    // When transfer completes (100%), record the time for idle timeout
    if (percent == 100) {
        lastTransfer100Time = millis();
    }
}

bool WebDAVServer::isTransferInProgress()
{
    if (!transferInProgress) {
        return false;
    }

    // Check if >1s idle after 100% completion - if so, clear the flag
    uint32_t last100 = lastTransfer100Time;
    if (last100 > 0 && (millis() - last100) > 1000) {
        transferInProgress = false;
        return false;
    }

    return true;
}

bool WebDAVServer::isWiFiConnected() const
{
    std::lock_guard<std::mutex> lock(mutex);
    return WiFi.status() == WL_CONNECTED;
}

uint32_t WebDAVServer::RSSI() const
{
    return WiFi.RSSI();
}

void WebDAVServer::WiFiEventHandler(WiFiEvent_t event, WiFiEventInfo_t info)
{
    WebDAVServer *server = WebDAVServer::instance();
    if (!server)
        return;

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

void WebDAVServer::onWiFiConnected()
{
    serverReady = true;
    ILOG_INFO("[WebDAV] WiFi connected (%s), server ready to accept connections (RSSI=%d)", WiFi.localIP().toString().c_str(),
              WiFi.RSSI());

    notifyStatusChange();
}

void WebDAVServer::onWiFiDisconnected()
{
    serverReady = false;
    ILOG_WARN("[WebDAV] WiFi disconnected, pausing server");
    notifyStatusChange();
}

void WebDAVServer::notifyStatusChange()
{
    // Signal UI thread that status has changed
    // UI thread will poll checkStatusChanged() and call updateWebDAVStatus()
    ILOG_DEBUG("[WebDAV] Status changed: WiFi %sconnected, server %s", WiFi.status() == WL_CONNECTED ? "" : "not ",
               running ? "" : "not ");
    statusChanged = true;
}

bool WebDAVServer::start(fs::FS *filesystem_)
{
    std::lock_guard<std::mutex> lock(mutex);

    if (running) {
        return false;
    }

    if (!filesystem_) {
        ILOG_WARN("[WebDAV] start() called without filesystem");
        return false;
    }
    filesystem = filesystem_;

    // Create our own WiFiServer on port 80
    ownedTcpServer = std::make_unique<WiFiServer>(80);
    tcpServer = ownedTcpServer.get();
    tcpServer->begin();

    ILOG_INFO("[WebDAV] WebDAV server socket listening on port 80");

    // Create WebDAV instance
    dav = std::make_unique<ESPWebDAV>();
    dav->begin(tcpServer, filesystem);

    // Register transfer progress callback - updates transfer state flags
    // (no LVGL calls, just atomic flag updates safe from WebDAV thread)
    dav->setTransferStatusCallback(
        std::bind(&WebDAVServer::onTransferProgress, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // Start the server task using FreeRTOS
    // Priority 0 (tskIDLE_PRIORITY) - lower than loopTask (priority 1)
    // Stack 10240 bytes (10 KB) - sufficient for WebDAV protocol and file operations
    running = true;
    BaseType_t result = xTaskCreatePinnedToCore(serverThreadTaskWrapper,   // Task function
                                                "WebDAVServer",            // Task name (for debugging)
                                                10240,                     // Stack size in bytes (10 KB)
                                                static_cast<void *>(this), // Parameter (pass 'this')
                                                1,                         // Priority: tskIDLE_PRIORITY (lower than UI)
                                                &serverTaskHandle,         // Task handle output
                                                tskNO_AFFINITY             // Let scheduler choose core
    );

    if (result != pdPASS) {
        ILOG_ERROR("[WebDAV] Failed to create server task");
        running = false;
        dav = nullptr;
        ownedTcpServer = nullptr;
        tcpServer = nullptr;
        filesystem = nullptr;
        return false;
    }

    notifyStatusChange();
    return true;
}

void WebDAVServer::stop()
{
    TaskHandle_t taskToDelete = nullptr;
    std::unique_ptr<class ESPWebDAV> davToCleanup;
    std::unique_ptr<WiFiServer> ownedServerToCleanup;

    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!running) {
            return;
        }

        ILOG_INFO("[WebDAV] Stopping server");

        // Signal task to exit
        running = false;
        serverReady = false;

        // Stop the server socket gracefully
        if (tcpServer) {
            tcpServer->stop();
        }

        // Move ownership out so we can cleanup without holding lock
        taskToDelete = serverTaskHandle;
        serverTaskHandle = nullptr;
        davToCleanup = std::move(dav);
        ownedServerToCleanup = std::move(ownedTcpServer);

        tcpServer = nullptr;
        filesystem = nullptr;
    }

    // Delete task outside the lock to avoid potential deadlock
    // The task will see running=false and exit cleanly
    if (taskToDelete) {
        // Give the task a moment to notice running=false
        vTaskDelay(pdMS_TO_TICKS(10));
        // Force delete (task may still be in handleClient() blocking call)
        vTaskDelete(taskToDelete);
    }

    notifyStatusChange();
}

#include "SPILock.h"
void WebDAVServer::serverThread()
{
    while (running) {
        // Wait for WiFi to connect if not yet ready
        if (!serverReady) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        bool clientWaiting = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            clientWaiting = running && serverReady && dav && dav->isClientWaiting();
        }

        if (!clientWaiting) {
            // Idle State: No data is being transferred over WebDAV.
            // Sleep for 40ms. This keeps your LVGL/TFT graphics running fluidly.
            vTaskDelay(pdMS_TO_TICKS(40));
            continue;
        }

        // Lock the SPI bus only after a passive readiness check so we do not
        // block the UI task while probing the socket state.
        if (!spiLock->lock(500)) {
            // SPI Bus busy (TFT or LoRa using it); yield and retry immediately
            vTaskDelay(pdMS_TO_TICKS(2));
            continue;
        }

        bool handledClient = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (running && serverReady && dav && dav->isClientWaiting()) {
                dav->handleClient();
                handledClient = true;
            }
        }

        spiLock->unlock();

        if (handledClient) {
            // Yield briefly during active transfers to keep the network stable
            vTaskDelay(pdMS_TO_TICKS(1));
        } else {
            vTaskDelay(pdMS_TO_TICKS(2));
        }
    }
}

#endif // has_include(ESPWebDAV)

#endif // ARCH_PORTDUINO