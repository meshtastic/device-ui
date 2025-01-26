#pragma once

#ifdef LVGL_DRIVER

#include "graphics/LVGL/LVGLDriver.h"
#include "lvgl.h"
#include "stdint.h"

class LVGL_ST7789_Driver;

class LVGL_TDECK //: public LVGLDriver<LVGL_TDECK>
{
  public:
    static constexpr uint16_t screenWidth = 320;
    static constexpr uint16_t screenHeight = 240;

    LVGL_TDECK(void);
    LVGL_TDECK(const DisplayDriverConfig &cfg);

    void init(void);
    bool hasButton(void) { return true; }
    bool hasTouch(void) { return true; }
    bool light(void) { return true; }
    uint8_t getBrightness(void) { return 128; }
    void setBrightness(uint8_t setBrightness) {}

    lv_display_t *create(uint32_t hor_res, uint32_t ver_res);
    void touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data);

  protected:
  private:
    LVGL_ST7789_Driver *driver;
};

#endif
