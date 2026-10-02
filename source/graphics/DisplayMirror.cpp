#include "graphics/DisplayMirror.h"
#include "graphics/driver/DisplayDriver.h"
#include "lvgl.h"
#include "src/indev/lv_indev_private.h"
#include "util/ILog.h"
#include <atomic>

namespace
{
struct Touch {
    int16_t x, y;
    uint16_t holdMs;
};
constexpr uint8_t queueLen = 16; // SPSC ring; one slot is the full/empty marker

std::atomic<DisplayMirror::FrameObserver> frameObserver{nullptr};
std::atomic<bool> fullRefreshRequested{false};
std::atomic<bool> wakeRequested{false};

DisplayDriver *displaydriver = nullptr;
bool byteSwapped = false;
lv_indev_t *pointer = nullptr;
lv_indev_t *keypad = nullptr;
lv_indev_t *encoder = nullptr;

Touch touchQueue[queueLen];
std::atomic<uint8_t> touchHead{0}, touchTail{0};
uint32_t keyQueue[queueLen];
std::atomic<uint8_t> keyHead{0}, keyTail{0};
int8_t encoderQueue[queueLen];
std::atomic<uint8_t> encoderHead{0}, encoderTail{0};

// LVGL thread. Remote input must wake a slept panel and still act, or every event is spent as a wake.
void service_requests(void)
{
    if (wakeRequested.exchange(false, std::memory_order_acquire) && displaydriver) {
        if (displaydriver->isPowersaving())
            displaydriver->forceWakeup();
        if (lv_display_t *display = displaydriver->getDisplay())
            lv_display_trigger_activity(display);
    }
    if (fullRefreshRequested.exchange(false, std::memory_order_acquire)) {
        lv_obj_invalidate(lv_screen_active());
        lv_obj_invalidate(lv_layer_top());
        lv_obj_invalidate(lv_layer_sys());
    }
}

// A queued touch is held PRESSED for holdMs (at least one read), then RELEASED once.
void pointer_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    static bool pressing = false;
    static bool needRelease = false;
    static uint32_t pressStart = 0;
    static lv_point_t last = {0, 0};

    service_requests();

    if (pressing) {
        const Touch &t = touchQueue[touchHead.load(std::memory_order_relaxed)];
        if (lv_tick_elaps(pressStart) >= t.holdMs) {
            pressing = false;
            needRelease = true;
            touchHead.store((touchHead.load(std::memory_order_relaxed) + 1) % queueLen, std::memory_order_release);
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
        const Touch &t = touchQueue[touchHead.load(std::memory_order_relaxed)];
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

// One key per read, released on the next so the widget sees a complete press.
void keypad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    static bool needRelease = false;
    static uint32_t lastKey = 0;

    service_requests();

    if (needRelease) {
        needRelease = false;
        data->key = lastKey;
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }
    if (keyHead.load(std::memory_order_relaxed) != keyTail.load(std::memory_order_acquire)) {
        lastKey = keyQueue[keyHead.load(std::memory_order_relaxed)];
        keyHead.store((keyHead.load(std::memory_order_relaxed) + 1) % queueLen, std::memory_order_release);
        needRelease = true;
        data->key = lastKey;
        data->state = LV_INDEV_STATE_PRESSED;
        return;
    }
    data->key = lastKey;
    data->state = LV_INDEV_STATE_RELEASED;
}

void encoder_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    service_requests();

    data->state = LV_INDEV_STATE_RELEASED;
    data->enc_diff = 0;
    uint8_t head = encoderHead.load(std::memory_order_relaxed);
    if (head != encoderTail.load(std::memory_order_acquire)) {
        data->enc_diff = encoderQueue[head];
        encoderHead.store((head + 1) % queueLen, std::memory_order_release);
    }
}

// The physical touch's long-press time, so a remote hold behaves like a finger.
uint16_t touch_long_press_time(void)
{
    for (lv_indev_t *indev = lv_indev_get_next(nullptr); indev; indev = lv_indev_get_next(indev)) {
        if (indev != pointer && lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER)
            return indev->long_press_time;
    }
    return 0;
}
} // namespace

void DisplayMirror::start(DisplayDriver *driver)
{
    if (displaydriver || !driver)
        return;
    displaydriver = driver;
    lv_display_t *display = driver->getDisplay();
    byteSwapped = display && lv_display_get_color_format(display) == LV_COLOR_FORMAT_RGB565_SWAPPED;

    driver->setFlushCB([](int16_t x, int16_t y, uint16_t width, uint16_t height, const uint16_t *pixels, uint16_t stride) {
        if (auto observer = frameObserver.load(std::memory_order_acquire))
            observer(x, y, width, height, pixels, stride);
    });

    // every input driver makes its group the default in init(); a board with none gets one here
    lv_group_t *group = lv_group_get_default();
    if (!group) {
        group = lv_group_create();
        lv_group_set_default(group);
    }

    keypad = lv_indev_create();
    lv_indev_set_type(keypad, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(keypad, keypad_read);
    lv_indev_set_group(keypad, group);

    encoder = lv_indev_create();
    lv_indev_set_type(encoder, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder, encoder_read);
    lv_indev_set_group(encoder, group);

    const uint16_t longPressTime = touch_long_press_time();
    pointer = lv_indev_create();
    lv_indev_set_type(pointer, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(pointer, pointer_read);
    if (longPressTime)
        lv_indev_set_long_press_time(pointer, longPressTime);
    ILOG_DEBUG("DisplayMirror: virtual input devices ready");
}

void DisplayMirror::setFrameObserver(FrameObserver observer)
{
    frameObserver.store(observer, std::memory_order_release);
}

bool DisplayMirror::pixelsByteSwapped(void)
{
    return byteSwapped;
}

void DisplayMirror::requestFullRefresh(void)
{
    fullRefreshRequested.store(true, std::memory_order_release);
}

void DisplayMirror::injectTouch(int16_t x, int16_t y, uint16_t holdMs)
{
    uint8_t tail = touchTail.load(std::memory_order_relaxed);
    uint8_t next = (tail + 1) % queueLen;
    if (next == touchHead.load(std::memory_order_acquire))
        return;
    touchQueue[tail] = {x, y, holdMs};
    wakeRequested.store(true, std::memory_order_release);
    touchTail.store(next, std::memory_order_release);
}

void DisplayMirror::injectKey(uint32_t key)
{
    uint8_t tail = keyTail.load(std::memory_order_relaxed);
    uint8_t next = (tail + 1) % queueLen;
    if (next == keyHead.load(std::memory_order_acquire))
        return;
    keyQueue[tail] = key;
    wakeRequested.store(true, std::memory_order_release);
    keyTail.store(next, std::memory_order_release);
}

void DisplayMirror::injectEncoder(int16_t steps)
{
    uint8_t tail = encoderTail.load(std::memory_order_relaxed);
    uint8_t next = (tail + 1) % queueLen;
    if (next == encoderHead.load(std::memory_order_acquire))
        return;
    encoderQueue[tail] = (int8_t)(steps > 127 ? 127 : (steps < -127 ? -127 : steps));
    wakeRequested.store(true, std::memory_order_release);
    encoderTail.store(next, std::memory_order_release);
}
