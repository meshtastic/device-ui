#pragma once

#include "lvgl.h"
#include <cstdint>

class LVGLGraphics
{
  public:
    LVGLGraphics(uint16_t width, uint16_t height);
    void init(void);

    lv_disp_draw_buf_t *get_draw_buf(void) { return draw_buf; }

  private:
    static void lv_debug(const char *buf);

    uint16_t screenWidth;
    uint16_t screenHeight;
    size_t bufsize;

    lv_disp_draw_buf_t *draw_buf = nullptr;
    lv_color_t *buf1 = nullptr;
    lv_color_t *buf2 = nullptr;
};