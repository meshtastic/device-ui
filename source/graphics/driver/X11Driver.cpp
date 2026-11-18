#ifdef USE_X11
#include "graphics/driver/X11Driver.h"
#include "src/display/lv_display_private.h"
#include "util/ILog.h"
#include <stdio.h>
#include <thread>

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
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

#endif