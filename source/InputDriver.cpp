#include "InputDriver.h"
#include "ILog.h"
#include "screens.h"

LV_IMG_DECLARE(mouse_cursor_icon);

InputDriver::InputDriver(void)
{
}

void InputDriver::init(void)
{

#if LV_USE_LIBINPUT
    // FIXME: fix scanner or pass input device through config interface
    ILOG_DEBUG("Scanning input devices...\n");
    char *mouse_path = /*"/dev/input/event9"; */ lv_libinput_find_dev(LV_LIBINPUT_CAPABILITY_POINTER, true);
    if (mouse_path) {
        ILOG_DEBUG("Using pointer device %s\n", mouse_path); // e.g. "/dev/input/event9"
        pointer = lv_libinput_create(LV_INDEV_TYPE_POINTER, mouse_path);

        lv_obj_t *mouse_cursor = lv_image_create(lv_screen_active());
        lv_image_set_src(mouse_cursor, &mouse_cursor_icon);
        lv_indev_set_cursor(pointer, mouse_cursor);
    }
    char *kb_path = /*"/dev/input/event5"; */ lv_libinput_find_dev(LV_LIBINPUT_CAPABILITY_KEYBOARD, true);
    if (kb_path) {
        ILOG_DEBUG("Using keyboard device %s\n", kb_path); // e.g. "/dev/input/event5"
        keyboard = lv_libinput_create(LV_INDEV_TYPE_KEYPAD, kb_path);
    }

#endif
}

void InputDriver::task_handler(void)
{
}
