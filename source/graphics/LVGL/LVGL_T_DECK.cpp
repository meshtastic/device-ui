#if defined(T_DECK) && defined(LVGL_DRIVER)

#include "graphics/LVGL/LVGL_T_DECK.h"
#include "drivers/display/st7789/lv_st7789.h"
#include "graphics/LVGL/LVGL_ST7789_Driver.h"
#include "hal/spi_types.h"

LVGL_TDECK::LVGL_TDECK(void) : driver(nullptr)
{
    driver = new LVGL_ST7789_Driver(screenWidth, screenHeight);
}

void LVGL_TDECK::init(void)
{
    ILOG_DEBUG("LVGL_TDECK::init...");
    driver->init(SPI2_HOST, 40, 41, 38, 11, -1, 12);
}

void LVGL_TDECK::touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
    ILOG_DEBUG("LVGL_TDECK::touchpad_read()");
}

/**
 * create lvgl display for T-Deck
 */
lv_display_t *LVGL_TDECK::create(uint32_t hor_res, uint32_t ver_res)
{
    return driver->create(hor_res, ver_res);
}

#endif