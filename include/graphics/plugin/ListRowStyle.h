#pragma once

#include "lvgl.h"
#include <initializer_list>

// Shared layout for the dashboard, node and channel plugins. The selector is
// drawn inside each control so it remains visible at a scroll viewport's edge.
namespace ListRowStyle
{
inline uint32_t &theme()
{
    static uint32_t value = 0;
    return value;
}
inline lv_color_t background()
{
    return lv_color_hex(theme() == 1 ? 0xf1f3f6 : theme() == 2 ? 0x150000 : 0x15171a);
}
inline lv_color_t foreground()
{
    return lv_color_hex(theme() == 1 ? 0x15171a : theme() == 2 ? 0xff6b6b : 0xf0f0f0);
}
inline lv_style_t &baseStyle()
{
    static lv_style_t style;
    static bool initialized = false;
    if (!initialized) {
        lv_style_init(&style);
        initialized = true;
    }
    return style;
}
inline lv_style_t &panelStyle()
{
    static lv_style_t style;
    static bool initialized = false;
    if (!initialized) {
        lv_style_init(&style);
        initialized = true;
    }
    return style;
}
inline void setTheme(uint32_t value)
{
    static bool initialized = false;
    value = value <= 2 ? value : 0;
    if (initialized && theme() == value)
        return;
    theme() = value;
    initialized = true;
    auto &base = baseStyle();
    lv_style_set_bg_color(&base, lv_color_hex(theme() == 1 ? 0xffffff : theme() == 2 ? 0x300d0d : 0x30343b));
    lv_style_set_text_color(&base, foreground());
    lv_style_set_bg_opa(&base, LV_OPA_COVER);
    lv_style_set_border_color(&base, lv_color_hex(theme() == 1 ? 0xb9bec6 : theme() == 2 ? 0x803030 : 0x50545a));
    lv_style_set_border_width(&base, 1);
    lv_style_set_radius(&base, 6);
    lv_style_set_shadow_width(&base, 0);
    auto &panel = panelStyle();
    lv_style_set_bg_color(&panel, background());
    lv_style_set_bg_opa(&panel, LV_OPA_COVER);
    lv_style_set_text_color(&panel, foreground());
    lv_obj_report_style_change(&base);
    lv_obj_report_style_change(&panel);
}
inline void panel(lv_obj_t *obj)
{
    setTheme(theme());
    lv_obj_add_style(obj, &panelStyle(), 0);
}
inline void focus(lv_obj_t *obj)
{
    if (!obj)
        return;
    const int32_t width = lv_display_dpx(lv_obj_get_display(obj), 3);
    for (lv_style_selector_t state : {LV_STATE_FOCUSED, LV_STATE_FOCUS_KEY, LV_STATE_EDITED}) {
        lv_obj_set_style_outline_width(obj, width, state);
        lv_obj_set_style_outline_pad(obj, -width, state);
        lv_obj_set_style_outline_color(obj, lv_palette_main(LV_PALETTE_BLUE), state);
        lv_obj_set_style_outline_opa(obj, LV_OPA_COVER, state);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x164578), state);
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, state);
        lv_obj_set_style_text_color(obj, lv_color_white(), state);
    }
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
}

inline void container(lv_obj_t *obj)
{
    if (!obj)
        return;
    panel(obj);
    for (lv_style_prop_t prop : {LV_STYLE_BG_COLOR, LV_STYLE_BG_OPA, LV_STYLE_TEXT_COLOR})
        lv_obj_remove_local_style_prop(obj, prop, 0);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_pad_left(obj, 8, 0);
    lv_obj_set_style_pad_right(obj, 12, 0);
    lv_obj_set_style_pad_top(obj, 5, 0);
    lv_obj_set_style_pad_bottom(obj, 5, 0);
    lv_obj_set_style_pad_row(obj, 5, 0);
    lv_obj_set_style_pad_right(obj, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_width(obj, 3, LV_PART_SCROLLBAR);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_scroll_dir(obj, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO);
}

inline void row(lv_obj_t *obj)
{
    if (!obj)
        return;
    setTheme(theme());
    lv_obj_add_style(obj, &baseStyle(), 0);
    // Generated widgets use local colors; remove those overrides so both these
    // existing rows and newly created rows share the live theme style.
    for (lv_style_prop_t prop : {LV_STYLE_BG_COLOR, LV_STYLE_BG_OPA, LV_STYLE_TEXT_COLOR, LV_STYLE_RADIUS, LV_STYLE_BORDER_COLOR,
                                 LV_STYLE_BORDER_WIDTH, LV_STYLE_SHADOW_WIDTH})
        lv_obj_remove_local_style_prop(obj, prop, 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_min_height(obj, 53, 0);
    lv_obj_set_style_pad_all(obj, 6, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x164578), LV_STATE_PRESSED);
    lv_obj_set_style_text_color(obj, lv_color_white(), LV_STATE_PRESSED);
    lv_obj_set_style_border_color(obj, lv_palette_main(LV_PALETTE_BLUE), LV_STATE_PRESSED);
    lv_obj_remove_flag(obj, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN));
    focus(obj);
}

inline void text(lv_obj_t *obj)
{
    lv_obj_remove_flag(obj,
                       static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE));
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_line_space(obj, 0, 0);
}
} // namespace ListRowStyle
