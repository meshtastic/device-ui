#include "LVGLGraphics.h"
#include "ILog.h"
#include "assert.h"

LVGLGraphics::LVGLGraphics(uint16_t width, uint16_t height) : screenWidth(width), screenHeight(height) {}

void LVGLGraphics::init(void)
{
    ILOG_DEBUG("LV init...");
#if LV_USE_LOG
    lv_log_register_print_cb(lv_debug);
#endif
    lv_init();
#if LV_USE_LOG
    lv_log_register_print_cb(lv_debug);
#endif
}

// debugging callback
void LVGLGraphics::lv_debug(lv_log_level_t level, const char *buf)
{
    switch (level) {
    case LV_LOG_LEVEL_TRACE: {
        ILOG_DEBUG("%s", buf);
        break;
    }
    case LV_LOG_LEVEL_INFO: {
        ILOG_INFO("%s", buf);
        break;
    }
    case LV_LOG_LEVEL_WARN: {
        ILOG_WARN("%s", buf);
        break;
    }
    case LV_LOG_LEVEL_ERROR: {
        ILOG_ERROR("%s", buf);
        break;
    }
    default:
        ILOG_DEBUG("%s", buf);
        break;
    }
}
