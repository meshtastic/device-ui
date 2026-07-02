#pragma once

#include "lvgl.h"
#include <string>

// #define LVGL_DEBUG_FOCUS

class TFTView_320x240;

class TFTView_Debug
{
#if defined(LVGL_DEBUG_FOCUS)
  public:
    static void ui_group_focus_debug_cb(lv_group_t *group);

  private:
    static const char *lookupObjectName320x240(lv_obj_t *obj);
    static bool isDescendantOf(lv_obj_t *obj, lv_obj_t *ancestor);
    static uint32_t getObjFlagsMask320x240(const lv_obj_t *obj);
    static std::string flagsMaskToCompactString320x240(uint32_t flags);
    static void logObjectDetails320x240(const char *tag, lv_obj_t *obj);
    static void logObjectChain320x240(lv_obj_t *obj);
    static void printObjectName(TFTView_320x240 *view, lv_obj_t *obj);
#endif
};