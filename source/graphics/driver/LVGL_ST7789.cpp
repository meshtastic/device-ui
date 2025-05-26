#if defined(LV_USE_ST7789) && defined(LVGL_DRIVER)

#include "graphics/driver/LVGL_ST7789.h"
#include "drivers/display/st7789/lv_st7789.h"
#include "graphics/LVGL/LVGLSpiBusDriver.h"
#include "hal/spi_types.h"

LVGL_ST7789::LVGL_ST7789(uint16_t width, uint16_t height) : DisplayDeviceDriver(width, height), driver(nullptr)
{
    ILOG_DEBUG("LVGL_ST7789::LVGL_ST7789()...");
    auto st7789Func = [](uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags, lv_lcd_send_cmd_cb_t send_cmd_cb,
                         lv_lcd_send_color_cb_t send_color_cb) {
        return lv_st7789_create(hor_res, ver_res, flags, send_cmd_cb, send_color_cb);
    };
    driver = new LVGLSpiBusDriver(screenWidth, screenHeight, st7789Func);
}

LVGL_ST7789::LVGL_ST7789(const DisplayDriverConfig &cfg) : DisplayDeviceDriver(cfg.width(), cfg.height()), driver(nullptr)
{
    ILOG_DEBUG("LVGL_ST7789::LVGL_ST7789() with config...");
    auto st7789Func = [](uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags, lv_lcd_send_cmd_cb_t send_cmd_cb,
                         lv_lcd_send_color_cb_t send_color_cb) {
        return lv_st7789_create(hor_res, ver_res, flags, send_cmd_cb, send_color_cb);
    };
    driver = new LVGLSpiBusDriver(cfg.width(), cfg.height(), st7789Func);
}

void LVGL_ST7789::init(void)
{
    ILOG_DEBUG("LVGL_ST7789::init()...");
}

void LVGL_ST7789::touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
    ILOG_DEBUG("LVGL_ST7789::touchpad_read()");
}

/**
 * create lvgl display
 */
lv_display_t *LVGL_ST7789::create(uint32_t hor_res, uint32_t ver_res)
{
    return driver->create(hor_res, ver_res);
}

LVGL_ST7789::~LVGL_ST7789()
{
    if (driver)
        delete driver;
}
#endif