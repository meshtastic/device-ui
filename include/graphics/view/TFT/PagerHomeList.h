#pragma once

#include "styles.h"
#include "ui.h"

inline void drawPagerHomeIcon(lv_event_t *event)
{
    lv_obj_t *icon = lv_event_get_target_obj(event);
    lv_obj_t *row = lv_obj_get_parent(icon);
    const void *source = lv_obj_get_style_bg_image_src(row, LV_PART_MAIN);
    lv_image_header_t header;
    if (!source || lv_image_decoder_get_info(source, &header) != LV_RESULT_OK)
        return;

    lv_area_t area;
    lv_obj_get_coords(icon, &area);
    area.x1 += (lv_area_get_width(&area) - static_cast<int32_t>(header.w)) / 2;
    area.y1 += (lv_area_get_height(&area) - static_cast<int32_t>(header.h)) / 2;
    area.x2 = area.x1 + header.w - 1;
    area.y2 = area.y1 + header.h - 1;

    // Read the existing button styles at draw time so live status icons,
    // pressed colors and theme changes continue to use their original updates.
    lv_draw_image_dsc_t draw;
    lv_draw_image_dsc_init(&draw);
    draw.base.layer = lv_event_get_layer(event);
    lv_obj_init_draw_image_dsc(icon, LV_PART_MAIN, &draw);
    draw.src = source;
    draw.recolor = lv_obj_get_style_bg_image_recolor(row, LV_PART_MAIN);
    draw.recolor_opa = lv_obj_get_style_bg_image_recolor_opa(row, LV_PART_MAIN);
    draw.image_area = area;
    lv_draw_image(draw.base.layer, &draw, &area);
}

inline void applyPagerHomeList()
{
    lv_obj_set_style_pad_left(objects.home_panel, 8, 0);
    lv_obj_set_style_pad_right(objects.home_panel, 12, 0);
    lv_obj_set_style_pad_top(objects.home_panel, 5, 0);
    lv_obj_set_style_pad_bottom(objects.home_panel, 5, 0);
    lv_obj_set_width(objects.home_container, LV_PCT(100));
    lv_obj_set_style_border_width(objects.home_container, 0, 0);
    lv_obj_set_style_shadow_width(objects.home_container, 0, 0);
    lv_obj_set_style_radius(objects.home_container, 0, 0);
    lv_obj_set_style_bg_opa(objects.home_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(objects.home_container, 0, 0);
    lv_obj_set_style_pad_row(objects.home_container, 5, 0);
    lv_obj_set_flex_flow(objects.home_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_flow(objects.home_container, LV_FLEX_FLOW_COLUMN, LV_STATE_FOCUSED);
    lv_obj_set_flex_align(objects.home_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_scroll_dir(objects.home_container, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(objects.home_container, LV_SCROLLBAR_MODE_AUTO);

    struct HomeRow {
        lv_obj_t *button;
        lv_obj_t *label;
    };
    const HomeRow rows[] = {
        {objects.home_mail_button, objects.home_mail_label},
        {objects.home_nodes_button, objects.home_nodes_label},
        {objects.home_time_button, objects.home_time_label},
        {objects.home_lora_button, objects.home_lora_label},
        {objects.home_signal_button, objects.home_signal_label},
        {objects.home_bell_button, objects.home_bell_label},
        {objects.home_location_button, objects.home_location_label},
        {objects.home_wlan_button, objects.home_wlan_label},
        {objects.home_ethernet_button, objects.home_ethernet_label},
        {objects.home_bluetooth_button, objects.home_bluetooth_label},
        {objects.home_mqtt_button, objects.home_mqtt_label},
        {objects.home_sd_card_button, objects.home_sd_card_label},
        {objects.home_memory_button, objects.home_memory_label},
        {objects.home_qr_button, objects.home_qr_label},
    };
    for (const auto &row : rows) {
        // Keep the existing button as the row's focus and action target.
        // Share Nodes' card colors, border, radius and typography across themes.
        add_style_node_panel_style(row.button);
        lv_obj_set_size(row.button, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_style_min_height(row.button, 53, 0);
        lv_obj_set_style_pad_all(row.button, 6, 0);
        lv_obj_set_style_pad_column(row.button, 8, 0);
        lv_obj_set_style_bg_image_opa(row.button, LV_OPA_TRANSP, 0);
        // Remove fixed colors so the shared card background follows the theme.
        lv_obj_remove_local_style_prop(row.button, LV_STYLE_BG_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_local_style_prop(row.button, LV_STYLE_BG_COLOR, LV_PART_MAIN | LV_STATE_PRESSED);
        lv_obj_remove_flag(row.button, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN));
        lv_obj_add_flag(row.button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_set_flex_flow(row.button, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row.button, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        lv_obj_t *icon = lv_obj_create(row.button);
        lv_obj_remove_style_all(icon);
        lv_obj_set_size(icon, 36, 36);
        lv_obj_remove_flag(icon, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                                                          LV_OBJ_FLAG_SCROLLABLE));
        lv_obj_add_event_cb(icon, drawPagerHomeIcon, LV_EVENT_DRAW_MAIN, nullptr);

        lv_obj_set_parent(row.label, row.button);
        lv_obj_set_pos(row.label, 0, 0);
        lv_obj_set_size(row.label, 0, LV_SIZE_CONTENT);
        lv_obj_set_flex_grow(row.label, 1);
        lv_obj_set_style_text_line_space(row.label, 0, 0);
        lv_label_set_long_mode(row.label, LV_LABEL_LONG_WRAP);
        lv_obj_remove_flag(row.label, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                                                               LV_OBJ_FLAG_SCROLLABLE));
    }

    // Signal quality was positioned inside the old label using a fixed offset.
    lv_obj_set_parent(objects.home_signal_pct_label, objects.home_signal_button);
    lv_obj_set_pos(objects.home_signal_pct_label, 0, 0);
    lv_obj_set_style_text_align(objects.home_signal_pct_label, LV_TEXT_ALIGN_RIGHT, 0);
}
