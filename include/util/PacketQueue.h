#pragma once

#include <thread>

#include <memory>
#include <queue>

#ifdef PICO_RP2350
#include "pico/mutex.h"
#define MUTEX_ENTER(x) mutex_enter_blocking(&x)
#define MUTEX_EXIT(x) mutex_exit(&x);
#define MUTEX_CREATE(x) mutex_t x
#define MUTEX_INIT(x) mutex_init(&x)
#define MUTEX_LOCK(x) mutex_lock(&x)
#define MUTEX_UNLOCK(x) mutex_unlock(&x)
#define MUTEX_TRY_LOCK(x) mutex_try_lock(&x)
#define MUTEX_TYPE mutex_t
#define CONDITION_VARIABLE_CREATE(x) pico_condvar_t x
#else
#include <mutex>
#define MUTEX_ENTER(x) std::lock_guard<std::mutex> lock(x)
#define MUTEX_CREATE(x) std::mutex x
#define MUTEX_INIT(x)
#define MUTEX_LOCK(x) x.lock()
#define MUTEX_UNLOCK(x) x.unlock()
#define MUTEX_TRY_LOCK(x) x.try_lock()
#define MUTEX_TYPE std::mutex
#define CONDITION_VARIABLE_CREATE(x) std::condition_variable x
#endif

#ifdef BLOCKING_PACKET_QUEUE
#ifdef PICO_RP2350
#include "pico/condition_variable.h"
#else
#include <condition_variable>
#endif
#endif

/**
 * Generic platform independent and re-entrant queue wrapper that can be used to
 * safely pass (generic) movable objects between threads.
 */
template <typename T> class PacketQueue
{
  public:
    PacketQueue() { MUTEX_INIT(mutex); }

    PacketQueue(PacketQueue const &other) = delete;

    /**
     * Push movable object into queue
     */
    void push(T &&packet)
    {
        MUTEX_ENTER(mutex);
        queue.push(packet.move());
        MUTEX_EXIT(mutex);
#ifdef BLOCKING_PACKET_QUEUE
        cond.notify_one();
#endif
    }

#ifdef BLOCKING_PACKET_QUEUE
    /**
     * Pop movable object from queue (blocking)
     */
    std::unique_ptr<T> pop(void)
    {
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock, [this] { return !queue.empty(); });
        T packet = queue.front()->move();
        queue.pop();
        return packet;
    }
#endif

    /**
     * Pop movable object from queue (non-blocking)
     */
    std::unique_ptr<T> try_pop()
    {
        MUTEX_ENTER(mutex);
        if (queue.empty()) {
            MUTEX_EXIT(mutex);
            return {nullptr};
        }
        auto packet = queue.front()->move();
        queue.pop();
        MUTEX_EXIT(mutex);
        return packet;
    }

    uint32_t size() const
    {
        MUTEX_ENTER(mutex);
        uint32_t size = queue.size();
        MUTEX_EXIT(mutex);
        return size;
    }

  private:
    mutable MUTEX_TYPE mutex;
    std::queue<std::unique_ptr<T>> queue;
#ifdef BLOCKING_PACKET_QUEUE
    std::condition_variable cond;
#endif
};