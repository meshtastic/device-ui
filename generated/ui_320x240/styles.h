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

// Style: TopPanelStyle
lv_style_t *get_style_top_panel_style_MAIN_DEFAULT();
void add_style_top_panel_style(lv_obj_t *obj);
void remove_style_top_panel_style(lv_obj_t *obj);

// Style: PanelStyle
lv_style_t *get_style_panel_style_MAIN_DEFAULT();
lv_style_t *get_style_panel_style_MAIN_PRESSED();
void add_style_panel_style(lv_obj_t *obj);
void remove_style_panel_style(lv_obj_t *obj);

// Style: SettingsPanelStyle
lv_style_t *get_style_settings_panel_style_MAIN_DEFAULT();
void add_style_settings_panel_style(lv_obj_t *obj);
void remove_style_settings_panel_style(lv_obj_t *obj);

// Style: NodePanelStyle
lv_style_t *get_style_node_panel_style_MAIN_DEFAULT();
void add_style_node_panel_style(lv_obj_t *obj);
void remove_style_node_panel_style(lv_obj_t *obj);

// Style: HomeButtonStyle
lv_style_t *get_style_home_button_style_MAIN_DEFAULT();
void add_style_home_button_style(lv_obj_t *obj);
void remove_style_home_button_style(lv_obj_t *obj);

// Style: SettingsButtonStyle
lv_style_t *get_style_settings_button_style_MAIN_DEFAULT();
void add_style_settings_button_style(lv_obj_t *obj);
void remove_style_settings_button_style(lv_obj_t *obj);

// Style: MainButtonStyle
lv_style_t *get_style_main_button_style_MAIN_DEFAULT();
void add_style_main_button_style(lv_obj_t *obj);
void remove_style_main_button_style(lv_obj_t *obj);

// Style: NewMessageStyle
lv_style_t *get_style_new_message_style_MAIN_DEFAULT();
void add_style_new_message_style(lv_obj_t *obj);
void remove_style_new_message_style(lv_obj_t *obj);

// Style: ChatMessageStyle
lv_style_t *get_style_chat_message_style_MAIN_DEFAULT();
void add_style_chat_message_style(lv_obj_t *obj);
void remove_style_chat_message_style(lv_obj_t *obj);

// Style: TabViewStyle
lv_style_t *get_style_tab_view_style_MAIN_DEFAULT();
void add_style_tab_view_style(lv_obj_t *obj);
void remove_style_tab_view_style(lv_obj_t *obj);

// Style: DropDownStyle
void add_style_drop_down_style(lv_obj_t *obj);
void remove_style_drop_down_style(lv_obj_t *obj);

// Style: BWLabelStyle
lv_style_t *get_style_bw_label_style_MAIN_DEFAULT();
void add_style_bw_label_style(lv_obj_t *obj);
void remove_style_bw_label_style(lv_obj_t *obj);

// Style: ColorLabelStyle
lv_style_t *get_style_color_label_style_MAIN_DEFAULT();
void add_style_color_label_style(lv_obj_t *obj);
void remove_style_color_label_style(lv_obj_t *obj);

// Style: TopImageStyle
lv_style_t *get_style_top_image_style_MAIN_DEFAULT();
void add_style_top_image_style(lv_obj_t *obj);
void remove_style_top_image_style(lv_obj_t *obj);

// Style: HomeContainerStyle
lv_style_t *get_style_home_container_style_MAIN_DEFAULT();
void add_style_home_container_style(lv_obj_t *obj);
void remove_style_home_container_style(lv_obj_t *obj);

// Style: ButtonPanelStyle
lv_style_t *get_style_button_panel_style_MAIN_DEFAULT();
void add_style_button_panel_style(lv_obj_t *obj);
void remove_style_button_panel_style(lv_obj_t *obj);

// Style: AlertPanelStyle
lv_style_t *get_style_alert_panel_style_MAIN_DEFAULT();
void add_style_alert_panel_style(lv_obj_t *obj);
void remove_style_alert_panel_style(lv_obj_t *obj);

// Style: MainScreenStyle
lv_style_t *get_style_main_screen_style_MAIN_DEFAULT();
lv_style_t *get_style_main_screen_style_MAIN_PRESSED();
void add_style_main_screen_style(lv_obj_t *obj);
void remove_style_main_screen_style(lv_obj_t *obj);

// Style: NodeButtonStyle
lv_style_t *get_style_node_button_style_MAIN_DEFAULT();
void add_style_node_button_style(lv_obj_t *obj);
void remove_style_node_button_style(lv_obj_t *obj);

// Style: ChannelButtonStyle
lv_style_t *get_style_channel_button_style_MAIN_DEFAULT();
void add_style_channel_button_style(lv_obj_t *obj);
void remove_style_channel_button_style(lv_obj_t *obj);

// Style: ButtonMatrixStyle
lv_style_t *get_style_button_matrix_style_ITEMS_DEFAULT();
lv_style_t *get_style_button_matrix_style_MAIN_DEFAULT();
void add_style_button_matrix_style(lv_obj_t *obj);
void remove_style_button_matrix_style(lv_obj_t *obj);

// Style: SpinnerStyle
lv_style_t *get_style_spinner_style_MAIN_DEFAULT();
lv_style_t *get_style_spinner_style_INDICATOR_DEFAULT();
void add_style_spinner_style(lv_obj_t *obj);
void remove_style_spinner_style(lv_obj_t *obj);

// Style: SettingsLabelStyle
lv_style_t *get_style_settings_label_style_MAIN_DEFAULT();
void add_style_settings_label_style(lv_obj_t *obj);
void remove_style_settings_label_style(lv_obj_t *obj);

// Style: PositiveImageStyle
lv_style_t *get_style_positive_image_style_MAIN_DEFAULT();
void add_style_positive_image_style(lv_obj_t *obj);
void remove_style_positive_image_style(lv_obj_t *obj);

// Style: StatisticsTableStyle
lv_style_t *get_style_statistics_table_style_MAIN_DEFAULT();
lv_style_t *get_style_statistics_table_style_ITEMS_DEFAULT();
void add_style_statistics_table_style(lv_obj_t *obj);
void remove_style_statistics_table_style(lv_obj_t *obj);

// Style: MapArrowStyle
lv_style_t *get_style_map_arrow_style_MAIN_DEFAULT();
lv_style_t *get_style_map_arrow_style_MAIN_PRESSED();
void add_style_map_arrow_style(lv_obj_t *obj);
void remove_style_map_arrow_style(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/