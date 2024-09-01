#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include "lv_i18n.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

void create_screen_boot_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.boot_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // MeshtasticLogo
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.meshtastic_logo = obj;
            lv_obj_set_pos(obj, 0, LV_PCT(-10));
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_meshtastic_boot_logo_image);
            lv_img_set_zoom(obj, 255);
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 210, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // MeshtasticUrl
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.meshtastic_url = obj;
            lv_obj_set_pos(obj, 0, LV_PCT(-15));
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, " www.meshtastic.org");
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // FirmwareLabel
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.firmware_label = obj;
            lv_obj_set_pos(obj, 0, -15);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // RebootPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.reboot_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // CancelRebootButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.cancel_reboot_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_PRESSED);
                }
                {
                    // RebootButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.reboot_button = obj;
                    lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(-10));
                    lv_obj_set_size(obj, 55, 55);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff229c00), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_image_src(obj, &img_reboot_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_x(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // ShutdownButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.shutdown_button = obj;
                    lv_obj_set_pos(obj, LV_PCT(-25), LV_PCT(-10));
                    lv_obj_set_size(obj, 55, 55);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_image_src(obj, &img_shutdown_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_x(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, LV_PCT(-15));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, " www.meshtastic.org");
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
}

void tick_screen_boot_screen() {
}

void create_screen_main_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    add_style_main_screen_style(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ButtonPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.button_panel = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_PCT(12), LV_PCT(100));
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM);
            add_style_button_panel_style(obj);
            lv_obj_set_style_max_width(obj, 38, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 536870911, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_width(obj, 38, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // HomeButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.home_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                    add_style_main_button_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_home_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // NodesButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.nodes_button = obj;
                    lv_obj_set_pos(obj, -12, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                    add_style_main_button_style(obj);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_nodes_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // GroupsButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.groups_button = obj;
                    lv_obj_set_pos(obj, -12, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                    add_style_main_button_style(obj);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_groups_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // MessagesButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.messages_button = obj;
                    lv_obj_set_pos(obj, -12, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                    add_style_main_button_style(obj);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_messages_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // MapButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.map_button = obj;
                    lv_obj_set_pos(obj, -12, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                    add_style_main_button_style(obj);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_map_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_button = obj;
                    lv_obj_set_pos(obj, -12, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                    add_style_main_button_style(obj);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_settings_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // HomePanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.home_panel = obj;
            lv_obj_set_pos(obj, 39, 24);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_panel_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // HomeContainer
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.home_container = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(95), LV_PCT(100));
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SNAPPABLE);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    add_style_home_container_style(obj);
                    lv_obj_set_style_shadow_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_x(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_spread(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW_WRAP, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_grow(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_FOCUSED);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_FOCUSED);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_FOCUSED);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_FOCUSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // HomeMailButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_mail_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                            add_style_home_button_style(obj);
                            lv_obj_set_style_bg_image_src(obj, &img_home_mail_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff9b9bff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_PRESSED);
                        }
                        {
                            // HomeMailLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_mail_label = obj;
                            lv_obj_set_pos(obj, 105, 50);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, _("no new messages"));
                        }
                        {
                            // HomeNodesButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_nodes_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                            add_style_home_button_style(obj);
                            lv_obj_set_style_bg_image_src(obj, &img_home_nodes_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff9b9bff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                        }
                        {
                            // HomeNodesLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_nodes_label = obj;
                            lv_obj_set_pos(obj, 105, 50);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, _("1 of 1 nodes online"));
                        }
                        {
                            // HomeTimeButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_time_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                            add_style_home_button_style(obj);
                            lv_obj_set_style_bg_image_src(obj, &img_home_clock_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff9b9bff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                        }
                        {
                            // HomeTimeLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_time_label = obj;
                            lv_obj_set_pos(obj, 105, 50);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "uptime 00:00:00");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                        }
                        {
                            // HomeLocationButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_location_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                            add_style_home_button_style(obj);
                            lv_obj_set_style_bg_image_src(obj, &img_home_location_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff9b9bff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                        }
                        {
                            // HomeLocationLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_location_label = obj;
                            lv_obj_set_pos(obj, 105, 50);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "N???° ??.????'\nW???° ??.????'");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                        }
                        {
                            // HomeWLANButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_wlan_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                            add_style_home_button_style(obj);
                            lv_obj_set_style_bg_image_src(obj, &img_home_wlan_off_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff9b9bff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_PRESSED);
                        }
                        {
                            // HomeWLANLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_wlan_label = obj;
                            lv_obj_set_pos(obj, 105, 50);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, "0.0.0.0");
                        }
                        {
                            // HomeEthernetButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_ethernet_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                            add_style_home_button_style(obj);
                            lv_obj_set_style_bg_image_src(obj, &img_home_ethernet_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff9b9bff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_PRESSED);
                        }
                        {
                            // HomeEthernetLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_ethernet_label = obj;
                            lv_obj_set_pos(obj, 105, 50);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, "0.0.0.0");
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                        }
                        {
                            // HomeBluetoothButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_bluetooth_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                            add_style_home_button_style(obj);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_src(obj, &img_home_bluetooth_on_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff9b9bff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                        }
                        {
                            // HomeBluetoothLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_bluetooth_label = obj;
                            lv_obj_set_pos(obj, 105, 50);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, "00:00:00:00:00:00");
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                        }
                        {
                            // HomeMQTTButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_mqtt_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                            add_style_home_button_style(obj);
                            lv_obj_set_style_bg_image_src(obj, &img_mqtt_logo2_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffc0c0c0), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff9b9bff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                        }
                        {
                            // HomeMQTTLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_mqtt_label = obj;
                            lv_obj_set_pos(obj, 105, 50);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "mqtt");
                        }
                        {
                            // HomeMemoryButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_memory_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                            add_style_home_button_style(obj);
                            lv_obj_set_style_bg_image_src(obj, &img_home_memory_button, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff9b9bff), LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_PRESSED);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
                        }
                        {
                            // HomeMemoryLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_memory_label = obj;
                            lv_obj_set_pos(obj, 105, 50);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Heap: 0\nLVGL: 0");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                        }
                    }
                }
            }
        }
        {
            // NodesPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.nodes_panel = obj;
            lv_obj_set_pos(obj, 39, 24);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
            add_style_panel_style(obj);
            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_grow(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // NodePanel
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.node_panel = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), 0);
                    lv_obj_set_size(obj, LV_PCT(100), 52);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_node_panel_style(obj);
                    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfffffff4), LV_PART_MAIN | LV_STATE_PRESSED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // NodeImage
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.node_image = obj;
                            lv_obj_set_pos(obj, -5, -10);
                            lv_obj_set_size(obj, 32, 32);
                            lv_img_set_src(obj, &img_node_client_image);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff5d9388), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff5d9388), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // NodeButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.node_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_PCT(106), LV_PCT(100));
                            add_style_node_button_style(obj);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 132, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // UserNameLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.user_name_label = obj;
                            lv_obj_set_pos(obj, -5, 22);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, "Meshtastic");
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // UserNameShortLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.user_name_short_label = obj;
                            lv_obj_set_pos(obj, 30, -3);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "ABCD");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // BatteryLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.battery_label = obj;
                            lv_obj_set_pos(obj, 8, 4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, "");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // LastHeardLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.last_heard_label = obj;
                            lv_obj_set_pos(obj, 8, 20);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, "");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // SignalLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.signal_label = obj;
                            lv_obj_set_pos(obj, 8, -12);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, "");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // PositionLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.position_label = obj;
                            lv_obj_set_pos(obj, -5, 36);
                            lv_obj_set_size(obj, 120, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, "");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff05f6cb), LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // Position2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.position2_label = obj;
                            lv_obj_set_pos(obj, -5, 50);
                            lv_obj_set_size(obj, 108, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, "");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // Telemetry1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.telemetry1_label = obj;
                            lv_obj_set_pos(obj, 8, 36);
                            lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, "");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // Telemetry2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.telemetry2_label = obj;
                            lv_obj_set_pos(obj, 8, 50);
                            lv_obj_set_size(obj, 130, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, "");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
            }
        }
        {
            // GroupsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.groups_panel = obj;
            lv_obj_set_pos(obj, 39, 24);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_panel_style(obj);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ChannelButton0
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.channel_button0 = obj;
                    lv_obj_set_pos(obj, 122, 39);
                    lv_obj_set_size(obj, LV_PCT(30), 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_channel_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ChannelLabel0
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.channel_label0 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_label_set_text(obj, "0");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // ChannelButton1
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.channel_button1 = obj;
                    lv_obj_set_pos(obj, 122, 39);
                    lv_obj_set_size(obj, LV_PCT(30), 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_channel_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ChannelLabel1
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.channel_label1 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_label_set_text(obj, "1");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // ChannelButton2
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.channel_button2 = obj;
                    lv_obj_set_pos(obj, 122, 39);
                    lv_obj_set_size(obj, LV_PCT(30), 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_channel_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ChannelLabel2
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.channel_label2 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_label_set_text(obj, "2");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // ChannelButton3
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.channel_button3 = obj;
                    lv_obj_set_pos(obj, 122, 39);
                    lv_obj_set_size(obj, LV_PCT(30), 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_channel_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ChannelLabel3
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.channel_label3 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_label_set_text(obj, "3");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // ChannelButton4
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.channel_button4 = obj;
                    lv_obj_set_pos(obj, 122, 39);
                    lv_obj_set_size(obj, LV_PCT(30), 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_channel_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ChannelLabel4
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.channel_label4 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_label_set_text(obj, "4");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // ChannelButton5
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.channel_button5 = obj;
                    lv_obj_set_pos(obj, 122, 39);
                    lv_obj_set_size(obj, LV_PCT(30), 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_channel_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ChannelLabel5
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.channel_label5 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_label_set_text(obj, "5");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // ChannelButton6
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.channel_button6 = obj;
                    lv_obj_set_pos(obj, 122, 39);
                    lv_obj_set_size(obj, LV_PCT(30), 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_channel_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ChannelLabel6
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.channel_label6 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_label_set_text(obj, "6");
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // ChannelButton7
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.channel_button7 = obj;
                    lv_obj_set_pos(obj, 122, 39);
                    lv_obj_set_size(obj, LV_PCT(30), 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_channel_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ChannelLabel7
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.channel_label7 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_label_set_text(obj, "7");
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
            }
        }
        {
            // MessagesPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.messages_panel = obj;
            lv_obj_set_pos(obj, 39, 24);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ACTIVE);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_panel_style(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // MessagesContainer
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.messages_container = obj;
                    lv_obj_set_pos(obj, LV_PCT(-5), LV_PCT(-6));
                    lv_obj_set_size(obj, LV_PCT(110), LV_PCT(97));
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                    add_style_panel_style(obj);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 44, 31);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Text\nLine2 Word2\nLine3");
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                            add_style_new_message_style(obj);
                        }
                    }
                }
                {
                    // MessageInputArea
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.message_input_area = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, LV_PCT(90), 25);
                    lv_textarea_set_max_length(obj, 237);
                    lv_textarea_set_placeholder_text(obj, "Enter Text ...");
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_scroll_dir(obj, LV_DIR_HOR);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_height(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // KeyboardButton_0
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.keyboard_button_0 = obj;
                    lv_obj_set_pos(obj, 5, 10);
                    lv_obj_set_size(obj, 25, 24);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_keyboard_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // ChatsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.chats_panel = obj;
            lv_obj_set_pos(obj, 39, 24);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
            add_style_panel_style(obj);
            lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ChatsButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.chats_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(100), 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    add_style_home_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xfffc8c04), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_x(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ChatsButtonLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.chats_button_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_label_set_text(obj, "");
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // ChatDelButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.chat_del_button = obj;
                            lv_obj_set_pos(obj, -3, -1);
                            lv_obj_set_size(obj, 40, 23);
                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffa70a0a), LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // DelLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.del_label = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_text(obj, _("DEL"));
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                            }
                        }
                    }
                }
            }
        }
        {
            // MapPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.map_panel = obj;
            lv_obj_set_pos(obj, 39, 24);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_panel_style(obj);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_image_src(obj, &img_worldmap_image, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // ControllerPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.controller_panel = obj;
            lv_obj_set_pos(obj, 39, 24);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SNAPPABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
            add_style_panel_style(obj);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ControllerTabView
                    lv_obj_t *obj = lv_tabview_create(parent_obj);
                    lv_tabview_set_tab_bar_position(obj, LV_DIR_BOTTOM);
                    lv_tabview_set_tab_bar_size(obj, 25);
                    objects.controller_tab_view = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(108), LV_PCT(110));
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
                    add_style_tab_view_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // TabPageBasicSettings
                            lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Settings");
                            objects.tab_page_basic_settings = obj;
                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ACTIVE);
                            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_row(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // BasicSettingsUserButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_user_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsUserLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_user_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("User name: "));
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsRegionButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_region_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsRegionLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_region_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, "Region: <Unset>");
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsModemPresetButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_modem_preset_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsModemPresetLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_modem_preset_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Modem Preset: LONG FAST"));
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsChannelButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_channel_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsChannelLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_channel_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Channel: LongFast"));
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsRoleButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_role_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsRoleLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_role_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Role: Client"));
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsTimeoutButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_timeout_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsTimeoutLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_timeout_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Screen Timeout: 60s"));
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsScreenLockButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_screen_lock_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsScreenLockLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_screen_lock_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Screen Lock: off"));
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsBrightnessButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_brightness_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsBrightnessLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_brightness_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Screen Brightness: 60%"));
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DISABLED);
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsThemeButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_theme_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsThemeLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_theme_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Theme: Dark"));
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DISABLED);
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsCalibrationButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_calibration_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsCalibrationLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_calibration_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Screen Calibration: default"));
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DISABLED);
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsInputButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_input_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsInputLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_input_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Input Control: none/none"));
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsAlertButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_alert_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsAlertLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_alert_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Message Alert Buzzer: on"));
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsLanguageButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_language_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsLanguageLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_language_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Language: English"));
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsTimezoneButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_timezone_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsTimezoneLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_timezone_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Language: English"));
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsResetButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_reset_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsResetLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_reset_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Configuration  Reset"));
                                        }
                                    }
                                }
                                {
                                    // BasicSettingsRebootButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.basic_settings_reboot_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // BasicSettingsRebootLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.basic_settings_reboot_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Reboot / Shutdown"));
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // TabPageTools
                            lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Tools");
                            objects.tab_page_tools = obj;
                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ACTIVE);
                            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_row(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // ToolsMeshDetectorButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.tools_mesh_detector_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // ToolsMeshDetectorLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.tools_mesh_detector_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Mesh Detector"));
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                                        }
                                    }
                                }
                                {
                                    // ToolsSignalScannerButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.tools_signal_scanner_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // ToolsSignalScannerLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.tools_signal_scanner_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Signal Scanner"));
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                                        }
                                    }
                                }
                                {
                                    // ToolsTraceRouteButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.tools_trace_route_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // ToolsTraceRouteLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.tools_trace_route_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Trace Route"));
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                                        }
                                    }
                                }
                                {
                                    // ToolsNeighborsButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.tools_neighbors_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // ToolsNeighborsLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.tools_neighbors_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Neighbors"));
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                                        }
                                    }
                                }
                                {
                                    // ToolsStatisticsButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.tools_statistics_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // ToolsStatisticsLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.tools_statistics_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Statistics"));
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                                        }
                                    }
                                }
                                {
                                    // ToolsDebugLogButton
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.tools_debug_log_button = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(95), 30);
                                    add_style_settings_button_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff4db270), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // ToolsDebugLogLabel
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.tools_debug_log_label = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_label_set_text(obj, _("Debug Log"));
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        {
            // AdvancedSettingsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.advanced_settings_panel = obj;
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // TopPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
            add_style_top_panel_style(obj);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // MeshtasticImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.meshtastic_image = obj;
                    lv_obj_set_pos(obj, -60, -9);
                    lv_obj_set_size(obj, 36, 17);
                    lv_img_set_src(obj, &img_meshtastic_logo_image);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_image_opa(obj, 255, LV_PART_MAIN | LV_STATE_DISABLED);
                    lv_obj_set_style_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DISABLED);
                    lv_obj_set_style_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DISABLED);
                }
                {
                    // MeshtasticLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.meshtastic_label = obj;
                    lv_obj_set_pos(obj, 14, -12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Meshtastic");
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopNodesPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_nodes_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopNodesOnlineLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_nodes_online_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("1 of 1 nodes online"));
                    lv_obj_set_style_text_font(obj, &ui_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopNodesImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_nodes_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_nodes_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopGroupsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_groups_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopUserGroupLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_user_group_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("Group Channels"));
                    lv_obj_set_style_text_font(obj, &ui_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopGroupImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_group_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_group_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopGroupChatPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_group_chat_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopGroupChatLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_group_chat_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("no messages"));
                    lv_obj_set_style_text_font(obj, &ui_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopGroupChatImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_group_chat_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_group_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopSettingsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_settings_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopBasicSettingsLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_basic_settings_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("Settings & Tools"));
                    lv_obj_set_style_text_font(obj, &ui_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopBasicSettingsImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_basic_settings_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_settings_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopAdvancedSettingsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_advanced_settings_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopAdvancedSettingsLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_advanced_settings_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, _("Settings (advanced)"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopAdvancedSettingsImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_advanced_settings_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_settings_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopMapPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_map_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopMapLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_map_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("Locations Map"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopMapImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_map_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_map_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopChatsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_chats_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopChatsLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_chats_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("no chats"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopChatsImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_chats_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_chats_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopMessagesPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_messages_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopMessagesNodeLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_messages_node_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("no messages"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopMessagesNodeImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_messages_node_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_message_node_images, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopNodeSearchPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_node_search_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopNodeSearchLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_node_search_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("Node Search"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopNodeSearchImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_node_search_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_node_detector_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopMeshDetectorPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_mesh_detector_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopMeshDetectorLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_mesh_detector_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("Mesh Detector"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopMeshDetectorImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_mesh_detector_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_node_sensor_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopSignalScannerPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_signal_scanner_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopSignalScannerLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_signal_scanner_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("Signal Scanner"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopSignalScannerImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_signal_scanner_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_signal_full_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopTraceRoutePanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_trace_route_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopTraceRouteLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_trace_route_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("Trace Route"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopTraceRouteImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_trace_route_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_trace_route_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopNeighborsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_neighbors_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopNeighborsLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_neighbors_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("Neigbors"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopNeighborsImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_neighbors_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_neighbors_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopStatisticsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_statistics_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopStatisticsLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_statistics_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("Statistics"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopStatisticsImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_statistics_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_chart_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopDebugLogPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_debug_log_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopDebugLogLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_debug_log_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("Debug Log"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopDebugLogImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_debug_log_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_debug_log_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TopNodeOptionsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.top_node_options_panel = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_PCT(80), 24);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopNodeOptionsLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_node_options_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
                    lv_label_set_text(obj, _("Node Options"));
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopNodeOptionsImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_node_options_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_top_settings_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // BatteryPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.battery_panel = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_PCT(19), 24);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            add_style_top_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // BatteryImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.battery_image = obj;
                    lv_obj_set_pos(obj, 13, 0);
                    lv_obj_set_size(obj, 24, 24);
                    add_style_top_image_style(obj);
                    lv_obj_set_style_bg_image_src(obj, &img_battery_full_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // BatteryPercentageLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.battery_percentage_label = obj;
                    lv_obj_set_pos(obj, -10, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, " ");
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_bw_label_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsUsernamePanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_username_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(5), LV_PCT(15));
            lv_obj_set_size(obj, 240, 165);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 250, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, _("Short Name"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DISABLED);
                }
                {
                    // SettingsUserShortTextarea
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.settings_user_short_textarea = obj;
                    lv_obj_set_pos(obj, 0, 18);
                    lv_obj_set_size(obj, LV_PCT(42), 25);
                    lv_textarea_set_max_length(obj, 4);
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // KeyboardButton_1
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.keyboard_button_1 = obj;
                    lv_obj_set_pos(obj, 163, 18);
                    lv_obj_set_size(obj, 25, 24);
                    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_keyboard_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 60);
                    lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, _("Long Name"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsUserLongTextarea
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.settings_user_long_textarea = obj;
                    lv_obj_set_pos(obj, 0, 80);
                    lv_obj_set_size(obj, LV_PCT(88), 25);
                    lv_textarea_set_max_length(obj, 39);
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // KeyboardButton_2
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.keyboard_button_2 = obj;
                    lv_obj_set_pos(obj, 200, 80);
                    lv_obj_set_size(obj, 25, 24);
                    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_keyboard_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj0 = obj;
                    lv_obj_set_pos(obj, 4, 14);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 156);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsChannelPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_channel_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(5), LV_PCT(12));
            lv_obj_set_size(obj, LV_PCT(70), LV_PCT(85));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_margin_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 360, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 320, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, "Primary Channel");
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DISABLED);
                }
                {
                    // SettingsChannel0Button
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_channel0_button = obj;
                    lv_obj_set_pos(obj, 0, 18);
                    lv_obj_set_size(obj, LV_PCT(100), 25);
                    add_style_settings_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // SettingsChannel0Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_channel0_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, _("<unset>"));
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 50);
                    lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, "Secondary Channels");
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsChannel1Button
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_channel1_button = obj;
                    lv_obj_set_pos(obj, 0, 18);
                    lv_obj_set_size(obj, LV_PCT(100), 25);
                    add_style_settings_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // SettingsChannel1Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_channel1_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, _("<unset>"));
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // SettingsChannel2Button
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_channel2_button = obj;
                    lv_obj_set_pos(obj, 0, 18);
                    lv_obj_set_size(obj, LV_PCT(100), 25);
                    add_style_settings_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // SettingsChannel2Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_channel2_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, _("<unset>"));
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // SettingsChannel3Button
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_channel3_button = obj;
                    lv_obj_set_pos(obj, 0, 18);
                    lv_obj_set_size(obj, LV_PCT(100), 25);
                    add_style_settings_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // SettingsChannel3Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_channel3_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, _("<unset>"));
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // SettingsChannel4Button
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_channel4_button = obj;
                    lv_obj_set_pos(obj, 0, 18);
                    lv_obj_set_size(obj, LV_PCT(100), 25);
                    add_style_settings_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // SettingsChannel4Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_channel4_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, _("<unset>"));
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // SettingsChannel5Button
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_channel5_button = obj;
                    lv_obj_set_pos(obj, 0, 18);
                    lv_obj_set_size(obj, LV_PCT(100), 25);
                    add_style_settings_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // SettingsChannel5Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_channel5_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, "<unset>");
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // SettingsChannel6Button
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_channel6_button = obj;
                    lv_obj_set_pos(obj, 0, 18);
                    lv_obj_set_size(obj, LV_PCT(100), 25);
                    add_style_settings_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // SettingsChannel6Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_channel6_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, _("<unset>"));
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // SettingsChannel7Button
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_channel7_button = obj;
                    lv_obj_set_pos(obj, 0, 18);
                    lv_obj_set_size(obj, LV_PCT(100), 25);
                    add_style_settings_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff015114), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DISABLED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // SettingsChannel7Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_channel7_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, _("<unset>"));
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj1 = obj;
                    lv_obj_set_pos(obj, 4, 14);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 160);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsRegionPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_region_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(25));
            lv_obj_set_size(obj, 220, 120);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SettingsRegionDropdown
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_region_dropdown = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 150, 30);
                    lv_dropdown_set_options(obj, "US\nEU_433\nEU_868\nCN\nJP\nANZ\nKR\nTW\nRU\nIN\nNZ_865\nTH\nLORA_24\nUA_433\nUA_868\nMY_433\nMY_919\nSG_923");
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj2 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 184);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsModemPresetPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_modem_preset_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(20));
            lv_obj_set_size(obj, 220, 160);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SettingsModemPresetDropdown
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_modem_preset_dropdown = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 180, 30);
                    lv_dropdown_set_options(obj, _("LONG FAST\nLONG SLOW\nVERY LONG SLOW\nMEDIUM SLOW\nMEDIUM FAST\nSHORT SLOW\nSHORT FAST\nLONG MODERATE"));
                    add_style_drop_down_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // FrequencySlotLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.frequency_slot_label = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), 40);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, "FrequencySlot: 1 (902.0MHz)");
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // FrequencySlotSlider
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.frequency_slot_slider = obj;
                    lv_obj_set_pos(obj, LV_PCT(10), 65);
                    lv_obj_set_size(obj, LV_PCT(80), 10);
                    lv_slider_set_range(obj, 1, 1);
                    lv_slider_set_value(obj, 1, LV_ANIM_OFF);
                    lv_obj_set_style_bg_image_src(obj, &img_knob_logo_image, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff000000), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xffa8cacd), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj3 = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 188);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsDeviceRolePanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_device_role_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(25));
            lv_obj_set_size(obj, 220, 120);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SettingsDeviceRoleDropdown
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_device_role_dropdown = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 150, 30);
                    lv_dropdown_set_options(obj, _("Client\nClient Mute\n-- deprecated --\nRouter\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker"));
                    add_style_drop_down_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj4 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 192);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsBrightnessPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_brightness_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(25));
            lv_obj_set_size(obj, 220, 120);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SettingsBrightnessLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.settings_brightness_label = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                    lv_label_set_text(obj, _("Brightness: 60%"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // BrightnessSlider
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.brightness_slider = obj;
                    lv_obj_set_pos(obj, LV_PCT(10), 25);
                    lv_obj_set_size(obj, LV_PCT(80), 10);
                    lv_slider_set_range(obj, 1, 100);
                    lv_slider_set_value(obj, 60, LV_ANIM_OFF);
                    lv_obj_set_style_bg_image_src(obj, &img_knob_logo_image, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff000000), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xffa8cacd), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj5 = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 196);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsThemePanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_theme_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(25));
            lv_obj_set_size(obj, 220, 120);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SettingsThemeDropdown
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_theme_dropdown = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 150, 30);
                    lv_dropdown_set_options(obj, _("Light\nDark"));
                    add_style_drop_down_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj6 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 200);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsScreenTimeoutPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_screen_timeout_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(25));
            lv_obj_set_size(obj, 220, 120);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SettingsScreenTimeoutLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.settings_screen_timeout_label = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                    lv_label_set_text(obj, _("Timeout: 60s"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // ScreenTimeoutSlider
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.screen_timeout_slider = obj;
                    lv_obj_set_pos(obj, LV_PCT(10), 25);
                    lv_obj_set_size(obj, LV_PCT(80), 10);
                    lv_slider_set_range(obj, 0, 900);
                    lv_slider_set_value(obj, 60, LV_ANIM_OFF);
                    lv_obj_set_style_bg_image_src(obj, &img_knob_logo_image, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff000000), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xffa8cacd), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj7 = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 204);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsScreenLockPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_screen_lock_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(25));
            lv_obj_set_size(obj, 220, 138);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 5);
                    lv_obj_set_size(obj, 125, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, _("Screen Lock"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsScreenLockSwitch
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.settings_screen_lock_switch = obj;
                    lv_obj_set_pos(obj, 5, 0);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb4b4b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_PRESSED);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 38);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, _("Lock PIN"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsScreenLockPasswordTextarea
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.settings_screen_lock_password_textarea = obj;
                    lv_obj_set_pos(obj, -30, 35);
                    lv_obj_set_size(obj, LV_PCT(40), 25);
                    lv_textarea_set_accepted_chars(obj, "1234567890");
                    lv_textarea_set_max_length(obj, 6);
                    lv_textarea_set_placeholder_text(obj, "123456");
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // KeyboardButton_7
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.keyboard_button_7 = obj;
                    lv_obj_set_pos(obj, 2, 35);
                    lv_obj_set_size(obj, 25, 24);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_keyboard_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj8 = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 208);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsInputControlPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_input_control_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(5), LV_PCT(18));
            lv_obj_set_size(obj, 220, 160);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, _("Mouse"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsMouseInputDropdown
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_mouse_input_dropdown = obj;
                    lv_obj_set_pos(obj, 50, -5);
                    lv_obj_set_size(obj, 100, 30);
                    lv_dropdown_set_options(obj, "none");
                    add_style_drop_down_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 48);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, _("Keyboard"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsKeyboardInputDropdown
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_keyboard_input_dropdown = obj;
                    lv_obj_set_pos(obj, 50, 40);
                    lv_obj_set_size(obj, 100, 30);
                    lv_dropdown_set_options(obj, "none");
                    add_style_drop_down_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj9 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 212);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsAlertBuzzerPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_alert_buzzer_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(25));
            lv_obj_set_size(obj, 220, 138);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 5);
                    lv_obj_set_size(obj, 125, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, _("Message Alert"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsAlertBuzzerSwitch
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.settings_alert_buzzer_switch = obj;
                    lv_obj_set_pos(obj, 150, 0);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_add_state(obj, LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb4b4b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_PRESSED);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 35);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, _("Ringtone"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsRingtoneDropdown
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_ringtone_dropdown = obj;
                    lv_obj_set_pos(obj, 40, 30);
                    lv_obj_set_size(obj, 130, 30);
                    lv_dropdown_set_options(obj, "Default");
                    add_style_drop_down_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj10 = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 164);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsLanguagePanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_language_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(25));
            lv_obj_set_size(obj, 220, 120);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SettingsLanguageDropdown
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_language_dropdown = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 150, 30);
                    lv_dropdown_set_options(obj, "English\nDeutsch\nEspañol\nFrançais\nItaliano\nPortuguês");
                    add_style_drop_down_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj11 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 168);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsTimezone
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_timezone = obj;
            lv_obj_set_pos(obj, LV_PCT(5), LV_PCT(18));
            lv_obj_set_size(obj, 220, 160);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, _("Zone"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsTZzone
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_tzzone = obj;
                    lv_obj_set_pos(obj, 5, -2);
                    lv_obj_set_size(obj, 150, 30);
                    lv_dropdown_set_options(obj, "Europe");
                    add_style_drop_down_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 48);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, _("City"));
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsTZcity
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_tzcity = obj;
                    lv_obj_set_pos(obj, 5, 40);
                    lv_obj_set_size(obj, 150, 30);
                    lv_dropdown_set_options(obj, "London\t");
                    add_style_drop_down_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj12 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 172);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsResetPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_reset_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(25));
            lv_obj_set_size(obj, 220, 120);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SettingsResetDropdown
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_reset_dropdown = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 150, 30);
                    lv_dropdown_set_options(obj, _("NodeDB Reset\nFactory Reset"));
                    add_style_drop_down_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj13 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 176);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsModifyChannelPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_modify_channel_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(5), LV_PCT(15));
            lv_obj_set_size(obj, 260, 165);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, _("Channel Name"));
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff7ff5f9), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DISABLED);
                }
                {
                    // SettingsModifyChannelNameTextarea
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.settings_modify_channel_name_textarea = obj;
                    lv_obj_set_pos(obj, 0, 18);
                    lv_obj_set_size(obj, LV_PCT(90), 25);
                    lv_textarea_set_max_length(obj, 11);
                    lv_textarea_set_placeholder_text(obj, "LongFast");
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // KeyboardButton_3
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.keyboard_button_3 = obj;
                    lv_obj_set_pos(obj, 224, 18);
                    lv_obj_set_size(obj, 25, 24);
                    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_keyboard_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 60);
                    lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, _("Pre-shared Key"));
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff7ff5f9), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsModifyChannelPSKTextarea
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.settings_modify_channel_psk_textarea = obj;
                    lv_obj_set_pos(obj, 0, 80);
                    lv_obj_set_size(obj, LV_PCT(90), 25);
                    lv_textarea_set_accepted_chars(obj, "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+=/");
                    lv_textarea_set_max_length(obj, 44);
                    lv_textarea_set_placeholder_text(obj, "AQ==");
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // KeyboardButton_4
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.keyboard_button_4 = obj;
                    lv_obj_set_pos(obj, 224, 80);
                    lv_obj_set_size(obj, 25, 24);
                    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_keyboard_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsModifyTrashButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_modify_trash_button = obj;
                    lv_obj_set_pos(obj, 224, 125);
                    lv_obj_set_size(obj, 24, 24);
                    lv_obj_set_style_bg_image_src(obj, &img_settings_trash_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff63d2f4), LV_PART_MAIN | LV_STATE_PRESSED);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj14 = obj;
                    lv_obj_set_pos(obj, 4, 14);
                    lv_obj_set_size(obj, 160, 50);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    create_user_widget_ok_cancel_widget(obj, 180);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // NodeOptionsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.node_options_panel = obj;
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SNAPPABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO);
            add_style_settings_panel_style(obj);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // NodeOptionsTabView
                    lv_obj_t *obj = lv_tabview_create(parent_obj);
                    lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
                    lv_tabview_set_tab_bar_size(obj, 25);
                    objects.node_options_tab_view = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(105), LV_PCT(110));
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO);
                    add_style_tab_view_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // TabPageFilter
                            lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Filter");
                            objects.tab_page_filter = obj;
                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_row(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // NodesFilterUnknownLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_filter_unknown_label = obj;
                                    lv_obj_set_pos(obj, 10, 0);
                                    lv_obj_set_size(obj, LV_PCT(98), 30);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_label_set_text(obj, _("Unknown"));
                                    add_style_settings_label_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // NodesFilterUnknownSwitch
                                            lv_obj_t *obj = lv_switch_create(parent_obj);
                                            objects.nodes_filter_unknown_switch = obj;
                                            lv_obj_set_pos(obj, -10, -3);
                                            lv_obj_set_size(obj, 50, 20);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb4b4b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8ff498), LV_PART_MAIN | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_PRESSED);
                                        }
                                    }
                                }
                                {
                                    // NodesFilterOfflineLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_filter_offline_label = obj;
                                    lv_obj_set_pos(obj, 10, 0);
                                    lv_obj_set_size(obj, LV_PCT(98), 30);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_label_set_text(obj, _("Offline"));
                                    add_style_settings_label_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // NodesFilterOfflineSwitch
                                            lv_obj_t *obj = lv_switch_create(parent_obj);
                                            objects.nodes_filter_offline_switch = obj;
                                            lv_obj_set_pos(obj, -10, -3);
                                            lv_obj_set_size(obj, 50, 20);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb4b4b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8ff498), LV_PART_MAIN | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_PRESSED);
                                        }
                                    }
                                }
                                {
                                    // NodesFilterMQTTLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_filter_mqtt_label = obj;
                                    lv_obj_set_pos(obj, 10, 0);
                                    lv_obj_set_size(obj, LV_PCT(98), 30);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_label_set_text(obj, _("MQTT"));
                                    add_style_settings_label_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // NodesFilterMQTTSwitch
                                            lv_obj_t *obj = lv_switch_create(parent_obj);
                                            objects.nodes_filter_mqtt_switch = obj;
                                            lv_obj_set_pos(obj, -10, -3);
                                            lv_obj_set_size(obj, 50, 20);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb4b4b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8ff498), LV_PART_MAIN | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_PRESSED);
                                        }
                                    }
                                }
                                {
                                    // NodesFilterPositionLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_filter_position_label = obj;
                                    lv_obj_set_pos(obj, 10, 0);
                                    lv_obj_set_size(obj, LV_PCT(98), 30);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_label_set_text(obj, _("Position"));
                                    add_style_settings_label_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // NodesFilterPositionSwitch
                                            lv_obj_t *obj = lv_switch_create(parent_obj);
                                            objects.nodes_filter_position_switch = obj;
                                            lv_obj_set_pos(obj, -10, -3);
                                            lv_obj_set_size(obj, 50, 20);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb4b4b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8ff498), LV_PART_MAIN | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_PRESSED);
                                        }
                                    }
                                }
                                {
                                    // NodesFilterNameLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_filter_name_label = obj;
                                    lv_obj_set_pos(obj, 10, 0);
                                    lv_obj_set_size(obj, LV_PCT(98), 30);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_label_set_text(obj, _("Name"));
                                    add_style_settings_label_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // NodesFilterNameArea
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.nodes_filter_name_area = obj;
                                            lv_obj_set_pos(obj, -40, -2);
                                            lv_obj_set_size(obj, LV_PCT(60), 24);
                                            lv_textarea_set_max_length(obj, 237);
                                            lv_textarea_set_placeholder_text(obj, "!Enter Filter ...");
                                            lv_textarea_set_one_line(obj, true);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                            lv_obj_set_scroll_dir(obj, LV_DIR_HOR);
                                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_max_height(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // KeyboardButton_5
                                            lv_obj_t *obj = lv_btn_create(parent_obj);
                                            objects.keyboard_button_5 = obj;
                                            lv_obj_set_pos(obj, -10, -2);
                                            lv_obj_set_size(obj, 25, 24);
                                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_image_src(obj, &img_keyboard_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // TabPageHighlight
                            lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Highlight");
                            objects.tab_page_highlight = obj;
                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_row(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // NodesHLActiveChatLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_hl_active_chat_label = obj;
                                    lv_obj_set_pos(obj, 10, 0);
                                    lv_obj_set_size(obj, LV_PCT(98), 30);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_label_set_text(obj, _("Active Chat"));
                                    add_style_settings_label_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // NodesHLActiveChatSwitch
                                            lv_obj_t *obj = lv_switch_create(parent_obj);
                                            objects.nodes_hl_active_chat_switch = obj;
                                            lv_obj_set_pos(obj, -10, -3);
                                            lv_obj_set_size(obj, 50, 20);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb4b4b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8ff498), LV_PART_MAIN | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_PRESSED);
                                        }
                                    }
                                }
                                {
                                    // NodesHLPositionLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_hl_position_label = obj;
                                    lv_obj_set_pos(obj, 10, -1);
                                    lv_obj_set_size(obj, LV_PCT(98), 30);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_label_set_text(obj, _("Position"));
                                    add_style_settings_label_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // NodesHLPositionSwitch
                                            lv_obj_t *obj = lv_switch_create(parent_obj);
                                            objects.nodes_hl_position_switch = obj;
                                            lv_obj_set_pos(obj, -10, -3);
                                            lv_obj_set_size(obj, 50, 20);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb4b4b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8ff498), LV_PART_MAIN | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_PRESSED);
                                        }
                                    }
                                }
                                {
                                    // NodesHLTelemetryLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_hl_telemetry_label = obj;
                                    lv_obj_set_pos(obj, 10, 0);
                                    lv_obj_set_size(obj, LV_PCT(98), 30);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_label_set_text(obj, _("Telemetry"));
                                    add_style_settings_label_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // NodesHLTelemetrySwitch
                                            lv_obj_t *obj = lv_switch_create(parent_obj);
                                            objects.nodes_hl_telemetry_switch = obj;
                                            lv_obj_set_pos(obj, -10, -3);
                                            lv_obj_set_size(obj, 50, 20);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb4b4b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8ff498), LV_PART_MAIN | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_PRESSED);
                                        }
                                    }
                                }
                                {
                                    // NodesHLIAQLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_hliaq_label = obj;
                                    lv_obj_set_pos(obj, 10, 0);
                                    lv_obj_set_size(obj, LV_PCT(98), 30);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_label_set_text(obj, _("IAQ"));
                                    add_style_settings_label_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // NodesHLIAQSwitch
                                            lv_obj_t *obj = lv_switch_create(parent_obj);
                                            objects.nodes_hliaq_switch = obj;
                                            lv_obj_set_pos(obj, -10, -3);
                                            lv_obj_set_size(obj, 50, 20);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb4b4b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff8ff498), LV_PART_MAIN | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_CHECKED);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_PRESSED);
                                        }
                                    }
                                }
                                {
                                    // NodesHLNameLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_hl_name_label = obj;
                                    lv_obj_set_pos(obj, 10, 0);
                                    lv_obj_set_size(obj, LV_PCT(98), 30);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_label_set_text(obj, _("Name"));
                                    add_style_settings_label_style(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // NodesHLNameArea
                                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                                            objects.nodes_hl_name_area = obj;
                                            lv_obj_set_pos(obj, -40, -2);
                                            lv_obj_set_size(obj, LV_PCT(60), 24);
                                            lv_textarea_set_max_length(obj, 237);
                                            lv_textarea_set_placeholder_text(obj, "Enter Filter ...");
                                            lv_textarea_set_one_line(obj, true);
                                            lv_textarea_set_password_mode(obj, false);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                                            lv_obj_set_scroll_dir(obj, LV_DIR_HOR);
                                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_max_height(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // KeyboardButton_6
                                            lv_obj_t *obj = lv_btn_create(parent_obj);
                                            objects.keyboard_button_6 = obj;
                                            lv_obj_set_pos(obj, -10, -2);
                                            lv_obj_set_size(obj, 25, 24);
                                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_image_src(obj, &img_keyboard_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        {
            // MeshDetectorPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.mesh_detector_panel = obj;
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
            add_style_panel_style(obj);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // DetectorRadarPanel
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.detector_radar_panel = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 202, 202);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
                    add_style_panel_style(obj);
                    lv_obj_set_style_bg_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_x(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_spread(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // radarBeam
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.radar_beam = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_img_set_src(obj, &img_radar_beam_image);
                            lv_img_set_pivot(obj, 100, 100);
                            lv_img_set_angle(obj, 177);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor_opa(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_line_create(parent_obj);
                            static lv_point_precise_t line_points[] = {
                                { 0, 0 },
                                { 0, 200 }
                            };
                            lv_line_set_points(obj, line_points, 2);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_line_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_line_create(parent_obj);
                            static lv_point_precise_t line_points[] = {
                                { -200, 0 },
                                { 200, 0 }
                            };
                            lv_line_set_points(obj, line_points, 2);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_line_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 200, 200);
                            lv_obj_set_style_radius(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 140, 140);
                            lv_obj_set_style_radius(obj, 140, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 80, 80);
                            lv_obj_set_style_radius(obj, 80, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // DetectorStartButtonPanel
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.detector_start_button_panel = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 64, 64);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    add_style_panel_style(obj);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // DetectorStartButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.detector_start_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 50, 50);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3ad821), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff063d0a), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_outline_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_outline_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // DetectorStartLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.detector_start_label = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_text(obj, _("Start"));
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                            }
                        }
                    }
                }
                {
                    // DetectedNodeButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.detected_node_button = obj;
                    lv_obj_set_pos(obj, 0, 120);
                    lv_obj_set_size(obj, LV_PCT(100), 38);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    add_style_settings_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // DetectedNodeImage
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.detected_node_image = obj;
                            lv_obj_set_pos(obj, -5, -6);
                            lv_obj_set_size(obj, 32, 32);
                            lv_img_set_src(obj, &img_node_client_image);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff5d9388), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff5d9388), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // DetectedNodeLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.detected_node_label = obj;
                            lv_obj_set_pos(obj, 40, 0);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, "");
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
            }
        }
        {
            // SignalScannerPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.signal_scanner_panel = obj;
            lv_obj_set_pos(obj, 17, 12);
            lv_obj_set_size(obj, LV_PCT(85), 210);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
            add_style_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 480, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 360, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_x(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_spread(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_color(obj, lv_color_hex(0xff25c75f), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SignalScannerNodeButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.signal_scanner_node_button = obj;
                    lv_obj_set_pos(obj, 0, -5);
                    lv_obj_set_size(obj, LV_PCT(45), 40);
                    add_style_home_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // SignalScannerNodeImage
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.signal_scanner_node_image = obj;
                            lv_obj_set_pos(obj, -5, -6);
                            lv_obj_set_size(obj, 32, 32);
                            lv_img_set_src(obj, &img_node_client_image);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff5d9388), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff5d9388), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // SignalScannerNodeButtonLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.signal_scanner_node_button_label = obj;
                            lv_obj_set_pos(obj, 35, 0);
                            lv_obj_set_size(obj, LV_PCT(75), LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Choose Node");
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // SignalScannerStartButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.signal_scanner_start_button = obj;
                    lv_obj_set_pos(obj, 0, -50);
                    lv_obj_set_size(obj, 50, 50);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3ad821), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff063d0a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_outline_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_outline_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // SignalScannerStartLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.signal_scanner_start_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Start");
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // SnrSlider
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.snr_slider = obj;
                    lv_obj_set_pos(obj, 40, 0);
                    lv_obj_set_size(obj, 20, 200);
                    lv_slider_set_range(obj, -20, 9);
                    lv_slider_set_value(obj, -10, LV_ANIM_OFF);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_PRESS_LOCK);
                    lv_obj_set_style_bg_image_src(obj, &img_signal_slider_image, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_outline_width(obj, 1, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 80, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // RssiSlider
                    lv_obj_t *obj = lv_slider_create(parent_obj);
                    objects.rssi_slider = obj;
                    lv_obj_set_pos(obj, -40, -1);
                    lv_obj_set_size(obj, 20, 200);
                    lv_slider_set_range(obj, -125, -20);
                    lv_slider_set_value(obj, -100, LV_ANIM_OFF);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_PRESS_LOCK);
                    lv_obj_set_style_bg_image_src(obj, &img_signal_slider_image, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_outline_width(obj, 1, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 80, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SignalScannerSnrLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.signal_scanner_snr_label = obj;
                    lv_obj_set_pos(obj, -35, 20);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "SNR\n-10.0");
                    add_style_color_label_style(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SignalScannerRssiLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.signal_scanner_rssi_label = obj;
                    lv_obj_set_pos(obj, 35, 20);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "RSSI\n-100");
                    add_style_color_label_style(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SignalScannerSnrScaleLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.signal_scanner_snr_scale_label = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 38, 200);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, "8.0\n6.0\n4.0\n2.0\n0.0\n-2.0\n-4.0\n-8.0\n-10.0\n-12.0\n-14.0\n-16.0\n-18.0");
                    lv_obj_set_style_text_font(obj, &ui_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SignalScannerRssiScaleLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.signal_scanner_rssi_scale_label = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 38, 202);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, "-20\n-30\n-40\n-50\n-60\n-70\n-80\n-90\n-100\n-110\n-120");
                    lv_obj_set_style_text_font(obj, &ui_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_line_space(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // TraceRoutePanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.trace_route_panel = obj;
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
            add_style_panel_style(obj);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TraceRouteToButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.trace_route_to_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(100), 38);
                    add_style_settings_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // TraceRouteToImage
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.trace_route_to_image = obj;
                            lv_obj_set_pos(obj, -5, -6);
                            lv_obj_set_size(obj, 32, 32);
                            lv_img_set_src(obj, &img_node_client_image);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff5d9388), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff5d9388), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // TraceRouteToButtonLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.trace_route_to_button_label = obj;
                            lv_obj_set_pos(obj, 40, 0);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, "choose target node");
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // StartButtonPanel
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.start_button_panel = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 70, 70);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    add_style_panel_style(obj);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // TraceRouteStartButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.trace_route_start_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 50, 50);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3ad821), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff063d0a), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_outline_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_outline_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // TraceRouteStartLabel
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.trace_route_start_label = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_text(obj, _("Start"));
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                            }
                        }
                    }
                }
            }
        }
        {
            // ToolsNeighborsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.tools_neighbors_panel = obj;
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
            add_style_panel_style(obj);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // ToolsStatisticsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.tools_statistics_panel = obj;
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
            add_style_panel_style(obj);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // StatisticsLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.statistics_label = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                }
            }
        }
        {
            // ToolsDebugLogPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.tools_debug_log_panel = obj;
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
            add_style_panel_style(obj);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // Keyboard
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.keyboard = obj;
            lv_obj_set_pos(obj, 1, 28);
            lv_obj_set_size(obj, LV_PCT(100), 120);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff1b43db), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffccd1d8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 400, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // MsgPopupPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.msg_popup_panel = obj;
            lv_obj_set_pos(obj, 18, -20);
            lv_obj_set_size(obj, LV_PCT(85), 55);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_alert_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff8c04), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_color(obj, lv_color_hex(0xffbb6c04), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_spread(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_x(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // MsgPopupButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.msg_popup_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 36, 36);
                    add_style_home_button_style(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_msg_popup_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // MsgPopupLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.msg_popup_label = obj;
                    lv_obj_set_pos(obj, 10, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, _("New Message from\n"));
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // AlertPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.alert_panel = obj;
            lv_obj_set_pos(obj, 18, -20);
            lv_obj_set_size(obj, LV_PCT(85), 55);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_alert_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0024), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_color(obj, lv_color_hex(0xffa10000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_spread(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_x(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // AlertLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alert_label = obj;
                    lv_obj_set_pos(obj, 10, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_label_set_text(obj, _("Resynch..."));
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
}

void tick_screen_main_screen() {
    tick_user_widget_ok_cancel_widget(156);
    tick_user_widget_ok_cancel_widget(160);
    tick_user_widget_ok_cancel_widget(184);
    tick_user_widget_ok_cancel_widget(188);
    tick_user_widget_ok_cancel_widget(192);
    tick_user_widget_ok_cancel_widget(196);
    tick_user_widget_ok_cancel_widget(200);
    tick_user_widget_ok_cancel_widget(204);
    tick_user_widget_ok_cancel_widget(208);
    tick_user_widget_ok_cancel_widget(212);
    tick_user_widget_ok_cancel_widget(164);
    tick_user_widget_ok_cancel_widget(168);
    tick_user_widget_ok_cancel_widget(172);
    tick_user_widget_ok_cancel_widget(176);
    tick_user_widget_ok_cancel_widget(180);
}

void create_screen_blank_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.blank_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
}

void tick_screen_blank_screen() {
}

void create_screen_lock_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.lock_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    add_style_main_screen_style(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ScreenLockButtonMatrix
            lv_obj_t *obj = lv_buttonmatrix_create(parent_obj);
            objects.screen_lock_button_matrix = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 220, 220);
            static const char *map[16] = {
                "1",
                "2",
                "3",
                "\n",
                "4",
                "5",
                "6",
                "\n",
                "7",
                "8",
                "9",
                "\n",
                "X",
                "0",
                "Del",
                NULL,
            };
            static lv_buttonmatrix_ctrl_t ctrl_map[12] = {
                1,
                1,
                1,
                1,
                1,
                1,
                1,
                1,
                1,
                1,
                2,
                1,
            };
            lv_buttonmatrix_set_map(obj, map);
            lv_buttonmatrix_set_ctrl_map(obj, ctrl_map);
            add_style_button_matrix_style(obj);
            lv_obj_set_style_transform_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_ITEMS | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_ITEMS | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 20, LV_PART_ITEMS | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff29cf29), LV_PART_ITEMS | LV_STATE_PRESSED);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff4a62f5), LV_PART_ITEMS | LV_STATE_PRESSED);
        }
        {
            // LockScreenDigitsLabel
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lock_screen_digits_label = obj;
            lv_obj_set_pos(obj, 0, -5);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            lv_label_set_text(obj, "o o o o o o");
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void tick_screen_lock_screen() {
}

void create_screen_calibration_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.calibration_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
}

void tick_screen_calibration_screen() {
}

void create_user_widget_ok_cancel_widget(lv_obj_t *parent_obj, int startWidgetIndex) {
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            // OkCancelPanelW
            lv_obj_t *obj = lv_obj_create(parent_obj);
            ((lv_obj_t **)&objects)[startWidgetIndex + 2] = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 160, 50);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_panel_style(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // OkButtonW
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_clip_corner(obj, true, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "OK");
                        }
                    }
                }
                {
                    // CancelButtonW
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_clip_corner(obj, true, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Cancel");
                        }
                    }
                }
            }
        }
    }
}

void tick_user_widget_ok_cancel_widget(int startWidgetIndex) {
}


void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_boot_screen();
    create_screen_main_screen();
    create_screen_blank_screen();
    create_screen_lock_screen();
    create_screen_calibration_screen();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_boot_screen,
    tick_screen_main_screen,
    tick_screen_blank_screen,
    tick_screen_lock_screen,
    tick_screen_calibration_screen,
    0,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
