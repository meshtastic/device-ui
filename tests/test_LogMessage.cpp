#include "util/LogMessage.h"
#include <algorithm>
#include <array>
#include <doctest/doctest.h>
#include <vector>

TEST_CASE("LogMessage text payload leaves room for deserialize terminator")
{
    CHECK(maxLogMessagePayloadLength == messagePayloadSize - 1);
    CHECK(fitsLogMessagePayload(maxLogMessagePayloadLength));
    CHECK_FALSE(fitsLogMessagePayload(messagePayloadSize));

    std::array<uint8_t, messagePayloadSize> payload{};
    payload.fill('x');

    LogMessageEnv stored(1, 2, 0, 0, LogMessage::eDefault, false, maxLogMessagePayloadLength, payload.data());

    std::vector<uint8_t> serialized;
    stored.serialize([&serialized](const uint8_t *data, size_t len) {
        serialized.insert(serialized.end(), data, data + len);
        return len;
    });

    size_t offset = 0;
    LogMessageEnv restored;
    restored.deserialize([&serialized, &offset](uint8_t *data, size_t len) {
        std::copy(serialized.begin() + offset, serialized.begin() + offset + len, data);
        offset += len;
        return len;
    });

    CHECK(restored.length() == maxLogMessagePayloadLength);
    CHECK(restored.bytes[maxLogMessagePayloadLength - 1] == 'x');
    CHECK(restored.bytes[maxLogMessagePayloadLength] == 0);
}
