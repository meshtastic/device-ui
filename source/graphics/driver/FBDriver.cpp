#ifdef USE_FRAMEBUFFER

#include "graphics/driver/FBDriver.h"
#include "src/core/lv_global.h"
#include "util/ILog.h"
#include <cstdlib>
#include <cstring>

LV_IMG_DECLARE(mouse_cursor_icon);

FBDriver *FBDriver::fbDriver = nullptr;
lv_display_t *FBDriver::display = nullptr;

FBDriver &FBDriver::create(uint16_t width, uint16_t height)
{
    if (!fbDriver)
        fbDriver = new FBDriver(width, height);
    return *fbDriver;
}

FBDriver::FBDriver(uint16_t width, uint16_t height) : DisplayDriver(width, height) {}

void FBDriver::init(DeviceGUI *gui)
{
    ILOG_DEBUG("FBDriver::init...");
    // Initialize LVGL
    DisplayDriver::init(gui);

    // Linux frame buffer device init
    const char *device = getenv("LV_LINUX_FBDEV_DEVICE");
    device = (device != nullptr && strlen(device) > 0) ? device : "/dev/fb0";
    display = lv_linux_fbdev_create();

    if (display == nullptr) {
        ILOG_CRIT("Failed to initialize %d", device);
        return;
    }
    lv_linux_fbdev_set_file(display, device);

#if LV_USE_EVDEV
    // discover input devices
    lv_evdev_discovery_start(discovery_cb, display);
#endif
}

#if LV_USE_EVDEV
void FBDriver::discovery_cb(lv_indev_t *indev, lv_evdev_type_t type, void *user_data)
{
    ILOG_INFO("'%s' device discovered", type == LV_EVDEV_TYPE_REL   ? "REL"
                                        : type == LV_EVDEV_TYPE_ABS ? "ABS"
                                        : type == LV_EVDEV_TYPE_KEY ? "KEY"
                                                                    : "???");

    lv_display_t *disp = (lv_display_t *)user_data;
    lv_indev_set_display(indev, disp);

    if (type == LV_EVDEV_TYPE_REL) {
        set_mouse_cursor_icon(indev, disp);
    }
}

void FBDriver::set_mouse_cursor_icon(lv_indev_t *indev, lv_display_t *display)
{
    // Set the cursor icon
    LV_IMAGE_DECLARE(mouse_cursor_icon);
    lv_obj_t *cursor_obj = lv_image_create(lv_display_get_screen_active(display));
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(indev, cursor_obj);

    // delete the mouse cursor icon if the device is removed
    lv_indev_add_event_cb(indev, indev_deleted_cb, LV_EVENT_DELETE, cursor_obj);
}

void FBDriver::indev_deleted_cb(lv_event_t *e)
{
    if (LV_GLOBAL_DEFAULT()->deinit_in_progress)
        return;
    lv_obj_t *cursor_obj = (lv_obj_t *)lv_event_get_user_data(e);
    lv_obj_del(cursor_obj);
}
#endif

#endif