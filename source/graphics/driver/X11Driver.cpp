#ifdef USE_X11
#include "graphics/driver/X11Driver.h"
#include "util/ILog.h"
#include <stdio.h>

// note: copied from local lv_x11_input.c to access keyboard/mouse/wheel
typedef struct _x11_inp_data {
    /* LVGL related information */
    lv_group_t *inp_group;    /**< input group for X input elements */
    lv_indev_t *keyboard;     /**< keyboard input device object */
    lv_indev_t *mousepointer; /**< mouse input device object */
    lv_indev_t *mousewheel;   /**< encoder input device object */
    lv_timer_t *timer;        /**< timer object for @ref x11_event_handler */
    /* user input related information */
    char kb_buffer[32];   /**< keyboard buffer for X keyboard inputs */
    lv_point_t mouse_pos; /**< current reported mouse position */
    bool left_mouse_btn;  /**< current state of left mouse button */
    bool right_mouse_btn; /**< current state of right mouse button */
    bool wheel_mouse_btn; /**< current state of wheel (=middle) mouse button */
    int16_t wheel_cnt;    /**< mouse wheel increments */
} x11_inp_data_t;

static x11_inp_data_t *x11_input_get_user_data(lv_display_t *disp)
{
    _x11_user_hdr_t *disp_hdr = (_x11_user_hdr_t *)lv_display_get_driver_data(disp);
    x11_inp_data_t **inp_data = &disp_hdr->inp_data;
    return *inp_data;
}

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

lv_indev_t *X11Driver::getKeyboard(void)
{
    x11_inp_data_t *xd = x11_input_get_user_data(x11driver->display);
    return xd->keyboard;
}

lv_indev_t *X11Driver::getPointer(void)
{
    x11_inp_data_t *xd = x11_input_get_user_data(x11driver->display);
    return xd->mousepointer;
}

lv_indev_t *X11Driver::getEncoder(void)
{
    x11_inp_data_t *xd = x11_input_get_user_data(x11driver->display);
    return xd->mousewheel;
}

lv_group_t *X11Driver::getGroup(void)
{
    x11_inp_data_t *xd = x11_input_get_user_data(x11driver->display);
    return xd->inp_group;
}
#endif