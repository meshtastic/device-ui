#if defined(USE_SDL) && USE_SDL
#include "graphics/driver/SDLDriver.h"
#include "input/InputDriver.h"
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
    lv_sdl_window_set_resizeable(display, true);

    lv_indev_t *mouse = lv_sdl_mouse_create();
    lv_indev_t *wheel = lv_sdl_mousewheel_create();
    lv_indev_t *keyboard = lv_sdl_keyboard_create();

    // SDL key events are delivered as KEYPAD input; they need a focused group.
    lv_group_t *group = lv_group_get_default();
    if (!group) {
        group = lv_group_create();
        lv_group_set_default(group);
    }

    if (keyboard)
        lv_indev_set_group(keyboard, group);
    if (mouse)
        lv_indev_set_group(mouse, group);
    if (wheel)
        lv_indev_set_group(wheel, group);
}

void SDLDriver::task_handler(void)
{
    uint32_t waitMs = lv_timer_handler();
    if (waitMs > 5)
        waitMs = 5;
    std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
}

#endif