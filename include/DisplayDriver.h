#pragma once

#include "LVGLGraphics.h"
#include <cstdint>

#define H_NORM_PX(h_scr_percent)                                               \
  ((int16_t)((screenWidth / 100.0) * (h_scr_percent)))
#define V_NORM_PX(v_scr_percent)                                               \
  ((int16_t)((screenHeight / 100.0) * (v_scr_percent)))

class DisplayDriver {
public:
  DisplayDriver(uint16_t width, uint16_t height);
  virtual void init(void);
  virtual bool hasTouch(void) { return false; }
  virtual void task_handler(void) { lv_timer_handler(); };
  virtual ~DisplayDriver(){};

  uint16_t getScreenWidth(void) { return screenWidth; }
  uint16_t getScreenHeight(void) { return screenHeight; }

protected:
  LVGLGraphics lvgl;
  uint16_t screenWidth;
  uint16_t screenHeight;

private:
};
