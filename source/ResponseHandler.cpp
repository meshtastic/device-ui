#include "ResponseHandler.h"
#include "Arduino.h"
#include "ILog.h"

/**
 * @brief Construct a new Response Handler:: Response Handler object
 *
 * @param timeout
 */
ResponseHandler::ResponseHandler(uint32_t timeout) : requestIdCounter(0), maxTime(timeout) {}

uint32_t ResponseHandler::addRequest(uint32_t id, RequestType type)
{
    requestIdCounter++;
    uint32_t requestId = ((id & 0x0000ffff) << 16) + requestIdCounter;
    pendingRequest[requestId] = Request{.id = id, .type = type, .timestamp = millis()};
    return requestId;
}

ResponseHandler::RequestType ResponseHandler::findRequest(uint32_t requestId)
{
    const auto it = pendingRequest.find(requestId);
    if (it != pendingRequest.end()) {
        return it->second.type;
    }
    return noRequest;
}

ResponseHandler::RequestType ResponseHandler::removeRequest(uint32_t requestId)
{
    RequestType type = noRequest;
    const auto it = pendingRequest.find(requestId);
    if (it != pendingRequest.end()) {
        type = it->second.type;
        ILOG_DEBUG("removing request %08x\n", it->first);
        pendingRequest.erase(it);
    }
    return type;
}

/**
 * @brief  Garbage collection that is periodically called.
 *         removes all pending requests that timed out
 *
 */
void ResponseHandler::task_handler(void)
{
    if (pendingRequest.size())
        ILOG_DEBUG("ResponseHandler has %d pending request(s)\n", pendingRequest.size());
    auto it = pendingRequest.begin();
    while (it != pendingRequest.end()) {
        if (it->second.timestamp + maxTime < millis()) {
            ILOG_DEBUG("removing timed out request %08x\n", it->first);
            it = pendingRequest.erase(it);
        } else {
            it++;
        }
    }
}
