#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: MainButtonStyle
//

void init_style_main_button_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff404040));
    lv_style_set_text_color(style, lv_color_hex(0xffa6f0ff));
    lv_style_set_border_width(style, 1);
    lv_style_set_shadow_width(style, 1);
    lv_style_set_shadow_spread(style, 1);
    lv_style_set_shadow_color(style, lv_color_hex(0xff707070));
    lv_style_set_shadow_ofs_x(style, 1);
    lv_style_set_shadow_ofs_y(style, 1);
    lv_style_set_bg_image_recolor_opa(style, 255);
    lv_style_set_bg_image_recolor(style, lv_color_hex(0xffd0d0d0));
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

void init_style_main_button_style_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff505050));
    lv_style_set_border_width(style, 2);
    lv_style_set_shadow_width(style, 2);
    lv_style_set_bg_image_recolor(style, lv_color_hex(0xffffffff));
};

lv_style_t *get_style_main_button_style_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_main_button_style_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_main_button_style_MAIN_FOCUSED(lv_style_t *style) {
    lv_style_set_border_width(style, 4);
    lv_style_set_border_color(style, lv_color_hex(0xff67ea38));
    lv_style_set_transform_width(style, 5);
    lv_style_set_transform_height(style, 5);
    lv_style_set_transform_scale_x(style, 260);
    lv_style_set_transform_scale_y(style, 260);
    lv_style_set_bg_image_recolor(style, lv_color_hex(0xffffffff));
};

lv_style_t *get_style_main_button_style_MAIN_FOCUSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_main_button_style_MAIN_FOCUSED(style);
    }
    return style;
};

void add_style_main_button_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_main_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_main_button_style_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_main_button_style_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
};

void remove_style_main_button_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_main_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_main_button_style_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_main_button_style_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
};

//
// Style: PageStyle
//

void init_style_page_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff202020));
};

lv_style_t *get_style_page_style_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_page_style_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_page_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_page_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_page_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_page_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: TopPanelStyle
//

void init_style_top_panel_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff37507e));
    lv_style_set_text_color(style, lv_color_hex(0xffe0e0e0));
    lv_style_set_text_font(style, &lv_font_montserrat_16);
    lv_style_set_pad_top(style, 2);
    lv_style_set_pad_bottom(style, 2);
    lv_style_set_pad_left(style, 2);
    lv_style_set_pad_right(style, 2);
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
    (void)obj;
    lv_obj_add_style(obj, get_style_top_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_top_panel_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_top_panel_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HomeButtonStyle
//

void init_style_home_button_style_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff303030));
    lv_style_set_border_color(style, lv_color_hex(0xff303030));
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_border_width(style, 1);
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
    (void)obj;
    lv_obj_add_style(obj, get_style_home_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_home_button_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_home_button_style_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ScreenStyle
//

void init_style_screen_style_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff202020));
};

lv_style_t *get_style_screen_style_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_screen_style_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_screen_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_screen_style_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_screen_style(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_screen_style_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_main_button_style,
        add_style_page_style,
        add_style_top_panel_style,
        add_style_home_button_style,
        add_style_screen_style,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_main_button_style,
        remove_style_page_style,
        remove_style_top_panel_style,
        remove_style_home_button_style,
        remove_style_screen_style,
    };
    remove_style_funcs[styleIndex](obj);
}

