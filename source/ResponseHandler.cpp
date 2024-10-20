#include "ResponseHandler.h"
#include "Arduino.h"
#include "ILog.h"


uint32_t ResponseHandler::rollingPacketId = 0;

/**
 * @brief Construct a new Response Handler:: Response Handler object
 *
 * @param timeout
 */
ResponseHandler::ResponseHandler(uint32_t timeout) : requestIdCounter(0), maxTime(timeout) {
    rollingPacketId = random(UINT32_MAX & 0x7fffffff);
}

uint32_t ResponseHandler::addRequest(uint32_t id, RequestType type, void *cookie)
{
    requestIdCounter++;
    uint32_t requestId = generatePacketId();
    pendingRequest[requestId] = Request{.id = id, .cookie = cookie, .type = type, .timestamp = millis()};
    return requestId;
}

ResponseHandler::Request ResponseHandler::findRequest(uint32_t requestId)
{
    const auto it = pendingRequest.find(requestId);
    if (it != pendingRequest.end()) {
        return it->second;
    }
    return Request{};
}

ResponseHandler::Request ResponseHandler::removeRequest(uint32_t requestId)
{
    Request req{};
    const auto it = pendingRequest.find(requestId);
    if (it != pendingRequest.end()) {
        req = it->second;
        ILOG_DEBUG("removing request %08x", it->first);
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
        if (it->second.timestamp + maxTime < millis()) {
            ILOG_DEBUG("removing timed out request %08x", it->first);
            it = pendingRequest.erase(it);
        } else {
            it++;
        }
    }
}
