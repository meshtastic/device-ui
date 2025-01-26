#pragma once

#include "graphics/LVGL/LVGLDriver.h"
#include "graphics/LVGL/LVGL_ST7789_Driver.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "strings.h"
#include "util/ILog.h"

class DisplayDeviceDriver;

/**
 * @brief Runtime configuration class for lvgl driver
 *
 */
class LVGLConfig
{
  public:
    uint16_t screenWidth = 0;
    uint16_t screenHeight = 0;

    LVGLConfig(void);
    LVGLConfig(const DisplayDriverConfig &cfg);
    void init(void);
    lv_display_t *create(uint32_t hor_res, uint32_t ver_res);
    void touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data);

    bool hasButton(void);
    bool hasTouch(void);
    bool light(void);

    uint8_t getBrightness(void);
    void setBrightness(uint8_t setBrightness);

    ~LVGLConfig();

  protected:
  private:
    const DisplayDriverConfig *config;
    DisplayDeviceDriver *lvglDeviceDriver;
};
