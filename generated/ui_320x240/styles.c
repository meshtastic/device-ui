#include "styles.h"
#include "images.h"
#include "fonts.h"


lv_style_t style_btn_default;
lv_style_t style_btn_active;
lv_style_t style_btn_pressed;

#if 1 // see Themes.cpp

#include "screens.h"

//
// Style: TopPanelStyle
//

void init_style_top_panel_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff67ea94));
    lv_style_set_text_color(style, lv_color_hex(0xff212121));
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 0);
};

lv_style_t *get_style_top_panel_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_top_panel_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_top_panel_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_top_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_top_panel_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_top_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: PanelStyle
//

void init_style_panel_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(0xff67ea94));
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 10);
    lv_style_set_pad_bottom(style, 10);
    lv_style_set_bg_color(style, lv_color_hex(0xfff4f4f0));
    lv_style_set_shadow_color(style, lv_color_hex(0xffe0e0e0));
    lv_style_set_radius(style, 0);
};

lv_style_t *get_style_panel_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_style_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_panel_style_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xfffafafa));
};

lv_style_t *get_style_panel_style_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_style_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_panel_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_panel_style_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_panel_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_panel_style_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: SettingsPanelStyle
//

void init_style_settings_panel_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xfff0f0f0));
    lv_style_set_shadow_color(style, lv_color_hex(0xff7e7e7e));
    lv_style_set_border_color(style, lv_color_hex(0xff979797));
    lv_style_set_bg_opa(style, 250);
    lv_style_set_radius(style, 0);
    lv_style_set_text_color(style, lv_color_hex(0xff003c9f));
};

lv_style_t *get_style_settings_panel_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_panel_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_settings_panel_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_settings_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_settings_panel_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_settings_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: NodePanelStyle
//

void init_style_node_panel_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
    lv_style_set_border_color(style, lv_color_hex(0xff979797));
    lv_style_set_border_width(style, 1);
    lv_style_set_radius(style, 10);
    lv_style_set_text_font(style, &ui_font_montserrat_12);
    lv_style_set_text_color(style, lv_color_hex(0xff212121));
};

lv_style_t *get_style_node_panel_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_node_panel_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_node_panel_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_node_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_node_panel_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_node_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HomeButtonStyle
//

void init_style_home_button_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
    lv_style_set_bg_image_recolor(style, lv_color_hex(0xff57a6b3));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_color(style, lv_color_hex(0xffd0d0d0));
    lv_style_set_text_color(style, lv_color_hex(0xff101010));
    lv_style_set_bg_image_recolor_opa(style, 255);
};

lv_style_t *get_style_home_button_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_home_button_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_home_button_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_home_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_home_button_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_home_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SettingsButtonStyle
//

void init_style_settings_button_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xfffafaf4));
    lv_style_set_bg_image_recolor_opa(style, 255);
    lv_style_set_bg_image_recolor(style, lv_color_hex(0xff67ea94));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_color(style, lv_color_hex(0xffd0d0d0));
    lv_style_set_text_color(style, lv_color_hex(0xff294337));
};

lv_style_t *get_style_settings_button_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_button_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_settings_button_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_settings_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_settings_button_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_settings_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: MainButtonStyle
//

void init_style_main_button_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_image_recolor(style, lv_color_hex(0xff757575));
    lv_style_set_border_color(style, lv_color_hex(0xff67ea94));
    lv_style_set_border_side(style, LV_BORDER_SIDE_RIGHT);
    lv_style_set_border_width(style, 3);
    lv_style_set_radius(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(0xfff4f4ee));
    lv_style_set_text_color(style, lv_color_hex(0xff101010));
    lv_style_set_bg_image_recolor_opa(style, 255);
    lv_style_set_shadow_color(style, lv_color_hex(0xffc0c0c0));
};

lv_style_t *get_style_main_button_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_main_button_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_main_button_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_main_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_main_button_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_main_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: NewMessageStyle
//

void init_style_new_message_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(0xff888888));
    lv_style_set_border_width(style, 1);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 2);
    lv_style_set_pad_bottom(style, 2);
    lv_style_set_pad_left(style, 2);
    lv_style_set_pad_right(style, 2);
    lv_style_set_text_font(style, &ui_font_montserrat_12);
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(0xff294337));
};

