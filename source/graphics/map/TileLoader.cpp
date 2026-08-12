#include "graphics/map/TileLoader.h"
#include "util/ILog.h"

#include <cstring>

// ─── Platform-specific thread spawn ──────────────────────────────────────────

#ifdef ARDUINO_ARCH_ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void AsyncTileLoader::workerTask(void *arg)
{
    static_cast<AsyncTileLoader *>(arg)->workerLoop();
    vTaskDelete(nullptr);
}

void AsyncTileLoader::start(ITileService *service)
{
    if (running_)
        return;
    service_ = service;
    running_ = true;
    // pin to core 0; LVGL runs on core 1 — keep HTTP/decode off the UI core
    xTaskCreatePinnedToCore(workerTask, "tileLoader", 8192, this, 1, &taskHandle_, 0);
}

void AsyncTileLoader::stop()
{
    if (!running_)
        return;
    running_ = false;
    requestQueue_.stop(); // wake blocked pop() so workerLoop() exits
    taskHandle_ = nullptr;
}

#else // Portduino / Linux

#include <thread>

void AsyncTileLoader::start(ITileService *service)
{
    if (running_)
        return;
    service_ = service;
    running_ = true;
    thread_ = std::thread(&AsyncTileLoader::workerLoop, this);
}

void AsyncTileLoader::stop()
{
    if (!running_)
        return;
    running_ = false;
    requestQueue_.stop();
    if (thread_.joinable())
        thread_.join();
}
#endif

// ─── UI-task API ─────────────────────────────────────────────────────────────

void AsyncTileLoader::enqueue(uint32_t hash, uint32_t generation, const char *filename)
{
    Request req;
    req.hash = hash;
    req.generation = generation;
    strncpy(req.filename, filename, IMG_PATH_LEN - 1);
    req.filename[IMG_PATH_LEN - 1] = '\0';
    requestQueue_.push(req);
}

void AsyncTileLoader::drainResults(std::function<void(Result &)> consumer)
{
    Result result;
    while (resultQueue_.pop(result))
        consumer(result);
}

void AsyncTileLoader::flushQueues()
{
    requestQueue_.clear();
    Result stale;
    while (resultQueue_.pop(stale)) {
        if (stale.img_dsc) {
            if (stale.img_dsc->data)
                lv_free((void *)stale.img_dsc->data);
            lv_free(stale.img_dsc);
        }
    }
}

// ─── Background worker ───────────────────────────────────────────────────────

void AsyncTileLoader::workerLoop()
{
    while (running_) {
        Request req;
        if (!requestQueue_.pop(req, 200)) // blocks up to 200 ms, then re-checks running_
            continue;
        if (!service_)
            continue;

        lv_image_dsc_t *img_dsc = service_->loadRaw(req.filename);
        Result result{req.hash, req.generation, img_dsc};
        resultQueue_.push(result);
    }
}
