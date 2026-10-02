#pragma once

#include "graphics/DeviceGUI.h"
#include "graphics/LVGL/LVGLGraphics.h"
#include <cstdint>
#include <functional>

#define H_NORM_PX(h_scr_percent) ((int16_t)((screenWidth / 100.0) * (h_scr_percent)))
#define V_NORM_PX(v_scr_percent) ((int16_t)((screenHeight / 100.0) * (v_scr_percent)))

// finger distance ratio per map zoom level; also the pinch recognition threshold
#ifndef PINCH_ZOOM_STEP
#define PINCH_ZOOM_STEP 1.4f
#endif

typedef lv_display_t LVGLDisplay;
typedef lv_indev_t LVGLTouch;

class DisplayDriver
{
  public:
    DisplayDriver(uint16_t width, uint16_t height);
    virtual void init(DeviceGUI *gui);
    virtual bool calibrate(uint16_t parameters[8]) { return false; }
    virtual bool hasTouch(void) { return false; }
    virtual bool hasButton(void) { return false; }
    virtual bool hasLight(void) { return false; }
    virtual void task_handler(void) { lv_timer_periodic_handler(); }
    virtual void forceWakeup(void) {}
    virtual bool isPowersaving() { return false; }
    virtual void toggleDisplay(void);
    virtual void printConfig(void) {}
    virtual ~DisplayDriver() {}

    virtual uint8_t getBrightness() { return 255; }
    virtual void setBrightness(uint8_t timeout) {}

    virtual uint16_t getScreenTimeout() { return 0; }
    virtual void setScreenTimeout(uint16_t timeout) {}

    uint16_t getScreenWidth(void) { return screenWidth; }
    uint16_t getScreenHeight(void) { return screenHeight; }

    lv_display_t *getDisplay(void) { return display; }

    /**
     * Dirty-rect sink. (x, y, width, height) is the area that changed; pixels points
     * at its top-left pixel in the display's color format, rows stride pixels apart.
     * Runs on the LVGL thread, so a callback must copy what it needs and return.
     */
    using FlushCallback = std::function<void(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint16_t *pixels,
                                             uint16_t stride)>;

    /**
     * Observe every flush. Static because the LVGL flush callbacks the subclasses
     * register are plain C function pointers with no instance to hand back.
     * Set once before the UI task starts; there is one display driver per build.
     */
    static void setFlushCB(FlushCallback cb);

  protected:
    /** Subclasses call this from their flush callback with the area LVGL rendered. */
    static void flush(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint16_t *pixels, uint16_t stride);

    LVGLGraphics lvgl;
    LVGLDisplay *display;
    LVGLTouch *touch;
    DeviceGUI *view;
    uint16_t screenWidth;
    uint16_t screenHeight;

  private:
    static void displayToggleCb(void *displayDriver);
    static FlushCallback flushCB;
};
