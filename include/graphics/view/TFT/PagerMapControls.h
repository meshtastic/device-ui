#pragma once

#include "graphics/map/TileProvider.h"
#include "ui.h"

inline void pagerMapControlKey(lv_event_t *event)
{
    if (lv_event_get_key(event) != LV_KEY_ESC)
        return;

    auto *group = lv_obj_get_group(lv_event_get_current_target_obj(event));
    if (group)
        lv_group_set_editing(group, false);
    lv_group_focus_obj(objects.map_button);
    lv_event_stop_processing(event);
}

inline void applyPagerMapControls()
{
    auto *group = lv_group_get_default();
    lv_obj_t *controls[] = {
        objects.arrow_up_button,    objects.arrow_left_button, objects.nav_button,
        objects.arrow_right_button, objects.arrow_down_button, objects.gps_lock_button,
        objects.zoom_in_button,     objects.zoom_out_button,   objects.zoom_slider,
    };
    for (auto *control : controls) {
        // Image buttons do not join LVGL's default group automatically.
        if (group && lv_obj_get_group(control) != group)
            lv_group_add_obj(group, control);
        lv_obj_add_event_cb(control, pagerMapControlKey, LV_EVENT_KEY, nullptr);
    }

    lv_slider_set_range(objects.zoom_slider, 2, TileProvider::maxZoom());
    lv_obj_set_size(objects.zoom_slider, 160, 8);
    // Leave room for the bottom labels/logo and the pan controls on the right.
    lv_obj_align(objects.zoom_slider, LV_ALIGN_BOTTOM_LEFT, 14, -38);
    lv_obj_remove_flag(objects.zoom_slider, LV_OBJ_FLAG_HIDDEN);
}

inline void syncPagerMapZoom(uint8_t zoom)
{
    lv_slider_set_range(objects.zoom_slider, 2, TileProvider::maxZoom());
    lv_slider_set_value(objects.zoom_slider, zoom, LV_ANIM_OFF);
}

inline void focusPagerMapZoom(uint8_t zoom)
{
    syncPagerMapZoom(zoom);
    lv_obj_remove_flag(objects.zoom_slider, LV_OBJ_FLAG_HIDDEN);
    lv_group_focus_obj(objects.zoom_slider);
    auto *group = lv_obj_get_group(objects.zoom_slider);
    if (group)
        lv_group_set_editing(group, true);
    // LVGL's slider leaves editing on a short encoder press/release.
}

inline void deferPagerMapZoom(uint8_t zoom)
{
    syncPagerMapZoom(zoom);
    // Dropdown release handling clears editing after VALUE_CHANGED returns.
    lv_async_call(
        [](void *) {
            if (lv_obj_is_visible(objects.map_panel) && lv_obj_has_flag(objects.map_osd_panel, LV_OBJ_FLAG_HIDDEN))
                focusPagerMapZoom(static_cast<uint8_t>(lv_slider_get_value(objects.zoom_slider)));
        },
        nullptr);
}

inline void pagerMapSettingsKey(lv_event_t *event)
{
    if (lv_event_get_key(event) != LV_KEY_ESC)
        return;
    lv_obj_add_flag(objects.map_osd_panel, LV_OBJ_FLAG_HIDDEN);
    focusPagerMapZoom(static_cast<uint8_t>(lv_slider_get_value(objects.zoom_slider)));
    lv_event_stop_processing(event);
}
