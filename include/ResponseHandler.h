#pragma once

#include <map>
#include <stdint.h>

/**
 * @brief Generic class that stores an id together with a timestamp
 *        and returns a requestId(cookie).
 */
class ResponseHandler
{
  public:
    ResponseHandler(uint32_t timeout);
    uint32_t addRequest(uint32_t id);
    uint32_t findRequest(uint32_t requestId);
    uint32_t removeRequest(uint32_t requestId);

    void task_handler(void);

  protected:
    struct Request {
        uint32_t id;
        unsigned long timestamp;
    };

    uint32_t requestIdCounter;
    uint32_t maxTime;
    std::map<uint32_t, Request> pendingRequest;
};