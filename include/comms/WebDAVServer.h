#pragma once

#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <WiFi.h>
#include <FS.h>

class ESPWebDAV;

class WebDAVServer {
  public:
    using ProgressCallback = std::function<void(const char* filename, int percent, bool receive)>;

    static WebDAVServer* instance();

    /**
     * @brief Start the WebDAV server
     * @param tcpServer WiFiServer instance (optional; server will create one if nullptr)
     * @param filesystem Pointer to fs::FS (must be properly initialized)
     * @param progressCallback Lambda/callback for transfer progress updates
     * @return true if started successfully, false if already running or init failed
     */
    bool start(WiFiServer* tcpServer, fs::FS* filesystem, ProgressCallback progressCallback);

    /**
     * @brief Stop the WebDAV server
     */
    void stop();

    /**
     * @brief Check if server is currently running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return running; }

  private:
    WebDAVServer();
    ~WebDAVServer();

    void serverThread();

    std::unique_ptr<std::thread> thread;
    std::unique_ptr<class ESPWebDAV> dav;
    WiFiServer* tcpServer;
    std::unique_ptr<WiFiServer> ownedTcpServer;
    fs::FS* filesystem;
    std::atomic<bool> running{false};
    std::mutex mutex;
};

