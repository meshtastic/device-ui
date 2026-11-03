#pragma once

#include <cstdint>

class DisplayDriver;

/**
 * @brief Streams the rendered screen to a host and feeds the host's input back
 *        into LVGL. Idle until start(); no driver knows this class exists.
 */
class DisplayMirror
{
  public:
    // LVGL thread: pixels is the area's top-left, rows stride pixels apart, in the
    // byte order pixelsByteSwapped() reports. Copy, never write, and return.
    using FrameObserver = void (*)(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint16_t *pixels,
                                   uint16_t stride);

    // once, after DeviceScreen::init() and before the UI task starts
    static void start(DisplayDriver *driver);

    // nullptr stops capture; a flush may still be in flight for one UI tick
    static void setFrameObserver(FrameObserver observer);

    static bool pixelsByteSwapped(void);

    // any thread; repaints the screen and its overlay layers
    static void requestFullRefresh(void);

    // remote input, from a single producer thread; dropped when the queue is full
    static void injectTouch(int16_t x, int16_t y);
    // held past this device's long-press threshold, so it behaves like a finger
    static void injectLongPress(int16_t x, int16_t y);
    static void injectKey(uint32_t key); // LV_KEY_* or a printable character
    static void injectLongPressKey(uint32_t key);
    // moves group focus; negative is backwards, clamped to a byte
    static void injectEncoder(int16_t steps);
};
