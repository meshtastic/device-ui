#if defined(T_DECK) && defined(LVGL_DRIVER)

#include "graphics/LVGL/LVGL_T_DECK.h"
#include "drivers/display/st7789/lv_st7789.h"
#include "graphics/LVGL/LVGLSpiBusDriver.h"
#include "hal/spi_types.h"

LVGL_TDECK::LVGL_TDECK(void) : LVGL_ST7789(c_screenWidth, c_screenHeight)
{
    ILOG_DEBUG("LVGL_TDECK::LVGL_TDECK()...");
    bl = 42;
}

LVGL_TDECK::LVGL_TDECK(const DisplayDriverConfig &cfg) : LVGL_ST7789(cfg) {}

void LVGL_TDECK::init(void)
{
    ILOG_DEBUG("LVGL_TDECK::init...");
    LVGL_ST7789::init();
    driver->init(SPI2_HOST, 40, 41, 38, 11, -1, 12);
}

void LVGL_TDECK::touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
    ILOG_DEBUG("LVGL_TDECK::touchpad_read()");
}

#endif