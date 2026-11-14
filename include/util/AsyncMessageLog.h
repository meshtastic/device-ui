#pragma once

#include "util/LogMessage.h"
#include "util/LogRotate.h"
#include <atomic>
#include <functional>
#include <list>
#include <memory>
#ifdef ARDUINO_ARCH_ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#else
#include <mutex>
#include <thread>
#endif

class AsyncMessageLog
{
  public:
    static constexpr uint32_t defaultTimeoutMs = 1000;
    using Writer = std::function<bool(const LogMessageEnv &)>;
    using Clear = std::function<bool()>;
    using Clock = std::function<uint32_t()>;
    static constexpr bool hasElapsed(uint32_t nowMs, uint32_t sinceMs, uint32_t intervalMs)
    {
        return static_cast<uint32_t>(nowMs - sinceMs) >= intervalMs;
    }
    explicit AsyncMessageLog(LogRotate &log)
        : AsyncMessageLog([&log](const LogMessageEnv &entry) { return log.write(entry); }, [&log]() { return log.clear(); })
    {
    }
    AsyncMessageLog(Writer writer, Clear clear, Clock clock = {});
    ~AsyncMessageLog();
    uint32_t nowMs() const { return clock(); }
    bool start();
    bool append(std::unique_ptr<LogMessageEnv> entry, uint32_t requestId = 0, uint32_t timeoutMs = 0);
    void setStatus(uint32_t requestId, LogMessage::MsgStatus status, bool final);
    bool remove(uint32_t from, uint32_t to, uint8_t channel);
    bool flush(uint32_t timeoutMs = defaultTimeoutMs);
    bool prepareShutdown(uint32_t timeoutMs = defaultTimeoutMs);

  private:
    struct Mutex {
        void lock();
        void unlock();
#ifdef ARDUINO_ARCH_ESP32
        StaticSemaphore_t storage;
        SemaphoreHandle_t handle = xSemaphoreCreateMutexStatic(&storage);
#else
        std::mutex handle;
#endif
    };
    struct Pending {
        std::unique_ptr<LogMessageEnv> entry;
        uint32_t requestId = 0;
        uint32_t queuedAt = 0;
        uint32_t timeoutMs = 0;
        bool clear = false;
        uint64_t seq = 0;
    };
    bool drainPending(uint32_t timeoutMs, bool stopAccepting);
    void workerLoop();
    Writer writer;
    Clear clear;
    Clock clock;
    Mutex mutex;
    std::list<Pending> pending;
    uint64_t nextSeq = 1;
    uint64_t activeSeq = 0;
    bool accepting = true;
    bool busy = false;
    std::atomic<bool> running{false};
    std::atomic<bool> exited{true};
#ifdef ARDUINO_ARCH_ESP32
    static void workerTask(void *argument);
#else
    std::thread worker;
#endif
};