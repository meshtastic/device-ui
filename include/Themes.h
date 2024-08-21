#pragma once

#include "lvgl.h";
class Themes
{
  public:
    enum Theme {
        eLight,
        eDark
    };

    static void initStyles(void);
    static enum Theme get(void);
    static void set(enum Theme th);
    static void recolorButton(lv_obj_t* obj, bool enabled, lv_opa_t opa = 255);
    static void recolorText(lv_obj_t* obj, bool enabled);
    static void recolorTopLabel(lv_obj_t* obj, bool alert);

  private:
    Themes(void) = default;
};