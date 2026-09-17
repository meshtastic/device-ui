#pragma once

#include "ui.h"

inline void stylePagerDialogButtonFocus(lv_obj_t *button)
{
    lv_obj_set_style_bg_color(button, lv_color_hex(0x67ea94), LV_STATE_FOCUS_KEY);
    lv_obj_set_style_text_color(button, lv_color_hex(0x102019), LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(button, lv_color_hex(0x102019), LV_STATE_FOCUS_KEY);
}
