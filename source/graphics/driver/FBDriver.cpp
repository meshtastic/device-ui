#ifdef USE_FRAMEBUFFER

#include "graphics/driver/FBDriver.h"
#include "src/core/lv_global.h"
#include "util/ILog.h"
#include <cstdlib>
#include <cstring>
#if LV_USE_EVDEV
#include <linux/input.h>
#include <sys/ioctl.h>
#endif

LV_IMG_DECLARE(mouse_cursor_icon);

FBDriver *FBDriver::fbDriver = nullptr;
lv_display_t *FBDriver::display = nullptr;

FBDriver &FBDriver::create(uint16_t width, uint16_t height)
{
    if (!fbDriver)
        fbDriver = new FBDriver(width, height);
    return *fbDriver;
}

FBDriver &FBDriver::create(const DisplayDriverConfig &cfg)
{
    if (!fbDriver)
        fbDriver = new FBDriver(cfg.width(), cfg.height(), toRotation(cfg._panel.offset_rotation), cfg.cursorMode());
    return *fbDriver;
}

FBDriver::FBDriver(uint16_t width, uint16_t height, lv_display_rotation_t rotation, DisplayDriverConfig::cursor_mode_t cursorMode)
    : DisplayDriver(width, height), rotation(rotation), cursorMode(cursorMode)
{
}

lv_display_rotation_t FBDriver::toRotation(uint8_t offsetRotation)
{
    switch (offsetRotation) {
    case 0:
        return LV_DISPLAY_ROTATION_0;
    case 1:
        return LV_DISPLAY_ROTATION_90;
    case 2:
        return LV_DISPLAY_ROTATION_180;
    case 3:
        return LV_DISPLAY_ROTATION_270;
    default:
        ILOG_WARN("FBDriver: unsupported rotation %d, using 0 degrees", offsetRotation);
        return LV_DISPLAY_ROTATION_0;
    }
}

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
    // config driven rotation, e.g. from the YAML (Display.Rotate + Display.OffsetRotate) of meshtasticd
    if (rotation != LV_DISPLAY_ROTATION_0)
        lv_display_set_rotation(display, rotation);

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

    const auto mode = fbDriver->cursorMode;
    if (type == LV_EVDEV_TYPE_REL && mode != DisplayDriverConfig::cursor_mode_t::NEVER &&
        (mode == DisplayDriverConfig::cursor_mode_t::ALWAYS || is_mouse(indev))) {
        set_mouse_cursor_icon(indev, disp);
        ILOG_INFO("Mouse cursor enabled for relative pointing device");
    } else if (type == LV_EVDEV_TYPE_REL) {
        ILOG_DEBUG("Cursor hidden for relative input device");
    }
}

bool FBDriver::is_mouse(lv_indev_t *indev)
{
#if defined(LV_EVDEV_HAS_FD_GETTER) && LV_EVDEV_HAS_FD_GETTER
    if (lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER)
        return false;
    const int fd = lv_evdev_get_fd(indev);
    constexpr unsigned bitsPerWord = sizeof(unsigned long) * 8;
    unsigned long keys[BTN_LEFT / bitsPerWord + 1] = {};
    // HDMI CEC and IR devices can advertise relative axes without being mice.
    return fd >= 0 && ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keys)), keys) >= 0 &&
           (keys[BTN_LEFT / bitsPerWord] & (1ul << (BTN_LEFT % bitsPerWord))) != 0;
#else
    // Older LVGL releases do not expose evdev capabilities; retain their cursor behavior.
    return lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER;
#endif
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
