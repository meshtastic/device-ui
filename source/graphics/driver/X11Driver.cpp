#ifdef USE_X11
#include "graphics/driver/X11Driver.h"
#include "util/ILog.h"
#include <stdio.h>
#include <thread>

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

    char title[25];
    sprintf(title, "Meshtastic (%dx%d)", screenWidth, screenHeight);
    display = lv_x11_window_create(title, screenWidth, screenHeight);
    lv_x11_inputs_create(display, &mouse_cursor_icon);
}

/**
 * Measure how long it takes to call DisplayDriver::task_handler().
 * Then tell the lvgl library how long it took via lv_tick_inc().
 */
void X11Driver::task_handler(void)
{
    const int ms = 10;
    auto start = std::chrono::high_resolution_clock::now();
    DisplayDriver::task_handler();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    if (duration.count() < ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms - duration.count()));
        lv_tick_inc(ms);
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        lv_tick_inc(duration.count() + 1);
    }
}

#endif