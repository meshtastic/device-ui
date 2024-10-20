#pragma once

#include <stdint.h>
#include <unordered_map>

/**
 * @brief Generic class that stores an id together with a timestamp
 *        and returns a requestId(cookie).
 */
class ResponseHandler
{
  public:
    enum RequestType { noRequest, TextMessageRequest, TraceRouteRequest, PositionRequest, RemoteConfigRequest };

    struct Request {
        uint32_t id;
        void *cookie;
        enum RequestType type;
        unsigned long timestamp;
    };

    ResponseHandler(uint32_t timeout);
    uint32_t addRequest(uint32_t id, RequestType type, void *cookie = nullptr);
    Request findRequest(uint32_t requestId);
    Request removeRequest(uint32_t requestId);

    void task_handler(void);

  protected:
    uint32_t generatePacketId(void);

    static uint32_t rollingPacketId;
    uint32_t requestIdCounter;
    uint32_t maxTime;
    std::unordered_map<uint32_t, Request> pendingRequest;
};