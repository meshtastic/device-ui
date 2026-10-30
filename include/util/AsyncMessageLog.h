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
    using Writer = std::function<bool(const LogMessageEnv &)>;
    using Clear = std::function<bool()>;
    using Clock = std::function<uint32_t()>;
    explicit AsyncMessageLog(LogRotate &log)
        : AsyncMessageLog([&log](const LogMessageEnv &entry) { return log.write(entry); }, [&log]() { return log.clear(); })
    {
    }
    AsyncMessageLog(Writer writer, Clear clear, Clock clock = {});
    ~AsyncMessageLog();
    bool start();
    bool append(std::unique_ptr<LogMessageEnv> entry, uint32_t requestId = 0, uint32_t timeoutMs = 0);
    void setStatus(uint32_t requestId, LogMessage::MsgStatus status, bool final);
    bool remove(uint32_t from, uint32_t to, uint8_t channel);
    bool prepareShutdown(uint32_t timeoutMs);

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
    };
    void workerLoop();
    Writer writer;
    Clear clear;
    Clock clock;
    Mutex mutex;
    std::list<Pending> pending;
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