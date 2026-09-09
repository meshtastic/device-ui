#pragma once

#include "lvgl.h"
#include <atomic>
#include <cstdint>

class DisplayDriver;

/**
 * @brief DisplayMirror - streams the rendered screen out to a host and feeds
 *        the host's input back into LVGL.
 *
 * Lets a host (e.g. firmware bridging a phone or desktop client) show the
 * on-device UI elsewhere and drive it remotely. Idle unless a host calls
 * start(): nothing is allocated, no input device is registered, and a flush
 * costs one acquire load.
 *
 * Threading: start(), stop() and the inject*() calls come from the host's
 * thread; onFlush() and the LVGL read callbacks run on the LVGL thread. Events
 * cross between them in lock-free single-producer rings, so no lock is taken on
 * the render path.
 */
class DisplayMirror
{
  public:
    /**
     * Dirty-rect sink. (x, y, width, height) is the area that changed; pixels
     * are native little-endian RGB565 with rows tightly packed - true for
     * LV_DISPLAY_RENDER_MODE_PARTIAL, which is what every driver here uses.
     * Runs on the LVGL thread ahead of the panel byte-swap, so an observer must
     * copy what it needs and return.
     */
    using FrameObserver = void (*)(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint16_t *pixels);

    /**
     * Register the virtual input devices.
     *
     * Call once after DeviceScreen::init(), which brings LVGL up, and before
     * the view builds its widgets: the virtual keypad and encoder join the
     * default focus group, and LVGL enrols widgets in that group as it creates
     * them. Any physical input driver has already made the group by this point;
     * on a board with no input at all, this makes it.
     *
     * @param driver display driver, used to wake a slept panel - see
     *               DeviceScreen::getDisplayDriver().
     */
    static void start(DisplayDriver *driver);

    /**
     * Stop capturing frames. The virtual input devices stay registered, since
     * LVGL has no way to remove one from a group's focus history safely while
     * the UI is live.
     */
    static void stop(void);

    /**
     * Frames are captured only while an observer is set. Clearing it stops
     * capture; wait at least one UI tick before freeing observer state, since
     * a flush may already be in flight.
     */
    static void setFrameObserver(FrameObserver observer);

    /**
     * Repaint the whole screen, so a newly attached observer gets a complete
     * frame rather than whatever happens to change next. Any thread; serviced
     * on the LVGL thread.
     */
    static void requestFullRefresh(void);

    // -- Remote input ---------------------------------------------------------
    // Single-producer: drive these from one thread. Events queue into rings the
    // LVGL read callbacks drain (15 usable slots each) and are dropped when
    // full, on the grounds that a client outrunning the UI wants the newest
    // state, not a backlog.

    /**
     * A tap at (x, y), held PRESSED for holdMs - 0 being a single read cycle,
     * around three refresh periods end to end. Pass ~600 for a long press.
     */
    static void injectTouch(int16_t x, int16_t y, uint16_t holdMs = 0);

    /** LV_KEY_* or a printable character, delivered to the focused widget. */
    static void injectKey(uint32_t key);

    /**
     * Encoder rotation. This, not LV_KEY_UP/DOWN, is what moves focus between
     * widgets in a group - LVGL routes keypad arrows to the focused widget
     * instead. Negative steps focus backwards, positive forwards, matching the
     * trackball driver. Clamped to a single byte.
     */
    static void injectEncoder(int16_t steps);

    /** Called by the display driver on every flush; a no-op while idle. */
    static void onFlush(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint16_t *pixels)
    {
        if (auto observer = frameObserver.load(std::memory_order_acquire))
            observer(x, y, width, height, pixels);
    }

  private:
    struct Touch {
        int16_t x, y;
        uint16_t holdMs;
    };
    static constexpr uint8_t queueLen = 16; // SPSC ring; one slot is the full/empty marker

    static void pointerRead(lv_indev_t *indev, lv_indev_data_t *data);
    static void keypadRead(lv_indev_t *indev, lv_indev_data_t *data);
    static void encoderRead(lv_indev_t *indev, lv_indev_data_t *data);

    // LVGL thread, from every read callback: drains the wake and repaint
    // requests. Riding the read callbacks rather than a timer means the mirror
    // adds no periodic work of its own.
    static void serviceRequests(void);

    static std::atomic<FrameObserver> frameObserver;
    static std::atomic<bool> fullRefreshRequested;
    static std::atomic<bool> wakeRequested;

    static DisplayDriver *displaydriver;
    static lv_indev_t *pointer;
    static lv_indev_t *keypad;
    static lv_indev_t *encoder;

    static Touch touchQueue[queueLen];
    static std::atomic<uint8_t> touchHead, touchTail;
    static uint32_t keyQueue[queueLen];
    static std::atomic<uint8_t> keyHead, keyTail;
    static int8_t encoderQueue[queueLen];
    static std::atomic<uint8_t> encoderHead, encoderTail;
};