lv_style_t *get_style_new_message_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_new_message_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_new_message_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_new_message_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_new_message_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_new_message_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ChatMessageStyle
//

void init_style_chat_message_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(0xff888888));
    lv_style_set_border_width(style, 2);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 2);
    lv_style_set_pad_bottom(style, 2);
    lv_style_set_pad_left(style, 2);
    lv_style_set_pad_right(style, 2);
    lv_style_set_text_font(style, &ui_font_montserrat_12);
    lv_style_set_bg_color(style, lv_color_hex(0xfffbfce9));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
};

lv_style_t *get_style_chat_message_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_chat_message_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_chat_message_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_chat_message_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_chat_message_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_chat_message_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: TabViewStyle
//

void init_style_tab_view_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xfff4f4f4));
    lv_style_set_text_color(style, lv_color_hex(0xff003c9f));
};

lv_style_t *get_style_tab_view_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_tab_view_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_tab_view_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_tab_view_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_tab_view_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_tab_view_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: DropDownStyle
//

void add_style_drop_down_style(lv_obj_t *obj) {
};

void remove_style_drop_down_style(lv_obj_t *obj) {
};

//
// Style: BWLabelStyle
//

void init_style_bw_label_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xff212121));
    lv_style_set_text_font(style, &ui_font_montserrat_12);
};

lv_style_t *get_style_bw_label_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bw_label_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_bw_label_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_bw_label_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_bw_label_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_bw_label_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ColorLabelStyle
//

void init_style_color_label_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xff003c9f));
};

lv_style_t *get_style_color_label_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_color_label_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_color_label_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_color_label_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_color_label_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_color_label_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: TopImageStyle
//

void init_style_top_image_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff67ea94));
    lv_style_set_image_recolor(style, lv_color_hex(0xff212121));
    lv_style_set_image_recolor_opa(style, 255);
    lv_style_set_bg_image_recolor(style, lv_color_hex(0xff212121));
    lv_style_set_bg_image_recolor_opa(style, 255);
};

lv_style_t *get_style_top_image_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_top_image_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_top_image_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_top_image_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_top_image_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_top_image_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HomeContainerStyle
//

void init_style_home_container_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(0xffaaaaaa));
    lv_style_set_border_width(style, 3);
    lv_style_set_border_side(style, LV_BORDER_SIDE_FULL);
    lv_style_set_bg_color(style, lv_color_hex(0xfffafaf4));
    lv_style_set_shadow_color(style, lv_color_hex(0xff999999));
    lv_style_set_text_font(style, &ui_font_montserrat_16);
    lv_style_set_radius(style, 10);
    lv_style_set_text_color(style, lv_color_hex(0xff294337));
};

lv_style_t *get_style_home_container_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_home_container_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_home_container_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_home_container_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_home_container_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_home_container_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonPanelStyle
//

void init_style_button_panel_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
    lv_style_set_radius(style, 0);
};

lv_style_t *get_style_button_panel_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_panel_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_panel_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_button_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_panel_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_button_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: AlertPanelStyle
//

void init_style_alert_panel_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xfffbfbfb));
};

lv_style_t *get_style_alert_panel_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_alert_panel_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_alert_panel_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_alert_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_alert_panel_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_alert_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: MainScreenStyle
//

void init_style_main_screen_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffe0e0e0));
};

lv_style_t *get_style_main_screen_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_main_screen_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_main_screen_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_main_screen_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_main_screen_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_main_screen_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: NodeButtonStyle
//

void init_style_node_button_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xfffffff8));
    lv_style_set_bg_opa(style, 0);
};

lv_style_t *get_style_node_button_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_node_button_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_node_button_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_node_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_node_button_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_node_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ChannelButtonStyle
//

void init_style_channel_button_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xfffafaf4));
    lv_style_set_border_color(style, lv_color_hex(0xffd0d0d0));
    lv_style_set_border_width(style, 1);
    lv_style_set_text_color(style, lv_color_hex(0xff101010));
};

