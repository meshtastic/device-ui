#include "input/InputDriver.h"

InputDriver *InputDriver::driver = nullptr;
lv_indev_t *InputDriver::keyboard = nullptr;
lv_indev_t *InputDriver::pointer = nullptr;
lv_indev_t *InputDriver::encoder = nullptr;
lv_indev_t *InputDriver::button = nullptr;
lv_group_t *InputDriver::inputGroup = nullptr;

InputDriver *InputDriver::instance(void)
{
    if (!driver)
        driver = new InputDriver;
    return driver;
}

lv_indev_t *InputDriver::virtualPointer = nullptr;
lv_indev_t *InputDriver::virtualKeypad = nullptr;
InputDriver::InjectedTouch InputDriver::touchQueue[InputDriver::injectQueueLen];
std::atomic<uint8_t> InputDriver::touchHead{0}, InputDriver::touchTail{0};
uint32_t InputDriver::keyQueue[InputDriver::injectQueueLen];
std::atomic<uint8_t> InputDriver::keyHead{0}, InputDriver::keyTail{0};

// Runs last in DeviceGUI::init, unconditionally: the virtual devices must
// exist even on boards with no physical input, and the default group must
// exist before ui_init() so widgets self-register for key navigation.
void InputDriver::init(void)
{
    if (!inputGroup) {
        inputGroup = lv_group_create();
        lv_group_set_default(inputGroup);
    }
    if (!virtualKeypad) {
        virtualKeypad = lv_indev_create();
        lv_indev_set_type(virtualKeypad, LV_INDEV_TYPE_KEYPAD);
        lv_indev_set_read_cb(virtualKeypad, virtualKeypadRead);
        lv_indev_set_group(virtualKeypad, inputGroup);
    }
    if (!virtualPointer) {
        virtualPointer = lv_indev_create();
        lv_indev_set_type(virtualPointer, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(virtualPointer, virtualPointerRead);
    }
}

void InputDriver::injectTouch(int16_t x, int16_t y, uint16_t holdMs)
{
    uint8_t tail = touchTail.load(std::memory_order_relaxed);
    uint8_t next = (tail + 1) % injectQueueLen;
    if (next == touchHead.load(std::memory_order_acquire))
        return; // full; drop
    touchQueue[tail] = {x, y, holdMs};
    touchTail.store(next, std::memory_order_release);
}

void InputDriver::injectKey(uint32_t key)
{
    uint8_t tail = keyTail.load(std::memory_order_relaxed);
    uint8_t next = (tail + 1) % injectQueueLen;
    if (next == keyHead.load(std::memory_order_acquire))
        return; // full; drop
    keyQueue[tail] = key;
    keyTail.store(next, std::memory_order_release);
}

// LVGL thread. A queued touch is held PRESSED until holdMs elapses (at least
// one read cycle), then RELEASED once before the next queue entry starts.
void InputDriver::virtualPointerRead(lv_indev_t *indev, lv_indev_data_t *data)
{
    static bool pressing = false;
    static bool needRelease = false;
    static uint32_t pressStart = 0;
    static lv_point_t last = {0, 0};

    if (pressing) {
        const InjectedTouch &t = touchQueue[touchHead.load(std::memory_order_relaxed)];
        if (lv_tick_elaps(pressStart) >= t.holdMs) {
            pressing = false;
            needRelease = true;
            touchHead.store((touchHead.load(std::memory_order_relaxed) + 1) % injectQueueLen, std::memory_order_release);
        }
        data->point = last;
        data->state = LV_INDEV_STATE_PRESSED;
        return;
    }
    if (needRelease) {
        needRelease = false;
        data->point = last;
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }
    if (touchHead.load(std::memory_order_relaxed) != touchTail.load(std::memory_order_acquire)) {
        const InjectedTouch &t = touchQueue[touchHead.load(std::memory_order_relaxed)];
        last.x = t.x;
        last.y = t.y;
        pressing = true;
        pressStart = lv_tick_get();
        data->point = last;
        data->state = LV_INDEV_STATE_PRESSED;
        return;
    }
    data->point = last;
    data->state = LV_INDEV_STATE_RELEASED;
}

void InputDriver::virtualKeypadRead(lv_indev_t *indev, lv_indev_data_t *data)
{
    static bool needRelease = false;
    static uint32_t lastKey = 0;

    if (needRelease) {
        needRelease = false;
        data->key = lastKey;
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }
    if (keyHead.load(std::memory_order_relaxed) != keyTail.load(std::memory_order_acquire)) {
        lastKey = keyQueue[keyHead.load(std::memory_order_relaxed)];
        keyHead.store((keyHead.load(std::memory_order_relaxed) + 1) % injectQueueLen, std::memory_order_release);
        needRelease = true;
        data->key = lastKey;
        data->state = LV_INDEV_STATE_PRESSED;
        return;
    }
    data->key = lastKey;
    data->state = LV_INDEV_STATE_RELEASED;
}

InputDriver::~InputDriver(void)
{
    if (keyboard)
        releaseKeyboardDevice();
    if (pointer)
        releasePointerDevice();
}
