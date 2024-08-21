#include "styles.h"
#include "images.h"
#include "fonts.h"


lv_style_t style_btn_default;
lv_style_t style_btn_active;
lv_style_t style_btn_pressed;

#if 0 // see Themes.cpp

void apply_style_top_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff8f8f8), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffafafa), LV_PART_MAIN | LV_STATE_PRESSED);
};
void apply_style_settings_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(obj, lv_color_hex(0xff7e7e7e), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xff979797), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 250, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff003c9f), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_node_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffffff8), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xff979797), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_home_button_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffafaf4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xffd0d0d0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_settings_button_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffafaf4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xffd0d0d0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff294337), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_main_button_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff757575), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff4f4ee), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(obj, lv_color_hex(0xffc0c0c0), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_new_message_style(lv_obj_t *obj) {
    lv_obj_set_style_border_color(obj, lv_color_hex(0xff888888), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff294337), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_chat_message_style(lv_obj_t *obj) {
    lv_obj_set_style_border_color(obj, lv_color_hex(0xff888888), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffbfce9), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_tab_view_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff4f4f4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff003c9f), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_drop_down_style(lv_obj_t *obj) {
};
void apply_style_bw_label_style(lv_obj_t *obj) {
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_color_label_style(lv_obj_t *obj) {
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff003c9f), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_top_image_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_home_container_style(lv_obj_t *obj) {
    lv_obj_set_style_border_color(obj, lv_color_hex(0xffaaaaaa), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffafaf4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(obj, lv_color_hex(0xff999999), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff294337), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_button_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_alert_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffbfbfb), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_main_screen_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_node_button_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffffff8), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_channel_button_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffafaf4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xffd0d0d0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_button_matrix_style(lv_obj_t *obj) {
    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffffff8), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_spinner_style(lv_obj_t *obj) {
    lv_obj_set_style_arc_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(obj, lv_color_hex(0xff000000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(obj, 4, LV_PART_INDICATOR | LV_STATE_DEFAULT);
};
void apply_style_settings_label_style(lv_obj_t *obj) {
    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffffff8), LV_PART_MAIN | LV_STATE_DEFAULT);
};


#endif