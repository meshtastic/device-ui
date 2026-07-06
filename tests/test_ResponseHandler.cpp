#include "graphics/common/ResponseHandler.h"
#include <doctest/doctest.h>

namespace
{
class FixedRequestIdResponseHandler : public ResponseHandler
{
  public:
    FixedRequestIdResponseHandler() : ResponseHandler(1000) {}

  protected:
    uint32_t generatePacketId(void) override { return generatedRequestId; }

  private:
    static constexpr uint32_t generatedRequestId = 0x0badcafe;
};
} // namespace

TEST_CASE("ResponseHandler keeps generated request id separate from destination id")
{
    FixedRequestIdResponseHandler requests;

    uint32_t callbackRequestId = 0;
    const uint32_t generatedRequestId =
        requests.addRequest(3, ResponseHandler::TextMessageRequest, reinterpret_cast<void *>(7),
                            [&callbackRequestId](const ResponseHandler::Request &req, ResponseHandler::EventType evt,
                                                 int32_t pass) {
                                callbackRequestId = req.requestId;
                                CHECK(req.id == 3);
                                CHECK(req.cookie == reinterpret_cast<void *>(7));
                                CHECK(evt == ResponseHandler::removed);
                                CHECK(pass == 42);
                            });

    CHECK(generatedRequestId == 0x0badcafe);

    const ResponseHandler::Request found = requests.findRequest(generatedRequestId);
    CHECK(found.requestId == generatedRequestId);
    CHECK(found.id == 3);

    requests.removeRequest(generatedRequestId, ResponseHandler::TextMessageRequest, 42);
    CHECK(callbackRequestId == generatedRequestId);
}
