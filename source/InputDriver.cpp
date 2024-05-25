#ifdef ARCH_PORTDUINO

#include "InputDriver.h"
#include "ILog.h"
#include "screens.h"
#include <glob.h>
#include <unistd.h>

LV_IMG_DECLARE(mouse_cursor_icon);

InputDriver::InputDriver(void) {}

void InputDriver::init(void) {}

void InputDriver::task_handler(void) {}

/**
 * @brief Scan devicetree for all keyboards
 *
 * @return std::vector<std::string>: list of keyboard input events
 */
std::vector<std::string> InputDriver::getKeyboardDevices(void)
{
    std::vector<std::string> kbd_path_list = globVector("/dev/input/by-id/*-event-kbd");
    std::vector<std::string> kbd_events;

    char buf[128] = {};
    for (std::string &s : kbd_path_list) {
        uint16_t nbytes;
        if ((nbytes = readlink(s.c_str(), buf, 128)) != -1) {
            buf[nbytes] = '\0';
            kbd_events.push_back(&buf[3]);
            // LOG_DEBUG("%s -> %s\n", s.c_str(), &buf[3]);
        }
    }
    return kbd_events;
}

/**
 * @brief Scan devicetree for all pointer (mouse or touch) devices
 *
 * @return std::vector<std::string>: list of mouse input event paths
 */
std::vector<std::string> InputDriver::getPointerDevices(void)
{
    std::vector<std::string> ptr_path_list = globVector("/dev/input/by-id/*-event-mouse");
    std::vector<std::string> ptr_events;

    char buf[16] = {};
    for (std::string &s : ptr_path_list) {
        uint16_t nbytes;
        if ((nbytes = readlink(s.c_str(), buf, 16)) != -1) {
            buf[nbytes] = '\0';
            ptr_events.push_back(&buf[3]);
            // LOG_DEBUG("%s -> %s\n", s.c_str(), &buf[3]);
        }
    }
    return ptr_events;
}

/**
 * @brief return all files matching pattern
 *
 * @param pattern
 * @return std::vector<std::string>
 */
std::vector<std::string> InputDriver::globVector(const std::string &pattern)
{
    glob_t glob_result;
    glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);

    std::vector<std::string> files;
    for (unsigned int i = 0; i < glob_result.gl_pathc; i++) {
        files.push_back(std::string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return files;
}

bool InputDriver::useKeyboardDevice(const std::string &event)
{
    std::string kb_path = "/dev/input/" + event;
    ILOG_DEBUG("Using keyboard device %s\n", kb_path);
    keyboard = lv_libinput_create(LV_INDEV_TYPE_KEYPAD, kb_path.c_str());
    return keyboard != nullptr;
}

bool InputDriver::usePointerDevice(const std::string &event)
{
    std::string ptr_path = "/dev/input/" + event;
    ILOG_INFO("Using pointer device %s\n", ptr_path);
    pointer = lv_libinput_create(LV_INDEV_TYPE_POINTER, ptr_path.c_str());
    if (pointer) {
        lv_obj_t *mouse_cursor = lv_image_create(lv_screen_active());
        lv_image_set_src(mouse_cursor, &mouse_cursor_icon);
        lv_indev_set_cursor(pointer, mouse_cursor);
        return true;
    }
    return false;
}

bool InputDriver::hasKeyboardDevice(void)
{
    return keyboard != nullptr;
}

bool InputDriver::hasPointerDevice(void)
{
    return pointer != nullptr;
}

bool InputDriver::releaseKeyboardDevice(void)
{
    lv_indev_delete(keyboard);
    keyboard = nullptr;
}

bool InputDriver::releasePointerDevice(void)
{
    lv_indev_delete(pointer);
    pointer = nullptr;
}

#endif