#if defined(ARCH_PORTDUINO) && LV_USE_LIBINPUT

#include "input/LinuxInputDriver.h"
#include "input/policy/DefaultInputPolicyFactory.h"
#include "input/policy/InputContextState.h"
#include "input/policy/InputPipeline.h"
#include "input/policy/InputSourceRegistry.h"
#include "input/policy/UICommandDispatcher.h"
#include "screens.h"
#include "util/ILog.h"
#include <glob.h>
#include <unistd.h>

LV_IMG_DECLARE(mouse_cursor_icon);

LinuxInputDriver::LinuxInputDriver(const std::string &kbdDevice, const std::string &ptrDevice, bool keyboardNavigation,
                                   DisplayDriverConfig::cursor_mode_t cursorMode)
    : keyboardNavigation(keyboardNavigation), cursorMode(cursorMode)
{
    ILOG_DEBUG("LinuxInputDriver: kbdDevice='%s' ptrDevice='%s'", kbdDevice.c_str(), ptrDevice.c_str());
    keyboardDevice = kbdDevice;
    pointerDevice = ptrDevice;
}

void LinuxInputDriver::init(void)
{
    ILOG_DEBUG("LinuxInputDriver::init ...");
    if (!inputGroup) {
        inputGroup = lv_group_create();
        lv_group_set_default(inputGroup);
    }

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

    if (keyboard)
        releaseKeyboardDevice();

    keyboard = lv_libinput_create(LV_INDEV_TYPE_KEYPAD, kb_path.c_str());
    if (keyboard) {
        ILOG_INFO("Using keyboard device %s(%s)", kb_path.c_str(), event.c_str());
        keyboardDevice = event;
        lv_indev_set_group(keyboard, inputGroup);
        if (keyboardNavigation) {
            initKeyboardPolicy();
            keyboardRead = lv_indev_get_read_cb(keyboard);
            keyPressed = false;
            pressedRawKey = pressedOutputKey = 0;
            lv_indev_set_user_data(keyboard, this);
            lv_indev_set_read_cb(keyboard, keyboard_read);
        }
    } else {
        ILOG_ERROR("Failed to use keyboard device %s", kb_path.c_str());
        keyboardDevice = "none";
    }

    return keyboard != nullptr;
}

void LinuxInputDriver::initKeyboardPolicy()
{
    if (keyboardPipeline)
        return;

    auto context = std::shared_ptr<input_policy::IInputContextProvider>(&input_policy::InputContextState::instance(),
                                                                        [](input_policy::IInputContextProvider *) {});
    auto dispatcher = std::shared_ptr<input_policy::IUICommandDispatcher>(&input_policy::UICommandDispatcher::instance(),
                                                                          [](input_policy::IUICommandDispatcher *) {});
    input_policy::InputSourceRegistry registry;
    input_policy::DefaultInputPolicyFactory factory;
    auto result = factory.build(registry, context, dispatcher);
    keyboardPipeline = std::make_shared<input_policy::InputPipeline>(result.bindingResolver, context, dispatcher);
    keyboardPipeline->setPolicyChain(std::move(result.chain));
}

void LinuxInputDriver::keyboard_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    auto *driver = static_cast<LinuxInputDriver *>(lv_indev_get_user_data(indev));
    driver->keyboardRead(indev, data);
    const uint32_t rawKey = data->key;

    if (data->state == LV_INDEV_STATE_RELEASED) {
        // A focus change during the press must not change its matching release key.
        if (rawKey != 0 && rawKey == driver->pressedRawKey)
            data->key = driver->pressedOutputKey;
        driver->keyPressed = false;
        return;
    }
    if (rawKey == 0)
        return;

    if (!driver->keyPressed || rawKey != driver->pressedRawKey) {
        static const input_policy::InputCapabilities capabilities = {true, true, true, true, true, true,
                                                                     true, true, true, true, true};
        input_policy::InputEvent event{};
        event.sourceId = "linux_keyboard";
        event.rawKeyCode = event.resolvedKeyCode = rawKey;
        event.timestampMs = lv_tick_get();
        // Linux text values can overlap TM9's private command codes.
        if ((rawKey >= 0x20 && rawKey <= 0x7e) || rawKey >= 0x100)
            event.action = input_policy::InputAction::Character;
        std::vector<input_policy::InputEvent> output;
        bool forward = driver->keyboardPipeline->process(event, capabilities, output);
        driver->pressedRawKey = rawKey;
        driver->pressedOutputKey = forward && !output.empty() ? output.front().resolvedKeyCode : 0;
        driver->keyPressed = true;
    }
    data->key = driver->pressedOutputKey;
    if (data->key == 0)
        data->state = LV_INDEV_STATE_RELEASED;
}

/**
 * create input device for pointer
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

    if (pointer)
        releasePointerDevice();

    pointer = lv_libinput_create(LV_INDEV_TYPE_POINTER, ptr_path.c_str());
    if (pointer) {
        ILOG_INFO("Using pointer device %s(%s)", ptr_path.c_str(), event.c_str());
        if (cursorMode != DisplayDriverConfig::cursor_mode_t::NEVER) {
            mouse_cursor = lv_image_create(lv_screen_active());
            lv_image_set_src(mouse_cursor, &mouse_cursor_icon);
            lv_indev_set_cursor(pointer, mouse_cursor);
        }
        lv_indev_set_group(pointer, lv_group_get_default());
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
    if (!keyboard)
        return true;
    ILOG_INFO("Releasing keyboard device %s", keyboardDevice.c_str());
    lv_libinput_delete(keyboard);
    keyboard = nullptr;
    keyboardRead = nullptr;
    keyPressed = false;
    keyboardDevice = "none";
    return true;
}

bool LinuxInputDriver::releasePointerDevice(void)
{
    if (!pointer)
        return true;
    ILOG_INFO("Releasing pointer device %s", pointerDevice.c_str());
    if (mouse_cursor) {
        lv_obj_add_flag(mouse_cursor, LV_OBJ_FLAG_HIDDEN);
        lv_obj_delete(mouse_cursor);
        mouse_cursor = nullptr;
    }
    lv_libinput_delete(pointer);
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
