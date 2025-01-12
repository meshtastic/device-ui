#include "graphics/common/ResponseHandler.h"
#include "Arduino.h"
#include "util/ILog.h"

uint32_t ResponseHandler::rollingPacketId = 0;

/**
 * @brief Construct a new Response Handler:: Response Handler object
 *
 * @param timeout
 */
ResponseHandler::ResponseHandler(uint32_t timeout) : requestIdCounter(0), maxTime(timeout)
{
    rollingPacketId = random(UINT32_MAX & 0x7fffffff);
}

uint32_t ResponseHandler::addRequest(uint32_t id, RequestType type, void *cookie, Callback cb)
{
    requestIdCounter++;
    uint32_t requestId = generatePacketId();
    pendingRequest[requestId] = Request{.id = id, .timestamp = millis(), .type = type, .cookie = cookie, .cb = cb};
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
    return rollingPacketId | random(UINT32_MAX & 0x7fffffff) << 10;
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
        if (req.timestamp + maxTime < millis()) {
            ILOG_DEBUG("removing timed out request %08x", it->first);
            if (req.cb)
                req.cb(req, timeout, 0);
            it = pendingRequest.erase(it);
        } else {
            it++;
        }
    }
}
