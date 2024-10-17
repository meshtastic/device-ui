#ifdef ARCH_PORTDUINO

#include "LinuxInputDriver.h"
#include "ILog.h"
#include "screens.h"
#include <glob.h>
#include <unistd.h>

LV_IMG_DECLARE(mouse_cursor_icon);

LinuxInputDriver::LinuxInputDriver(const std::string &kbdDevice, const std::string &ptrDevice)
{
    keyboardDevice = kbdDevice;
    pointerDevice = ptrDevice;
}

void LinuxInputDriver::init(void)
{
    ILOG_DEBUG("LinuxInputDriver::init ...");
    if (!keyboardDevice.empty()) {
        useKeyboardDevice(keyboardDevice);
    } else
        keyboardDevice = "none";

    if (!pointerDevice.empty()) {
        usePointerDevice(pointerDevice);
    } else
        pointerDevice = "none";
}

void LinuxInputDriver::task_handler(void) {}

/**
 * @brief Scan devicetree for all keyboards
 *
 * @return std::vector<std::string>: list of keyboard input events
 */
std::vector<std::string> LinuxInputDriver::getKeyboardDevices(void)
{
    std::vector<std::string> kbd_path_list = globVector("/dev/input/by-id/*-event-kbd");
    std::vector<std::string> kbd_events;

    char buf[128] = {};
    for (std::string &s : kbd_path_list) {
        uint16_t nbytes;
        if ((nbytes = readlink(s.c_str(), buf, 128)) != -1) {
            buf[nbytes] = '\0';
            kbd_events.push_back(&buf[3]);
            // ILOG_DEBUG("%s -> %s", s.c_str(), &buf[3]);
        }
    }
    return kbd_events;
}

/**
 * @brief Scan devicetree for all pointer (mouse or touch) devices
 *
 * @return std::vector<std::string>: list of mouse input event paths
 */
std::vector<std::string> LinuxInputDriver::getPointerDevices(void)
{
    std::vector<std::string> ptr_path_list = globVector("/dev/input/by-id/*-event-mouse");
    std::vector<std::string> ptr_events;

    char buf[16] = {};
    for (std::string &s : ptr_path_list) {
        uint16_t nbytes;
        if ((nbytes = readlink(s.c_str(), buf, 16)) != -1) {
            buf[nbytes] = '\0';
            ptr_events.push_back(&buf[3]); // strip off "../"
            // ILOG_DEBUG("%s -> %s", s.c_str(), &buf[3]);
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
std::vector<std::string> LinuxInputDriver::globVector(const std::string &pattern)
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

/**
 * create input device for keyboard
 * name is either eventX or the full path to the input event
 */
bool LinuxInputDriver::useKeyboardDevice(const std::string &name)
{
    std::string kb_path;
    std::string event;

    if (name.at(0) != '/') {
        kb_path += "/dev/input/" + name;
        event = name;
    } else {
        char buf[16];
        int16_t nbytes;
        if ((nbytes = readlink(name.c_str(), buf, 16)) != -1) {
            buf[nbytes] = '\0';
            event = &buf[3];
        }
        kb_path = name;
    }

    keyboard = lv_libinput_create(LV_INDEV_TYPE_KEYPAD, kb_path.c_str());
    if (keyboard) {
        ILOG_INFO("Using keyboard device %s", kb_path.c_str());
        keyboardDevice = event;
    } else {
        ILOG_ERROR("Failed to use keyboard device %s", kb_path.c_str());
        keyboardDevice = "none";
    }

    return keyboard != nullptr;
}

/**
 * create input device for keyboard
 * name is either eventX or the full path to the input event
 */
bool LinuxInputDriver::usePointerDevice(const std::string &name)
{
    std::string ptr_path;
    std::string event;

    if (name.at(0) != '/') {
        event = name;
        ptr_path += "/dev/input/" + name;
    } else {
        char buf[16];
        int16_t nbytes;
        if ((nbytes = readlink(name.c_str(), buf, 16)) != -1) {
            buf[nbytes] = '\0';
            event = &buf[3];
        }
        ptr_path = name;
    }

    pointer = lv_libinput_create(LV_INDEV_TYPE_POINTER, ptr_path.c_str());
    if (pointer) {
        ILOG_INFO("Using pointer device %s", ptr_path.c_str());
        lv_obj_t *mouse_cursor = lv_image_create(lv_screen_active());
        lv_image_set_src(mouse_cursor, &mouse_cursor_icon);
        lv_indev_set_cursor(pointer, mouse_cursor);
        pointerDevice = event;
        return true;
    } else {
        ILOG_ERROR("Failed to use pointer device %s", ptr_path.c_str());
        pointerDevice = "none";
    }
    return false;
}

bool LinuxInputDriver::releaseKeyboardDevice(void)
{
    ILOG_INFO("Releasing keyboard device %s", keyboardDevice.c_str());
    lv_indev_delete(keyboard);
    keyboard = nullptr;
    keyboardDevice = "none";
    return true;
}

bool LinuxInputDriver::releasePointerDevice(void)
{
    ILOG_INFO("Releasing pointer device %s", pointerDevice.c_str());
    if (mouse_cursor) {
        lv_obj_add_flag(mouse_cursor, LV_OBJ_FLAG_HIDDEN);
        lv_obj_delete(mouse_cursor);
        mouse_cursor = nullptr;
    }
    lv_indev_delete(pointer);
    pointer = nullptr;
    pointerDevice = "none";
    return true;
}

LinuxInputDriver::~LinuxInputDriver(void)
{
    if (keyboard)
        releaseKeyboardDevice();
    if (pointer)
        releasePointerDevice();
}

#endif