lv_style_t *get_style_channel_button_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_channel_button_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_channel_button_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_channel_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_channel_button_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_channel_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonMatrixStyle
//

void init_style_button_matrix_style_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(0xff67ea94));
    lv_style_set_bg_color(style, lv_color_hex(0xfffffff8));
    lv_style_set_text_color(style, lv_color_hex(0xff212121));
};

lv_style_t *get_style_button_matrix_style_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_matrix_style_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_button_matrix_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffe0e0e0));
};

lv_style_t *get_style_button_matrix_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_matrix_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_matrix_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_button_matrix_style_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_matrix_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_matrix_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_button_matrix_style_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_matrix_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SpinnerStyle
//

void init_style_spinner_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(0xff67ea94));
    lv_style_set_arc_width(style, 5);
};

lv_style_t *get_style_spinner_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_spinner_style_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_spinner_style_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(0xff67ea94));
    lv_style_set_arc_width(style, 4);
};

lv_style_t *get_style_spinner_style_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_spinner_style_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_spinner_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_spinner_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_spinner_style_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_spinner_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_spinner_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_spinner_style_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: SettingsLabelStyle
//

void init_style_settings_label_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(0xff808080));
    lv_style_set_border_width(style, 1);
    lv_style_set_bg_opa(style, 255);
    lv_style_set_bg_color(style, lv_color_hex(0xfffffff8));
};

lv_style_t *get_style_settings_label_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_label_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_settings_label_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_settings_label_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_settings_label_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_settings_label_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: PositiveImageStyle
//

void init_style_positive_image_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_image_recolor(style, lv_color_hex(0xff212121));
    lv_style_set_image_recolor_opa(style, 255);
};

lv_style_t *get_style_positive_image_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_positive_image_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_positive_image_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_positive_image_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_positive_image_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_positive_image_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StatisticsTableStyle
//

void init_style_statistics_table_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_montserrat_12);
};

lv_style_t *get_style_statistics_table_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_statistics_table_style_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_statistics_table_style_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xfff4f4f0));
    lv_style_set_text_color(style, lv_color_hex(0xff212121));
    lv_style_set_border_color(style, lv_color_hex(0xffe0e0e0));
};

lv_style_t *get_style_statistics_table_style_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_statistics_table_style_ITEMS_DEFAULT(style);
    }
    return style;
};

void add_style_statistics_table_style(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_statistics_table_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_statistics_table_style_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
};

void remove_style_statistics_table_style(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_statistics_table_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_statistics_table_style_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_top_panel_style,
        add_style_panel_style,
        add_style_settings_panel_style,
        add_style_node_panel_style,
        add_style_home_button_style,
        add_style_settings_button_style,
        add_style_main_button_style,
        add_style_new_message_style,
        add_style_chat_message_style,
        add_style_tab_view_style,
        add_style_drop_down_style,
        add_style_bw_label_style,
        add_style_color_label_style,
        add_style_top_image_style,
        add_style_home_container_style,
        add_style_button_panel_style,
        add_style_alert_panel_style,
        add_style_main_screen_style,
        add_style_node_button_style,
        add_style_channel_button_style,
        add_style_button_matrix_style,
        add_style_spinner_style,
        add_style_settings_label_style,
        add_style_positive_image_style,
        add_style_statistics_table_style,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_top_panel_style,
        remove_style_panel_style,
        remove_style_settings_panel_style,
        remove_style_node_panel_style,
        remove_style_home_button_style,
        remove_style_settings_button_style,
        remove_style_main_button_style,
        remove_style_new_message_style,
        remove_style_chat_message_style,
        remove_style_tab_view_style,
        remove_style_drop_down_style,
        remove_style_bw_label_style,
        remove_style_color_label_style,
        remove_style_top_image_style,
        remove_style_home_container_style,
        remove_style_button_panel_style,
        remove_style_alert_panel_style,
        remove_style_main_screen_style,
        remove_style_node_button_style,
        remove_style_channel_button_style,
        remove_style_button_matrix_style,
        remove_style_spinner_style,
        remove_style_settings_label_style,
        remove_style_positive_image_style,
        remove_style_statistics_table_style,
    };
    remove_style_funcs[styleIndex](obj);
}


#endif