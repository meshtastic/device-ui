#pragma once

#include "graphics/map/BlockingQueue.h"
#include "graphics/map/TileService.h"
#include "lvgl.h"

#include <atomic>
#include <cstdint>
#include <functional>

#ifdef ARDUINO_ARCH_ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#else
#include <thread>
#endif

#ifndef IMG_PATH_LEN
#define IMG_PATH_LEN 64
#endif

/**
 * Decouples tile fetching from the UI task.
 * enqueue() and drainResults() run on the UI task; workerLoop() runs on a background thread.
 */
class AsyncTileLoader
{
  public:
    struct Result {
        uint32_t hash;
        uint32_t generation;
        lv_image_dsc_t *img_dsc; // heap-allocated; nullptr on failure; caller takes ownership
    };

    // start background worker; service must outlive the loader
    void start(ITileService *service);
    void stop();

    // UI task: enqueue a load request (non-blocking)
    void enqueue(uint32_t hash, uint32_t generation, const char *filename);

    // UI task: call consumer for every completed result (non-blocking drain)
    void drainResults(std::function<void(Result &)> consumer);

    // UI task: discard all in-flight requests and stale results (call on full redraw)
    void flushQueues();

    ~AsyncTileLoader() { stop(); }

  private:
    struct Request {
        uint32_t hash;
        uint32_t generation;
        char filename[IMG_PATH_LEN];
    };

    void workerLoop();

    ITileService *service_ = nullptr;
    BlockingQueue<Request> requestQueue_;
    BlockingQueue<Result> resultQueue_;
    std::atomic<bool> running_{false};
    std::atomic<bool> exited_{false};

#ifdef ARDUINO_ARCH_ESP32
    TaskHandle_t taskHandle_ = nullptr;
    static void workerTask(void *arg);
#else
    std::thread thread_;
#endif
};
