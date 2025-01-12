#pragma once

#include "lvgl.h"
#include <cstdint>

class LVGLGraphics
{
  public:
    LVGLGraphics(uint16_t width, uint16_t height);
    void init(void);

  private:
    static void lv_debug(lv_log_level_t level, const char *buf);

    uint16_t screenWidth;
    uint16_t screenHeight;
};