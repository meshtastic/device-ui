#include "graphics/view/TFT/PacketAuthPolicy.h"
#include <doctest/doctest.h>

using PacketAuthPolicy::Selection;

TEST_CASE("packet authenticity wire values map to UI selections")
{
    CHECK(PacketAuthPolicy::fromWireValue(0) == Selection::Balanced);
    CHECK(PacketAuthPolicy::fromWireValue(1) == Selection::Compatible);
    CHECK(PacketAuthPolicy::fromWireValue(2) == Selection::Strict);
    CHECK(PacketAuthPolicy::fromWireValue(99) == Selection::Balanced);
    CHECK(PacketAuthPolicy::toWireValue(Selection::Balanced) == 0);
    CHECK(PacketAuthPolicy::toWireValue(Selection::Compatible) == 1);
    CHECK(PacketAuthPolicy::toWireValue(Selection::Strict) == 2);
}

TEST_CASE("only a transition into Strict requires confirmation")
{
    CHECK(PacketAuthPolicy::requiresStrictConfirmation(Selection::Balanced, Selection::Strict));
    CHECK(PacketAuthPolicy::requiresStrictConfirmation(Selection::Compatible, Selection::Strict));
    CHECK_FALSE(PacketAuthPolicy::requiresStrictConfirmation(Selection::Strict, Selection::Strict));
    CHECK_FALSE(PacketAuthPolicy::requiresStrictConfirmation(Selection::Strict, Selection::Balanced));
}
