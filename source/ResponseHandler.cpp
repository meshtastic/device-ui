#include "ResponseHandler.h"
#include "Arduino.h"
#include "ILog.h"

/**
 * @brief Construct a new Response Handler:: Response Handler object
 *
 * @param timeout
 */
ResponseHandler::ResponseHandler(uint32_t timeout) : requestIdCounter(0), maxTime(timeout) {}

uint32_t ResponseHandler::addRequest(uint32_t id)
{
    requestIdCounter++;
    uint32_t requestId = ((id & 0x0000ffff) << 16) + requestIdCounter;
    pendingRequest[requestId] = Request{.id = id, .timestamp = millis()};
    return requestId;
}

uint32_t ResponseHandler::findRequest(uint32_t requestId)
{
    const auto it = pendingRequest.find(requestId);
    if (it != pendingRequest.end()) {
        return it->second.id;
    }
    return UINT32_MAX;
}

uint32_t ResponseHandler::removeRequest(uint32_t requestId)
{
    uint32_t id = UINT32_MAX;
    const auto it = pendingRequest.find(requestId);
    if (it != pendingRequest.end()) {
        id = it->second.id;
        pendingRequest.erase(it);
    }
    return id;
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
    std::map<uint32_t, Request>::iterator it = pendingRequest.begin();
    while (it != pendingRequest.end()) {
        if (it->second.timestamp + maxTime < millis()) {
            ILOG_DEBUG("removing timed out request %08x\n", it->first);
            it = pendingRequest.erase(it);
        } else {
            it++;
        }
    }
}
