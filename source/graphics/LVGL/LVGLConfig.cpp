#ifdef LVGL_DRIVER

#include "graphics/LVGL/LVGLConfig.h"
#include "util/ILog.h"
#include <assert.h>

LVGLConfig::LVGLConfig(void) : config(nullptr)
{
    ILOG_ERROR("ctor called without config");
    assert(0);
}

LVGLConfig::LVGLConfig(const DisplayDriverConfig &cfg) : config(&cfg)
{
    ILOG_DEBUG("LVGLConfig ...");
}

void LVGLConfig::init()
{
    ILOG_DEBUG("LVGLConfig::init() ...");
    if (strcasecmp(config->_panel.type, "ST7789") == 0) {
        lvglDeviceDriver = new LVGL_ST7789_Driver(config->_width, config->_height);
    } else {
        ILOG_CRIT("LVGL device panel support not yet implemented for '%s'", config->_panel.type);
        return;
    }

    //  TODO: allocate touch driver
    if (config->_touch.type && strcasecmp(config->_touch.type, "NOTOUCH") != 0) {
        ILOG_ERROR("LVGLConfig: touch not yet implemented!");
    }
}

lv_display_t *LVGLConfig::create(uint32_t hor_res, uint32_t ver_res)
{
    ILOG_DEBUG("LVGLConfig::create() ...");
    return lvglDeviceDriver->create(hor_res, ver_res);
}

void LVGLConfig::touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
    ILOG_DEBUG("LVGLConfig::touchpad_read()");
}

bool LVGLConfig::hasButton(void)
{
    return false; // TODO
}

bool LVGLConfig::hasTouch(void)
{
    return true; // TODO
}

bool LVGLConfig::light(void)
{
    return false; // TODO
}

uint8_t LVGLConfig::getBrightness(void)
{
    return 128; // TODO
}

void LVGLConfig::setBrightness(uint8_t setBrightness)
{
    // TODO
}

LVGLConfig::~LVGLConfig()
{
    delete lvglDeviceDriver;
}

#endif