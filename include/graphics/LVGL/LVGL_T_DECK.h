#pragma once

#ifdef LVGL_DRIVER

#include "graphics/LVGL/LVGLDriver.h"
#include "lvgl.h"
#include "stdint.h"

class LVGL_ST7789_Driver;

class LVGL_TDECK : public LVGLDriver<LVGL_TDECK>
{
  public:
    LVGL_TDECK(void);
    LVGL_TDECK(const DisplayDriverConfig &cfg);

    void init(void);
    bool hasButton(void) { return true; }
    bool light(void) { return true; }
    uint8_t getBrightness(void) { return 128; }

    static constexpr uint16_t screenWidth = 320;
    static constexpr uint16_t screenHeight = 240;

  protected:
    lv_display_t *create(uint32_t hor_res, uint32_t ver_res) override;

  private:
    static void lcd_send_cmd_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size);
    static void lcd_send_color_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size);

    LVGL_ST7789_Driver *driver;
};

#endif
