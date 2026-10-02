#pragma once

#include "lgfx/v1/panel/Panel_FrameBufferBase.hpp"

namespace lgfx
{
inline namespace v1
{
namespace experimental
{

struct Panel_FrameBufferBase : public lgfx::Panel_FrameBufferBase {
    virtual void *getFrameBuffer(uint8_t) const { return nullptr; }
    virtual bool presentFrameBuffer(const void *, uint16_t, uint16_t, uint16_t, uint16_t) { return false; }
    virtual void waitFrameBuffer() {}
};

} // namespace experimental
} // namespace v1
} // namespace lgfx
