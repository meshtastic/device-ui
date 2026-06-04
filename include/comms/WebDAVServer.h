#pragma once

#include <FS.h>
#include <WiFi.h>
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <functional>
#include <memory>
#include <mutex>

#ifndef WEBDAV_HOSTNAME
#define WEBDAV_HOSTNAME "meshtastic"
#endif

class ESPWebDAV;

class WebDAVServer
{
  public:
    // Progress callback: filename, percent complete (0-100), is_receive
    using ProgressCallback = std::function<void(const char *filename, int percent, bool receive)>;

    static WebDAVServer *instance();

    /**
     * @brief Initialize WiFi for WebDAV use
     * Device-UI owns WiFi lifecycle to prevent concurrent access with firmware WiFi
     * @param ssid WiFi network name
     * @param password WiFi password
     * @return true if WiFi is connected (or was already connected)
     */
    bool initWiFi(const char *ssid, const char *password);

    /**
     * @brief Initialize MDNS.
     */
    void initMDNS(void);

    /**
     * @brief Deinitialize WiFi
     */
    void deinitWiFi();

    /**
     * @brief Start the WebDAV server
     * Requires WiFi to be initialized first via initWiFi()
     * @param filesystem Pointer to fs::FS (must be properly initialized)
     * @return true if started successfully, false if WiFi not connected or already running
     */
    bool start(fs::FS *filesystem);

    /**
     * @brief Stop the WebDAV server
     */
    void stop();

    /**
     * @brief Check if WiFi is connected
     * @return true if connected, false otherwise
     */
    bool isWiFiConnected() const;

    /**
     * @brief Check if server is currently running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return running; }

    /**
     * @brief Check if status has changed (WiFi or WebDAV)
     * UI thread should poll this and call updateWebDAVStatus() when true
     * @return true if status changed since last check, false otherwise
     */
    bool checkStatusChanged() { return statusChanged.exchange(false); }

    /**
     * @brief Check if a file transfer is currently in progress
     * Useful for showing "Transfer in progress..." message in UI
     * @return true if transfer is ongoing, false otherwise
     */
    bool isTransferInProgress();

  private:
    WebDAVServer();
    ~WebDAVServer();

    static void WiFiEventHandler(WiFiEvent_t event, WiFiEventInfo_t info);
    void onWiFiConnected();
    void onWiFiDisconnected();
    void notifyStatusChange();
    void onTransferProgress(const char *filename, int percent, bool receive);
    void serverThread();

    // FreeRTOS task wrapper
    static void serverThreadTaskWrapper(void *pvParameters);

    fs::FS *filesystem;
    WiFiServer *tcpServer;
    mutable std::mutex mutex;
    std::unique_ptr<class ESPWebDAV> dav;
    std::unique_ptr<WiFiServer> ownedTcpServer;
    std::atomic<bool> running{false};
    std::atomic<bool> wifiInitialized{false};     // Track if we initialized WiFi
    std::atomic<bool> serverReady{false};         // Track if server should accept connections
    std::atomic<bool> statusChanged{false};       // Signals UI thread that status changed
    std::atomic<bool> transferInProgress{false};  // True while file transfer is ongoing
    std::atomic<uint32_t> lastTransfer100Time{0}; // millis() when we last saw 100% completion
    TaskHandle_t serverTaskHandle = nullptr;      // FreeRTOS task handle
};
