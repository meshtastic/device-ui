#pragma once

#ifdef LVGL_DRIVER

#include "drivers/display/st7789/lv_st7789.h"
#include "graphics/driver/DisplayDeviceDriver.h"
#include "graphics/driver/DisplayDriverConfig.h"

class LVGL_ST7789_Driver : public DisplayDeviceDriver
{
  public:
    LVGL_ST7789_Driver(uint16_t width, uint16_t height);
    void init(int16_t spiBus, int16_t sclk, int16_t mosi, int16_t miso, int16_t dc, int16_t rst, int16_t cs);
    lv_display_t *create(uint32_t hor_res, uint32_t ver_res);

  protected:
  private:
    LVGL_ST7789_Driver(const LVGL_ST7789_Driver &) = delete;
    LVGL_ST7789_Driver &operator=(const LVGL_ST7789_Driver &) = delete;

    static void lcd_send_cmd_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size);
    static void lcd_send_color_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size);
};

#endif