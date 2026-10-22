#ifdef USE_X11
#include "graphics/driver/X11Driver.h"
#include "src/display/lv_display_private.h"
#include "util/ILog.h"
#include <stdio.h>

LV_IMG_DECLARE(mouse_cursor_icon);

X11Driver *X11Driver::x11driver = nullptr;
static lv_display_flush_cb_t x11Flush = nullptr;

X11Driver &X11Driver::create(uint16_t width, uint16_t height)
{
    if (!x11driver)
        x11driver = new X11Driver(width, height);
    return *x11driver;
}

X11Driver::X11Driver(uint16_t width, uint16_t height) : DisplayDriver(width, height) {}

void X11Driver::init(DeviceGUI *gui)
{
    ILOG_DEBUG("X11Driver::init...");
    // Initialize LVGL
    DisplayDriver::init(gui);

    char title[25];
    sprintf(title, "Meshtastic (%dx%d)", screenWidth, screenHeight);
    display = lv_x11_window_create(title, screenWidth, screenHeight);
    if (!display)
        return;
    // LVGL's X11 driver owns the flush callback; wrap it so the frame observer sees each area too,
    // copied before the original hands the buffer back
    x11Flush = display->flush_cb;
    lv_display_set_flush_cb(display, [](lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
        x11driver->flush(disp, area, px_map);
        x11Flush(disp, area, px_map);
    });
    lv_x11_inputs_create(display, &mouse_cursor_icon);
}

#endif