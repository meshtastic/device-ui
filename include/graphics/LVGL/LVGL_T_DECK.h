#pragma once

#ifdef LVGL_DRIVER

#include "graphics/driver/LVGL_ST7789.h"
#include "lvgl.h"
#include "stdint.h"

class LVGLSpiBusDriver;

class LVGL_TDECK : public LVGL_ST7789
{
  public:
    static constexpr uint16_t c_screenWidth = 320;
    static constexpr uint16_t c_screenHeight = 240;

    LVGL_TDECK(void);
    LVGL_TDECK(const DisplayDriverConfig &cfg);

    void init(void) override;
    bool hasButton(void) const override { return true; }
    bool hasTouch(void) const override { return true; }
    bool light(void) override { return true; }
    uint8_t getBrightness(void) const override { return 150; }
    void setBrightness(uint8_t setBrightness) override {}

    void touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data);

    ~LVGL_TDECK() {}

  protected:
  private:
};

#endif
