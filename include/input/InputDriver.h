#pragma once

#include "lvgl.h"
#include <atomic>
#include <cstdint>
#include <string>
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
    virtual void init(void);
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

    // -- Remote input injection ------------------------------------------------
    // Lets a host (e.g. firmware bridging a client's remote-control events)
    // inject input as two always-present virtual devices: a pointer and a
    // group-attached keypad. Callable from any thread: events land in small
    // lock-free queues drained by the LVGL read callbacks. A tap holds
    // PRESSED for holdMs (0 = one read cycle); pass ~600 to synthesize a
    // long press. Keys take LV_KEY_* values or printable characters.
    static void injectTouch(int16_t x, int16_t y, uint16_t holdMs = 0);
    static void injectKey(uint32_t key);

    // Encoder rotation: this is what moves focus between widgets in a group
    // (LVGL delivers keypad UP/DOWN to the focused widget instead). Negative
    // steps focus backwards, positive forwards — matching the trackball driver.
    static void injectEncoder(int16_t steps);

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

  private:
    struct InjectedTouch {
        int16_t x, y;
        uint16_t holdMs;
    };
    static constexpr uint8_t injectQueueLen = 16; // power of two; SPSC ring

    static void virtualPointerRead(lv_indev_t *indev, lv_indev_data_t *data);
    static void virtualKeypadRead(lv_indev_t *indev, lv_indev_data_t *data);
    static void virtualEncoderRead(lv_indev_t *indev, lv_indev_data_t *data);

    static lv_indev_t *virtualPointer;
    static lv_indev_t *virtualKeypad;
    static lv_indev_t *virtualEncoder;
    static InjectedTouch touchQueue[injectQueueLen];
    static std::atomic<uint8_t> touchHead, touchTail;
    static uint32_t keyQueue[injectQueueLen];
    static std::atomic<uint8_t> keyHead, keyTail;
    static int8_t encoderQueue[injectQueueLen];
    static std::atomic<uint8_t> encoderHead, encoderTail;
};
