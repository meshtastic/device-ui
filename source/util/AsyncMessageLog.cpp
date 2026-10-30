#include "util/AsyncMessageLog.h"
#include "Arduino.h"
#include "util/ILog.h"
#include <mutex>
#ifndef ARDUINO_ARCH_ESP32
#include <chrono>
#endif

namespace
{
void workerDelay(uint32_t durationMs)
{
#ifdef ARDUINO_ARCH_ESP32
    delay(durationMs);
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
#endif
}
} // namespace

AsyncMessageLog::AsyncMessageLog(Writer writer, Clear clear, Clock clock)
    : writer(std::move(writer)), clear(std::move(clear)), clock(clock ? std::move(clock) : Clock([]() { return millis(); }))
{
}

void AsyncMessageLog::Mutex::lock()
{
#ifdef ARDUINO_ARCH_ESP32
    xSemaphoreTake(handle, portMAX_DELAY);
#else
    handle.lock();
#endif
}

void AsyncMessageLog::Mutex::unlock()
{
#ifdef ARDUINO_ARCH_ESP32
    xSemaphoreGive(handle);
#else
    handle.unlock();
#endif
}

AsyncMessageLog::~AsyncMessageLog()
{
    prepareShutdown(1000);
    running = false;
#ifdef ARDUINO_ARCH_ESP32
    while (!exited)
        workerDelay(1);
#else
    if (worker.joinable())
        worker.join();
#endif
}

bool AsyncMessageLog::start()
{
    if (running)
        return true;
    running = true;
    exited = false;
#ifdef ARDUINO_ARCH_ESP32
    if (xTaskCreate(workerTask, "messageLog", 4096, this, 1, nullptr) != pdPASS) {
        running = false;
        exited = true;
        ILOG_ERROR("Cannot start message log worker");
        return false;
    }
#else
    worker = std::thread(&AsyncMessageLog::workerLoop, this);
#endif
    return true;
}

#ifdef ARDUINO_ARCH_ESP32
void AsyncMessageLog::workerTask(void *argument)
{
    auto *self = static_cast<AsyncMessageLog *>(argument);
    self->workerLoop();
    self->exited = true;
    vTaskDelete(nullptr);
}
#endif

bool AsyncMessageLog::append(std::unique_ptr<LogMessageEnv> entry, uint32_t requestId, uint32_t timeoutMs)
{
    std::lock_guard<Mutex> guard(mutex);
    if (!accepting)
        return false;
    pending.push_back(Pending{std::move(entry), requestId, clock(), timeoutMs, false});
    return true;
}

void AsyncMessageLog::setStatus(uint32_t requestId, LogMessage::MsgStatus status, bool final)
{
    if (!requestId)
        return;
    std::lock_guard<Mutex> guard(mutex);
    for (auto &item : pending) {
        if (item.requestId == requestId) {
            if (status != LogMessage::eNoResponse || item.entry->status == LogMessage::eDefault)
                item.entry->status = status;
            if (item.entry->to == UINT32_MAX && status == LogMessage::eHeard) {
                item.entry->status = LogMessage::eAcked;
                final = true;
            }
            if (final)
                item.requestId = 0;
            return;
        }
    }
}

bool AsyncMessageLog::remove(uint32_t from, uint32_t to, uint8_t channel)
{
    std::lock_guard<Mutex> guard(mutex);
    if (!accepting)
        return false;
    const bool clear = !from && !to && !channel;
    pending.remove_if([=](const Pending &item) {
        return clear || (item.entry && (to == UINT32_MAX ? item.entry->to == UINT32_MAX && item.entry->ch == channel
                                                         : ((item.entry->from == from && item.entry->to == to) ||
                                                            (item.entry->from == to && item.entry->to == from))));
    });
    pending.push_back(
        Pending{std::make_unique<LogMessageEnv>(from, to, channel, 0, LogMessage::eDefault, true, 0, nullptr), 0, 0, 0, clear});
    return true;
}

bool AsyncMessageLog::prepareShutdown(uint32_t timeoutMs)
{
    {
        std::lock_guard<Mutex> guard(mutex);
        accepting = false;
        for (auto &item : pending) {
            item.requestId = 0;
        }
    }
    const uint32_t started = millis();
    do {
        {
            std::lock_guard<Mutex> guard(mutex);
            if (pending.empty() && !busy)
                return true;
        }
        if (!running || millis() - started >= timeoutMs)
            return false;
        workerDelay(1);
    } while (true);
}

void AsyncMessageLog::workerLoop()
{
    while (running) {
        Pending item;
        bool found = false;
        {
            std::lock_guard<Mutex> guard(mutex);
            const uint32_t now = clock();
            for (auto position = pending.begin(); position != pending.end(); ++position) {
                if (position->requestId && now - position->queuedAt < position->timeoutMs)
                    break;
                if (position->requestId && position->entry->status == LogMessage::eDefault)
                    position->entry->status = LogMessage::eNoResponse;
                position->requestId = 0;
                item = std::move(*position);
                pending.erase(position);
                busy = true;
                found = true;
                break;
            }
        }
        if (!found) {
            workerDelay(10);
            continue;
        }
        const bool written = item.clear ? clear() : writer(*item.entry);
        {
            std::lock_guard<Mutex> guard(mutex);
            busy = false;
            if (!written)
                pending.push_front(std::move(item));
        }
        if (!written) {
            ILOG_ERROR("Message log write failed; retaining queued entry");
            workerDelay(100);
        } else
            workerDelay(1);
    }
}