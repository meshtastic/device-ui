#pragma once

#include <stdint.h>

namespace PacketAuthPolicy
{
enum class Selection : uint8_t {
    Balanced = 0,
    Compatible = 1,
    Strict = 2,
};

constexpr Selection fromWireValue(uint32_t value)
{
    return value <= static_cast<uint32_t>(Selection::Strict) ? static_cast<Selection>(value) : Selection::Balanced;
}

constexpr uint32_t toWireValue(Selection selection)
{
    return static_cast<uint32_t>(selection);
}

constexpr bool requiresStrictConfirmation(Selection current, Selection requested)
{
    return requested == Selection::Strict && current != Selection::Strict;
}

constexpr const char *label(Selection selection)
{
    switch (selection) {
    case Selection::Compatible:
        return "Compatible";
    case Selection::Strict:
        return "Strict";
    case Selection::Balanced:
    default:
        return "Balanced";
    }
}
} // namespace PacketAuthPolicy
