#include "comms/WebDAVServer.h"
#include <ESPWebDAV.h>

static WebDAVServer* g_webdav_instance = nullptr;

WebDAVServer* WebDAVServer::instance() {
    if (!g_webdav_instance) {
        g_webdav_instance = new WebDAVServer();
    }
    return g_webdav_instance;
}

WebDAVServer::WebDAVServer() : tcpServer(nullptr), filesystem(nullptr) {
}

WebDAVServer::~WebDAVServer() {
    stop();
}

bool WebDAVServer::start(WiFiServer* tcpServer_, fs::FS* filesystem_, ProgressCallback progressCallback) {
    std::lock_guard<std::mutex> lock(mutex);

    if (running) {
        return false;
    }

    if (!filesystem_) {
        return false;
    }

    filesystem = filesystem_;

    // Use provided WiFiServer or create one
    if (tcpServer_) {
        tcpServer = tcpServer_;
    } else {
        // Create our own WiFiServer on port 80
        ownedTcpServer = std::make_unique<WiFiServer>(80);
        tcpServer = ownedTcpServer.get();
        tcpServer->begin();
    }

    // Create WebDAV instance
    dav = std::make_unique<ESPWebDAV>();
    dav->begin(tcpServer, filesystem);

    // Set the progress callback
    if (progressCallback) {
        dav->setTransferStatusCallback(progressCallback);
    }

    // Start the server thread
    running = true;
    thread = std::make_unique<std::thread>(&WebDAVServer::serverThread, this);

    return true;
}

void WebDAVServer::stop() {
    std::lock_guard<std::mutex> lock(mutex);

    if (!running) {
        return;
    }

    running = false;

    // Wait for thread to finish
    if (thread && thread->joinable()) {
        thread->join();
    }
    thread.reset();

    // Cleanup WebDAV
    if (dav) {
        dav.reset();
    }

    // Cleanup our own WiFiServer if we created it
    if (ownedTcpServer) {
        ownedTcpServer.reset();
    }

    tcpServer = nullptr;
    filesystem = nullptr;
}

void WebDAVServer::serverThread() {
    while (running) {
        if (dav && tcpServer) {
            dav->handleClient();
        }
        // Small yield to prevent blocking
        delay(5);
    }
}
