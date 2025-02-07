#if defined(LV_USE_ST7789) && LV_USE_ST7789 == 1

#include "graphics/LVGL/LVGL_ST7789_Driver.h"
#include "util/ILog.h"

LVGL_ST7789_Driver::LVGL_ST7789_Driver(uint16_t width, uint16_t height) : DisplayDeviceDriver(width, height)
{
    ILOG_DEBUG("LVGL_ST7789_Driver()...");
}

void LVGL_ST7789_Driver::init(int16_t spiBus, int16_t sclk, int16_t mosi, int16_t miso, int16_t dc, int16_t rst, int16_t cs)
{
    ILOG_DEBUG("LVGL_ST7789_Driver::init()...");
}

/**
 * Create an LCD display with ST7789 driver
 * @param hor_res       horizontal resolution
 * @param ver_res       vertical resolution
 * @param flags         default configuration settings (mirror, RGB ordering, etc.)
 * @param send_cmd      platform-dependent function to send a command to the LCD controller (usually uses polling transfer)
 * @param send_color    platform-dependent function to send pixel data to the LCD controller (usually uses DMA transfer: must
 *                      implement a 'ready' callback)
 * @return              pointer to the created display
 */
lv_display_t *LVGL_ST7789_Driver::create(uint32_t hor_res, uint32_t ver_res)
{
    ILOG_DEBUG("lv_st7789_create...");
    lv_display_t *display = lv_st7789_create(hor_res, ver_res, LV_LCD_FLAG_NONE, lcd_send_cmd_cb, lcd_send_color_cb);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    return display;
}

/**
 * Send short command to the LCD.
 * This function shall wait until the transaction finishes.
 *
 */
void LVGL_ST7789_Driver::lcd_send_cmd_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param,
                                         size_t param_size)
{
}

/**
 * Send large array of pixel data to the LCD. If necessary, this function has to do the byte-swapping.
 * This function can do the transfer in the background.
 *
 */
void LVGL_ST7789_Driver::lcd_send_color_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param,
                                           size_t param_size)
{
}

#endif
