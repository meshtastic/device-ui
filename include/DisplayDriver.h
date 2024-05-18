#pragma once

#include "DeviceGUI.h"
#include "LVGLGraphics.h"
#include <cstdint>

#define H_NORM_PX(h_scr_percent) ((int16_t)((screenWidth / 100.0) * (h_scr_percent)))
#define V_NORM_PX(v_scr_percent) ((int16_t)((screenHeight / 100.0) * (v_scr_percent)))

typedef lv_display_t LVGLDisplay;

class DisplayDriver
{
  public:
    DisplayDriver(uint16_t width, uint16_t height);
    virtual void init(DeviceGUI *gui);
    virtual bool hasTouch(void) { return false; }
    virtual bool hasLight(void) { return false; }
    virtual void task_handler(void) { lv_timer_handler(); };
    virtual bool isPowersaving() { return false; }
    virtual ~DisplayDriver() {}

    virtual uint8_t getBrightness() { return 255; }
    virtual void setBrightness(uint8_t timeout) {}
    virtual uint8_t getScreenTimeout() { return 0; }
    virtual void setScreenTimeout(uint8_t timeout) {}

    uint16_t getScreenWidth(void) { return screenWidth; }
    uint16_t getScreenHeight(void) { return screenHeight; }

  protected:
    LVGLGraphics lvgl;
    LVGLDisplay *display;
    DeviceGUI *view;
    uint16_t screenWidth;
    uint16_t screenHeight;
};
