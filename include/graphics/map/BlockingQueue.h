#pragma once

#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>

/**
 * Thread-safe bounded queue; maps to FreeRTOS primitives on ESP32 via the C++ stdlib shim.
 */
template <typename T> class BlockingQueue
{
  public:
    void push(T item)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push_back(std::move(item));
        }
        cv_.notify_one();
    }

    // non-blocking (timeoutMs == 0) or blocking with timeout
    bool pop(T &item, uint32_t timeoutMs = 0)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (timeoutMs == 0) {
            if (queue_.empty())
                return false;
        } else {
            cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this] { return !queue_.empty() || stopped_; });
            if (queue_.empty())
                return false;
        }
        item = std::move(queue_.front());
        queue_.pop_front();
        return true;
    }

    // discard all queued items
    void clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.clear();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    // wake any blocked pop() so the consumer thread can notice running_ == false
    void stop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = true;
        cv_.notify_all();
    }

    void reset()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = false;
        queue_.clear();
    }

  private:
    std::deque<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool stopped_ = false;
};
