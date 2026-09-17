#pragma once

#include "ui.h"

inline void stylePagerDialogButtonFocus(lv_obj_t *button)
{
    lv_obj_set_style_bg_color(button, lv_color_hex(0x67ea94), LV_STATE_FOCUS_KEY);
    lv_obj_set_style_text_color(button, lv_color_hex(0x102019), LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(button, lv_color_hex(0x102019), LV_STATE_FOCUS_KEY);
}

inline void applyPagerDialogFocus()
{
    lv_obj_t *panels[] = {objects.obj2__ok_cancel_panel_w,  objects.obj3__ok_cancel_panel_w,  objects.obj4__ok_cancel_panel_w,
                          objects.obj5__ok_cancel_panel_w,  objects.obj6__ok_cancel_panel_w,  objects.obj7__ok_cancel_panel_w,
                          objects.obj8__ok_cancel_panel_w,  objects.obj9__ok_cancel_panel_w,  objects.obj10__ok_cancel_panel_w,
                          objects.obj11__ok_cancel_panel_w, objects.obj12__ok_cancel_panel_w, objects.obj13__ok_cancel_panel_w,
                          objects.obj14__ok_cancel_panel_w, objects.obj15__ok_cancel_panel_w, objects.obj16__ok_cancel_panel_w,
                          objects.obj17__ok_cancel_panel_w, objects.obj18__ok_cancel_panel_w, objects.obj21__ok_cancel_panel_w,
                          objects.obj27__ok_cancel_panel_w};
    for (auto panel : panels) {
        for (uint32_t i = 0; i < lv_obj_get_child_count(panel); ++i) {
            lv_obj_t *button = lv_obj_get_child(panel, i);
            if (!lv_obj_check_type(button, &lv_button_class))
                continue;
            stylePagerDialogButtonFocus(button);
        }
    }
}
