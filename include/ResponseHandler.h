#pragma once

#include <functional>
#include <stdint.h>
#include <unordered_map>

/**
 * @brief Generic class that stores an id together with a timestamp, cookie and callback function.
 *        Returns a unique requestId for later reference.
 *        Callbacks can be used to decouple the reply handler from the actual caller.
 */
class ResponseHandler
{
  public:
    struct Request;

    enum RequestType { noRequest, TextMessageRequest, TraceRouteRequest, PositionRequest, RemoteConfigRequest, anyRequest };
    enum EventType { found, removed, timeout, user1, user2, user3 };

    using Callback = std::function<void(const Request &, EventType, int32_t)>;

    struct Request {
        uint32_t id;
        unsigned long timestamp;
        enum RequestType type;
        void *cookie;
        Callback cb;
    };

    ResponseHandler(uint32_t timeout);
    // add new request, pass optional anytype cookie and optional callback function
    virtual uint32_t addRequest(uint32_t id, RequestType type, void *cookie = nullptr, Callback cb = nullptr);
    // findRequest, call cb on found if pass != -1 and if request type matches
    virtual Request findRequest(uint32_t requestId, RequestType match = anyRequest, int32_t pass = -1);
    // removeRequest, call cb on found if pass != -1) and if event type matches
    virtual Request removeRequest(uint32_t requestId, RequestType match = anyRequest, int32_t pass = -1);
    // custom request for implementing user function in derived class
    virtual Request customRequest(uint32_t requestId, RequestType match = anyRequest, int32_t pass = -1) { return Request{}; }
    // task handler, must be called periodically
    virtual void task_handler(void);

    ~ResponseHandler() = default;

  protected:
    virtual uint32_t generatePacketId(void);

    static uint32_t rollingPacketId;
    uint32_t requestIdCounter;
    uint32_t maxTime;
    std::unordered_map<uint32_t, Request> pendingRequest;

  private:
    ResponseHandler(const ResponseHandler &) = delete;
    ResponseHandler &operator=(const ResponseHandler &) = delete;
};