#ifdef USE_X11
#include "X11Driver.h"
#include "ILog.h"
#include <stdio.h>

LV_IMG_DECLARE(mouse_cursor_icon);

X11Driver *X11Driver::x11driver = nullptr;

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

    char title[24];
    sprintf(title, "Meshtastic (%dx%d)", screenWidth, screenHeight);
    display = lv_x11_window_create(title, screenWidth, screenHeight);
    lv_x11_inputs_create(display, &mouse_cursor_icon);
}

#endif