#pragma once

#include <FS.h>
#include <WiFi.h>
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <functional>
#include <mutex>
#include <string>

#ifndef FTP_HOSTNAME
#define FTP_HOSTNAME "meshtastic"
#endif

#ifndef FTP_DEFAULT_USERNAME
#define FTP_DEFAULT_USERNAME "admin"
#endif

class UiFtpServer
{
  public:
    // Progress callback: filename, percent complete (0-100), is_receive
    using ProgressCallback = std::function<void(const char *filename, int percent, bool receive)>;

    static UiFtpServer *instance();

    /**
     * @brief Initialize WiFi for FTP use
     * Device-UI owns WiFi lifecycle to prevent concurrent access with firmware WiFi
     * @param ssid WiFi network name
     * @param password WiFi password
     * @return true if WiFi init started (or was already initialized)
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
     * @brief Set FTP credentials (must be called before start())
     */
    void setCredentials(const char *username, const char *password);

    /**
     * @brief Optional callback for transfer progress events
     */
    void setProgressCallback(ProgressCallback callback);

    /**
     * @brief Start the FTP server
     * Requires WiFi to be initialized first via initWiFi()
     * @param filesystem Pointer to fs::FS (must be properly initialized)
     * @return true if started successfully, false otherwise
     */
    bool start(fs::FS *filesystem);

    /**
     * @brief Stop the FTP server
     */
    void stop();

    /**
     * @brief Check if WiFi is connected
     * @return true if connected, false otherwise
     */
    bool isWiFiConnected() const;

    /**
     * @brief Get WiFi connection quality
     * @return RSSI value
     */
    uint32_t RSSI() const;

    /**
     * @brief Check if server is currently running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return running; }

    /**
     * @brief Check if status has changed (WiFi or FTP)
     * UI thread should poll this and refresh status when true
     * @return true if status changed since last check, false otherwise
     */
    bool checkStatusChanged() { return statusChanged.exchange(false); }

    /**
     * @brief Check if a file transfer is currently in progress
     * Useful for showing transfer feedback in UI
     * @return true if transfer is ongoing, false otherwise
     */
    bool isTransferInProgress();

    // Internal callback bridge used by MultiFTPServer event callback.
    void onTransferProgress(const char *filename, int percent, bool receive);

  private:
    UiFtpServer();
    ~UiFtpServer();

    static void WiFiEventHandler(WiFiEvent_t event, WiFiEventInfo_t info);
    void onWiFiConnected();
    void onWiFiDisconnected();
    void notifyStatusChange();
    void serverThread();

    // FreeRTOS task wrapper
    static void serverThreadTaskWrapper(void *pvParameters);

    fs::FS *filesystem;
    class FtpServer *ftpServer;
    mutable std::mutex mutex;
    ProgressCallback progressCallback;
    std::atomic<bool> running{false};
    std::atomic<bool> wifiInitialized{false};     // Track if we initialized WiFi
    std::atomic<bool> serverReady{false};         // Track if server should accept connections
    std::atomic<bool> statusChanged{false};       // Signals UI thread that status changed
    std::atomic<bool> transferInProgress{false};  // True while file transfer is ongoing
    std::atomic<uint32_t> lastTransfer100Time{0}; // millis() when we last saw 100% completion
    TaskHandle_t serverTaskHandle = nullptr;      // FreeRTOS task handle
    std::string username{FTP_DEFAULT_USERNAME};
    std::string password;
};
