#if defined(LVGL_DRIVER)

#include "graphics/LVGL/LVGL_T_DECK.h"
#include "drivers/display/st7789/lv_st7789.h"
#include "graphics/LVGL/LVGL_ST7789_Driver.h"

LVGL_TDECK::LVGL_TDECK(void) : LVGLDriver(screenWidth, screenHeight), driver(nullptr) {}

/**
 * Create an LCD display with ST7789 driver
 * @param hor_res       horizontal resolution
 * @param ver_res       vertical resolution
 * @param flags         default configuration settings (mirror, RGB ordering, etc.)
 * @param send_cmd      platform-dependent function to send a command to the LCD controller (usually uses polling transfer)
 * @param send_color    platform-dependent function to send pixel data to the LCD controller (usually uses DMA transfer: must
 * implement a 'ready' callback)
 * @return              pointer to the created display
 */

void LVGL_TDECK::init(void)
{
    ILOG_DEBUG("LVGL_TDECK::init...\n");
    driver = new LVGL_ST7789_Driver();
}

/**
 * Send short command to the LCD.
 * This function shall wait until the transaction finishes.
 *
 */
void LVGL_TDECK::lcd_send_cmd_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
}

/**
 * Send large array of pixel data to the LCD. If necessary, this function has to do the byte-swapping.
 * This function can do the transfer in the background.
 *
 */
void LVGL_TDECK::lcd_send_color_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size) {}

/**
 * create lvgl display for T-Deck
 */
lv_display_t *LVGL_TDECK::create(uint32_t hor_res, uint32_t ver_res)
{
    return lv_st7789_create(hor_res, ver_res, LV_LCD_FLAG_NONE, lcd_send_cmd_cb, lcd_send_color_cb);
}

#endif