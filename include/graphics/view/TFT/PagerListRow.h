#pragma once

#include "styles.h"
#include "ui.h"

inline void pagerListRowFocus(lv_event_t *event)
{
    auto *row = static_cast<lv_obj_t *>(lv_event_get_user_data(event));
    lv_obj_set_state(row, LV_STATE_FOCUS_KEY, lv_event_get_code(event) == LV_EVENT_FOCUSED);
}

inline void stylePagerListRow(lv_obj_t *row, lv_obj_t *focusTarget)
{
    add_style_node_panel_style(row);
    lv_obj_set_style_min_height(row, 53, 0);
    const int32_t width = lv_display_dpx(lv_obj_get_display(row), 3);
    lv_obj_set_style_outline_width(row, width, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_pad(row, -width, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(row, lv_palette_main(LV_PALETTE_BLUE), LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_opa(row, LV_OPA_COVER, LV_STATE_FOCUS_KEY);

    // Nodes retains its existing input target and indexed status fields.
    // The shared row, rather than the invisible overlay, owns its selection.
    if (row != focusTarget) {
        lv_obj_set_style_outline_opa(focusTarget, LV_OPA_TRANSP, LV_STATE_FOCUS_KEY);
        lv_obj_set_style_outline_opa(focusTarget, LV_OPA_TRANSP, LV_STATE_EDITED);
        lv_obj_add_event_cb(focusTarget, pagerListRowFocus, LV_EVENT_FOCUSED, row);
        lv_obj_add_event_cb(focusTarget, pagerListRowFocus, LV_EVENT_DEFOCUSED, row);
    }
}
