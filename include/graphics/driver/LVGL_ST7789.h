#pragma once

#ifdef LVGL_DRIVER

#include "graphics/LVGL/LVGLDriver.h"
#include "graphics/driver/DisplayDeviceDriver.h"
#include "stdint.h"

class LVGLSpiBusDriver;

class LVGL_ST7789 : public DisplayDeviceDriver
{
  public:
    LVGL_ST7789(uint16_t width, uint16_t height);
    LVGL_ST7789(const DisplayDriverConfig &cfg);

    void init(void) override;
    uint8_t getBrightness(void) const override { return 128; }
    void setBrightness(uint8_t setBrightness) override {}

    lv_display_t *create(uint32_t hor_res, uint32_t ver_res) override;
    void touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data);

    virtual ~LVGL_ST7789();

  protected:
    LVGLSpiBusDriver *driver;

  private:
};

#endif
