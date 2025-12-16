#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: MainButtonStyle
lv_style_t *get_style_main_button_style_MAIN_DEFAULT();
lv_style_t *get_style_main_button_style_MAIN_PRESSED();
lv_style_t *get_style_main_button_style_MAIN_FOCUSED();
void add_style_main_button_style(lv_obj_t *obj);
void remove_style_main_button_style(lv_obj_t *obj);

// Style: PageStyle
lv_style_t *get_style_page_style_MAIN_DEFAULT();
void add_style_page_style(lv_obj_t *obj);
void remove_style_page_style(lv_obj_t *obj);

// Style: TopPanelStyle
lv_style_t *get_style_top_panel_style_MAIN_DEFAULT();
void add_style_top_panel_style(lv_obj_t *obj);
void remove_style_top_panel_style(lv_obj_t *obj);

// Style: HomeButtonStyle
lv_style_t *get_style_home_button_style_MAIN_DEFAULT();
void add_style_home_button_style(lv_obj_t *obj);
void remove_style_home_button_style(lv_obj_t *obj);

// Style: ScreenStyle
lv_style_t *get_style_screen_style_MAIN_PRESSED();
void add_style_screen_style(lv_obj_t *obj);
void remove_style_screen_style(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/