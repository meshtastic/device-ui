#include "graphics/DisplayMirror.h"
#include "graphics/driver/DisplayDriver.h"
#include "util/ILog.h"

std::atomic<DisplayMirror::FrameObserver> DisplayMirror::frameObserver{nullptr};
std::atomic<bool> DisplayMirror::fullRefreshRequested{false};
std::atomic<bool> DisplayMirror::wakeRequested{false};

DisplayDriver *DisplayMirror::displaydriver = nullptr;
lv_indev_t *DisplayMirror::pointer = nullptr;
lv_indev_t *DisplayMirror::keypad = nullptr;
lv_indev_t *DisplayMirror::encoder = nullptr;

DisplayMirror::Touch DisplayMirror::touchQueue[DisplayMirror::queueLen];
std::atomic<uint8_t> DisplayMirror::touchHead{0}, DisplayMirror::touchTail{0};
uint32_t DisplayMirror::keyQueue[DisplayMirror::queueLen];
std::atomic<uint8_t> DisplayMirror::keyHead{0}, DisplayMirror::keyTail{0};
int8_t DisplayMirror::encoderQueue[DisplayMirror::queueLen];
std::atomic<uint8_t> DisplayMirror::encoderHead{0}, DisplayMirror::encoderTail{0};

void DisplayMirror::start(DisplayDriver *driver)
{
    displaydriver = driver;

    // Every physical input driver creates this in its own init() and makes it
    // the default, and those run before a host can call start(); so only a
    // board with no input at all gets here with none.
    lv_group_t *group = lv_group_get_default();
    if (!group) {
        group = lv_group_create();
        lv_group_set_default(group);
    }

    if (!keypad) {
        keypad = lv_indev_create();
        lv_indev_set_type(keypad, LV_INDEV_TYPE_KEYPAD);
        lv_indev_set_read_cb(keypad, keypadRead);
        lv_indev_set_group(keypad, group);
    }
    if (!encoder) {
        encoder = lv_indev_create();
        lv_indev_set_type(encoder, LV_INDEV_TYPE_ENCODER);
        lv_indev_set_read_cb(encoder, encoderRead);
        lv_indev_set_group(encoder, group);
    }
    if (!pointer) {
        pointer = lv_indev_create();
        lv_indev_set_type(pointer, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(pointer, pointerRead);
    }
    ILOG_DEBUG("DisplayMirror: virtual input devices ready");
}

void DisplayMirror::stop(void)
{
    frameObserver.store(nullptr, std::memory_order_release);
}

void DisplayMirror::setFrameObserver(FrameObserver observer)
{
    frameObserver.store(observer, std::memory_order_release);
}

void DisplayMirror::requestFullRefresh(void)
{
    fullRefreshRequested.store(true, std::memory_order_release);
}

void DisplayMirror::serviceRequests(void)
{
    // Remote input has to wake a slept panel and still act: without this the
    // first event is swallowed as a wake, which is every event when nobody is
    // physically at the device.
    if (wakeRequested.exchange(false, std::memory_order_acquire) && displaydriver) {
        if (displaydriver->isPowersaving())
            displaydriver->forceWakeup();
        if (lv_display_t *display = displaydriver->getDisplay())
            lv_display_trigger_activity(display);
    }
    if (fullRefreshRequested.exchange(false, std::memory_order_acquire)) {
        lv_obj_invalidate(lv_scr_act());
        // Overlay content - clock, notifications - lives on the top and system
        // layers, so a full sync has to repaint those too.
        lv_obj_invalidate(lv_layer_top());
        lv_obj_invalidate(lv_layer_sys());
    }
}

void DisplayMirror::injectTouch(int16_t x, int16_t y, uint16_t holdMs)
{
    uint8_t tail = touchTail.load(std::memory_order_relaxed);
    uint8_t next = (tail + 1) % queueLen;
    if (next == touchHead.load(std::memory_order_acquire))
        return; // full; drop
    touchQueue[tail] = {x, y, holdMs};
    wakeRequested.store(true, std::memory_order_release);
    touchTail.store(next, std::memory_order_release);
}

void DisplayMirror::injectKey(uint32_t key)
{
    uint8_t tail = keyTail.load(std::memory_order_relaxed);
    uint8_t next = (tail + 1) % queueLen;
    if (next == keyHead.load(std::memory_order_acquire))
        return; // full; drop
    keyQueue[tail] = key;
    wakeRequested.store(true, std::memory_order_release);
    keyTail.store(next, std::memory_order_release);
}

void DisplayMirror::injectEncoder(int16_t steps)
{
    uint8_t tail = encoderTail.load(std::memory_order_relaxed);
    uint8_t next = (tail + 1) % queueLen;
    if (next == encoderHead.load(std::memory_order_acquire))
        return; // full; drop
    // The ring stores a byte; clamp rather than let 256 truncate to a no-op.
    encoderQueue[tail] = (int8_t)(steps > 127 ? 127 : (steps < -127 ? -127 : steps));
    wakeRequested.store(true, std::memory_order_release);
    encoderTail.store(next, std::memory_order_release);
}

// LVGL thread. A queued touch is held PRESSED until holdMs elapses (at least
// one read cycle), then RELEASED once before the next entry starts.
void DisplayMirror::pointerRead(lv_indev_t *indev, lv_indev_data_t *data)
{
    static bool pressing = false;
    static bool needRelease = false;
    static uint32_t pressStart = 0;
    static lv_point_t last = {0, 0};

    serviceRequests();

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

// LVGL thread. One key per read, released on the following read so the widget
// sees a complete press.
void DisplayMirror::keypadRead(lv_indev_t *indev, lv_indev_data_t *data)
{
    static bool needRelease = false;
    static uint32_t lastKey = 0;

    serviceRequests();

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

// LVGL thread. One queued rotation per read; enc_diff moves the group focus.
void DisplayMirror::encoderRead(lv_indev_t *indev, lv_indev_data_t *data)
{
    serviceRequests();

    data->state = LV_INDEV_STATE_RELEASED;
    data->enc_diff = 0;
    uint8_t head = encoderHead.load(std::memory_order_relaxed);
    if (head != encoderTail.load(std::memory_order_acquire)) {
        data->enc_diff = encoderQueue[head];
        encoderHead.store((head + 1) % queueLen, std::memory_order_release);
    }
}
