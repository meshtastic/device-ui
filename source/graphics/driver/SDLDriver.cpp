#ifdef USE_SDL
#include "graphics/driver/SDLDriver.h"
#include "util/ILog.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

LV_IMG_DECLARE(mouse_cursor_icon);

SDLDriver *SDLDriver::SDLdriver = nullptr;

SDLDriver &SDLDriver::create(uint16_t width, uint16_t height)
{
    if (!SDLdriver)
        SDLdriver = new SDLDriver(width, height);
    return *SDLdriver;
}

SDLDriver::SDLDriver(uint16_t width, uint16_t height) : DisplayDriver(width, height) {}

void SDLDriver::init(DeviceGUI *gui)
{
    ILOG_DEBUG("SDLDriver::init...");
    // Initialize LVGL
    DisplayDriver::init(gui);

    display = lv_sdl_window_create(screenWidth, screenHeight);
    char title[25];
    sprintf(title, "Meshtastic (%dx%d)", screenWidth, screenHeight);
    lv_sdl_window_set_title(display, title);
    lv_sdl_mouse_create();
    lv_sdl_mousewheel_create();
    lv_sdl_keyboard_create();
}

void SDLDriver::task_handler(void)
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