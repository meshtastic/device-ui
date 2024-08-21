#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include "lvgl.h"

// styles for tabview buttons
extern lv_style_t style_btn_default;
extern lv_style_t style_btn_active;
extern lv_style_t style_btn_pressed;

#ifdef __cplusplus
extern "C" {
#endif

extern void apply_style_top_panel_style(lv_obj_t *obj);
extern void apply_style_panel_style(lv_obj_t *obj);
extern void apply_style_settings_panel_style(lv_obj_t *obj);
extern void apply_style_node_panel_style(lv_obj_t *obj);
extern void apply_style_home_button_style(lv_obj_t *obj);
extern void apply_style_settings_button_style(lv_obj_t *obj);
extern void apply_style_main_button_style(lv_obj_t *obj);
extern void apply_style_new_message_style(lv_obj_t *obj);
extern void apply_style_chat_message_style(lv_obj_t *obj);
extern void apply_style_tab_view_style(lv_obj_t *obj);
extern void apply_style_drop_down_style(lv_obj_t *obj);
extern void apply_style_bw_label_style(lv_obj_t *obj);
extern void apply_style_color_label_style(lv_obj_t *obj);
extern void apply_style_top_image_style(lv_obj_t *obj);
extern void apply_style_home_container_style(lv_obj_t *obj);
extern void apply_style_button_panel_style(lv_obj_t *obj);
extern void apply_style_alert_panel_style(lv_obj_t *obj);
extern void apply_style_main_screen_style(lv_obj_t *obj);
extern void apply_style_node_button_style(lv_obj_t *obj);
extern void apply_style_channel_button_style(lv_obj_t *obj);
extern void apply_style_button_matrix_style(lv_obj_t *obj);
extern void apply_style_spinner_style(lv_obj_t *obj);
extern void apply_style_settings_label_style(lv_obj_t *obj);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/