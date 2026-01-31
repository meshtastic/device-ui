#pragma once

#include <string>

#include "lvgl.h"
#include <vector>

/**
 * @brief This base class merges all input devices of its children
 *        into one "set". It allows to create several instances of different(!)
 *        input devices types that are accessible through this common base class.
 *        There is one global input group creates that serves all input devices. This
 *        input group behaves according the grid/flex layout of all created widgets.
 *        The view just needs to apply the focus to the appropriate widget when a panel
 *        is arranged into forground or background.
 */
class InputDriver
{
  public:
    static InputDriver *instance(void);
    virtual void init(void) {}
    virtual void task_handler(void) {}
    virtual ~InputDriver(void);

    virtual std::vector<std::string> getKeyboardDevices(void) { return std::vector<std::string>(); }
    virtual std::vector<std::string> getPointerDevices(void) { return std::vector<std::string>(); }

    std::string getCurrentKeyboardDevice(void) { return keyboardDevice; }
    std::string getCurrentPointerDevice(void) { return pointerDevice; }

    virtual bool useKeyboardDevice(const std::string &name) { return false; }
    virtual bool usePointerDevice(const std::string &name) { return false; }

    virtual bool releaseKeyboardDevice(void) { return keyboard == nullptr; }
    virtual bool releasePointerDevice(void) { return pointer == nullptr; }

    virtual bool hasKeyboardDevice(void) { return keyboard != nullptr; }
    virtual bool hasPointerDevice(void) { return pointer != nullptr; }
    virtual bool hasEncoderDevice(void) { return encoder != nullptr; }
    virtual bool hasButtonDevice(void) { return button != nullptr; }

    virtual lv_indev_t *getKeyboard(void) { return keyboard; }
    virtual lv_indev_t *getPointer(void) { return pointer; }
    virtual lv_indev_t *getEncoder(void) { return encoder; }
    virtual lv_indev_t *getButton(void) { return button; }

    static lv_group_t *getInputGroup(void) { return inputGroup; }

  protected:
    InputDriver(void) : keyboardDevice("none"), pointerDevice("none") {}
    static InputDriver *driver;
    static lv_indev_t *keyboard;
    static lv_indev_t *pointer;
    static lv_indev_t *encoder;
    static lv_indev_t *button;
    static lv_group_t *inputGroup;

    // used for linux hot plugging and unplugging
    std::string keyboardDevice; // current keyboard device string in use
    std::string pointerDevice;  // current pointer device string in use
};
