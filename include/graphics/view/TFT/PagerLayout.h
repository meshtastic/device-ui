#pragma once

#include "ui.h"

inline void insetPagerFocusOutline(lv_group_t *group)
{
    lv_obj_t *focused = lv_group_get_focused(group);
    if (!focused)
        return;

    // LVGL scrolls the widget into view without its external focus outline.
    // Draw the ring inside its bounds so screen/container edges cannot clip it.
    const int32_t width = lv_display_dpx(lv_obj_get_display(focused), 3);
    const lv_state_t states[] = {LV_STATE_FOCUS_KEY, LV_STATE_EDITED};
    for (auto state : states) {
        lv_obj_set_style_outline_width(focused, width, LV_PART_MAIN | state);
        lv_obj_set_style_outline_pad(focused, -width, LV_PART_MAIN | state);
    }
}

// The shared landscape UI scales its panels, but these fixed vertical sizes
// need less space on the Pager's 222-pixel-high display.
inline void applyPagerLayout(uint32_t height)
{
    if (height >= 240)
        return;

    // Apply on focus changes so dynamically created rows receive the same fix.
    lv_group_t *group = lv_group_get_default();
    if (group) {
        lv_group_set_focus_cb(group, insetPagerFocusOutline);
        insetPagerFocusOutline(group);
    }

    // Keep all six 36-pixel navigation buttons visible without shrinking them.
    lv_obj_set_style_pad_row(objects.button_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Fit first-run setup and its bottom-aligned OK/Cancel controls in the panel.
    lv_obj_set_y(objects.setup_panel, 0);
    lv_obj_set_height(objects.setup_panel, LV_PCT(100));

    // The scanner is 210 pixels tall; its normal 12-pixel offset clips the bottom.
    lv_obj_set_y(objects.signal_scanner_panel, 0);
}
