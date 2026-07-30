#ifdef MUI_RUNTIME_ROTATION

#include "graphics/ScreenRotation.h"
#include "util/ILog.h"

#if defined(VIEW_240x320)
static const uint16_t kPrimaryWidth = 240;
static const uint16_t kPrimaryHeight = 320;
#else
static const uint16_t kPrimaryWidth = 320;
static const uint16_t kPrimaryHeight = 240;
#endif

ScreenRotation::Value ScreenRotation::current = ScreenRotation::Rotation0;
bool ScreenRotation::loaded = false;

void ScreenRotation::setLoaded(uint8_t raw)
{
    if (loaded)
        return;
    loaded = true;
    if (raw > (uint8_t)Rotation270) {
        ILOG_WARN("invalid stored screen rotation %u, using %u", raw, (unsigned)current);
        return;
    }
    current = (Value)raw;
    ILOG_INFO("screen rotation %u restored", (unsigned)current);
}

void ScreenRotation::load(void)
{
    if (!loaded) {
        loaded = true;
        ILOG_WARN("screen rotation not supplied, using %u", (unsigned)current);
    }
}

uint16_t ScreenRotation::width(Value v)
{
    return usesSecondaryTree(v) ? kPrimaryHeight : kPrimaryWidth;
}

uint16_t ScreenRotation::height(Value v)
{
    return usesSecondaryTree(v) ? kPrimaryWidth : kPrimaryHeight;
}

#endif // MUI_RUNTIME_ROTATION
