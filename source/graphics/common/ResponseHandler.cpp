#include "graphics/common/ResponseHandler.h"
#include "Arduino.h"
#include "util/ILog.h"
#if defined(ARCH_PORTDUINO)
#include <chrono>
#include <cstdlib>
#include <mutex>
#endif

namespace
{
uint32_t randomPacketBits()
{
#if defined(ARCH_PORTDUINO)
    // This block will run exactly ONCE the very first time this function is called
    static std::once_flag flag;
    std::call_once(flag, []() {
        auto now = std::chrono::high_resolution_clock::now();
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        std::srand(static_cast<unsigned int>(nanos));
    });

    // Your existing code remains unchanged:
    return std::rand() % (UINT32_MAX & 0x7fffffff);
#else
    return random(UINT32_MAX & 0x7fffffff);
#endif
}
} // namespace

uint32_t ResponseHandler::rollingPacketId = 0;

/**
 * @brief Construct a new Response Handler:: Response Handler object
 *
 * @param timeout
 */
ResponseHandler::ResponseHandler(uint32_t timeout) : requestIdCounter(0), maxTime(timeout)
{
    rollingPacketId = randomPacketBits();
}

uint32_t ResponseHandler::addRequest(uint32_t id, RequestType type, void *cookie, Callback cb)
{
    requestIdCounter++;
    uint32_t requestId = generatePacketId();
    pendingRequest[requestId] =
        Request{.id = id, .timestamp = millis(), .type = type, .cookie = cookie, .cb = cb, .packetId = requestId};
    return requestId;
}

ResponseHandler::Request ResponseHandler::findRequest(uint32_t requestId, RequestType match, int32_t pass)
{
    const auto it = pendingRequest.find(requestId);
    if (it != pendingRequest.end()) {
        Request &req = it->second;
        if (req.cb && pass != -1 && (match == anyRequest || match == req.type))
            req.cb(req, found, pass);
        return req;
    }
    return Request{};
}

ResponseHandler::Request ResponseHandler::removeRequest(uint32_t requestId, RequestType match, int32_t pass)
{
    Request req{};
    const auto it = pendingRequest.find(requestId);
    if (it != pendingRequest.end()) {
        req = it->second;
        ILOG_DEBUG("removing request %08x", it->first);
        if (req.cb && pass != -1 && (match == anyRequest || match == req.type))
            req.cb(req, removed, pass);
        pendingRequest.erase(it);
    }
    return req;
}

/**
 * @brief: Generate a unique packet id
 *
 */
uint32_t ResponseHandler::generatePacketId(void)
{
    rollingPacketId++;
    rollingPacketId &= UINT32_MAX >> 22;
    return rollingPacketId | randomPacketBits() << 10;
}

/**
 * @brief  Garbage collection that is periodically called.
 *         removes all pending requests that timed out
 *
 */
void ResponseHandler::task_handler(void)
{
    if (pendingRequest.size())
        ILOG_DEBUG("ResponseHandler has %d pending request(s)", pendingRequest.size());
    auto it = pendingRequest.begin();
    while (it != pendingRequest.end()) {
        Request &req = it->second;
        if (millis() - req.timestamp >= maxTime) {
            ILOG_DEBUG("removing timed out request %08x", it->first);
            if (req.cb)
                req.cb(req, timeout, 0);
            it = pendingRequest.erase(it);
        } else {
            it++;
        }
    }
}
