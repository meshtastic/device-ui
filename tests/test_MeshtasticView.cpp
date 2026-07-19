#include "graphics/common/MeshtasticView.h"
#include <doctest/doctest.h>

namespace {

class TestMeshtasticView : public MeshtasticView
{
  public:
    TestMeshtasticView() : MeshtasticView(nullptr, nullptr, nullptr) {}

    bool decodePsk(const std::string &base64, uint8_t *bytes, uint16_t &size, uint16_t capacity)
    {
        return base64ToPsk(base64, bytes, size, capacity);
    }
};

} // namespace

TEST_CASE("MeshtasticView rejects a decoded PSK larger than its destination")
{
    TestMeshtasticView view;
    uint8_t bytes[32] = {};
    uint16_t size = 0;

    CHECK_FALSE(view.decodePsk(std::string(44, 'A'), bytes, size, sizeof(bytes)));
    CHECK(size == 0);
}
