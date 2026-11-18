#include "graphics/common/Ringtones.h"
#include "meshtastic/mesh.pb.h"

#include <doctest/doctest.h>

#include <cstddef>
#include <cstring>

namespace
{
// size of AdminMessage{set_ringtone_message} on the wire: field 37 takes a two byte key,
// the string length is a varint, followed by the string itself
size_t encodedRingtoneSize(size_t len)
{
    size_t varint = 1;
    for (size_t v = len; v >= 0x80; v >>= 7)
        varint++;
    return 2 + varint + len;
}
} // namespace

TEST_CASE("every built-in ringtone fits into a single admin packet")
{
    CHECK(encodedRingtoneSize(0x7f) == 2 + 1 + 0x7f);
    CHECK(encodedRingtoneSize(0x80) == 2 + 2 + 0x80);

    for (int i = 0; i < numRingtones; i++) {
        CAPTURE(i);
        CAPTURE(ringtone[i].name);
        CHECK(encodedRingtoneSize(strlen(ringtone[i].rtttl)) <= meshtastic_Constants_DATA_PAYLOAD_LEN);
    }
}
