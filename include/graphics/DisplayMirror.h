#pragma once

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
 * Nothing in the driver framework knows this class exists. start() registers a
 * callback with DisplayDriver and creates its own virtual input devices, so the
 * dependency runs one way only. LVGL stays an implementation detail: none of it
 * appears here, so a host can include this header without taking on the
 * drawing framework.
 *
 * Threading: start(), stop() and the inject*() calls come from the host's
 * thread; the flush callback and the LVGL read callbacks run on the LVGL
 * thread. Events cross between them in lock-free single-producer rings, so no
 * lock is taken on the render path.
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
     * Register the flush callback and the virtual input devices.
     *
     * Call once after DeviceScreen::init(), which brings LVGL up, and before
     * the view builds its widgets: the virtual keypad and encoder join the
     * default focus group, and LVGL enrols widgets in that group as it creates
     * them. Any physical input driver has already made the group by this point;
     * on a board with no input at all, this makes it.
     *
     * Must also run before the host starts its UI task. This touches LVGL
     * directly rather than hopping threads, so it is only safe while nothing
     * is calling lv_timer_periodic_handler() yet.
     *
     * @param driver display driver, used to wake a slept panel - see
     *               DeviceScreen::getDisplayDriver().
     */
    static void start(DisplayDriver *driver);

    /**
     * Stop capturing frames. The flush callback stays registered and the
     * virtual input devices stay in the group: both are only safe to install
     * while the UI is not yet running, and capture is gated on the observer
     * instead, which any thread may clear at any time.
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
};
