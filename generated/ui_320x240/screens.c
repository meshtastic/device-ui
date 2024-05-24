#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

objects_t objects;
lv_obj_t *tick_value_change_obj;

void create_screen_boot_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.boot_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff3dda83), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
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
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_min_width(obj, 320, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_max_width(obj, 640, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_min_height(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_max_height(obj, 480, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ButtonPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.button_panel = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_PCT(12), LV_PCT(100));
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN);
            lv_obj_set_style_max_width(obj, 38, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 536870911, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff585858), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff585858), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_home_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // NodesButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.nodes_button = obj;
                    lv_obj_set_pos(obj, -12, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff585858), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_nodes_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // GroupsButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.groups_button = obj;
                    lv_obj_set_pos(obj, -12, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff585858), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_groups_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // MessagesButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.messages_button = obj;
                    lv_obj_set_pos(obj, -12, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff585858), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_messages_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // MapButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.map_button = obj;
                    lv_obj_set_pos(obj, -12, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff585858), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_map_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_button = obj;
                    lv_obj_set_pos(obj, -12, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff585858), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_settings_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // HomePanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.home_panel = obj;
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // HomeContainer
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.home_container = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(95), LV_PCT(100));
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM);
                    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_x(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_spread(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_color(obj, lv_color_hex(0xff2b824a), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW_WRAP, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_clip_corner(obj, false, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_grow(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // HomeMailButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_mail_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "no new messages");
                        }
                        {
                            // HomeNodesButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_nodes_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "1 of 1 nodes online");
                        }
                        {
                            // HomeLocationButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_location_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                        }
                        {
                            // HomeWLANButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_wlan_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_src(obj, &img_home_wlan_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "0.0.0.0");
                        }
                        {
                            // HomeBluetoothButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_bluetooth_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_src(obj, &img_home_bluetooth_on_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_image_recolor_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "00:00:00:00:00:00");
                        }
                        {
                            // HomeMemoryButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_memory_button = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 36, 36);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_src(obj, &img_home_memory_button, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                        }
                    }
                }
            }
        }
        {
            // NodesPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.nodes_panel = obj;
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_grow(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // NodePanel
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.node_panel = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), 0);
                    lv_obj_set_size(obj, LV_PCT(100), 50);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff808080), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // NodeImage
                            lv_obj_t *obj = lv_img_create(parent_obj);
                            objects.node_image = obj;
                            lv_obj_set_pos(obj, -5, -10);
                            lv_obj_set_size(obj, 32, 32);
                            lv_img_set_src(obj, &img_node_client_image);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
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
                            lv_obj_set_pos(obj, -5, 0);
                            lv_obj_set_size(obj, LV_PCT(50), 50);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_shadow_ofs_y(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // UserNameLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.user_name_label = obj;
                            lv_obj_set_pos(obj, -5, 15);
                            lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
                            lv_label_set_text(obj, " ");
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // UserNameShortLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.user_name_short_label = obj;
                            lv_obj_set_pos(obj, 30, -8);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, " ");
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // BatteryLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.battery_label = obj;
                            lv_obj_set_pos(obj, 8, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, " ");
                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // LastHeardLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.last_heard_label = obj;
                            lv_obj_set_pos(obj, 8, 15);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, " ");
                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // SignalLabel
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.signal_label = obj;
                            lv_obj_set_pos(obj, 8, -18);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_label_set_text(obj, " ");
                            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_grow(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ChannelButton0
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.channel_button0 = obj;
                    lv_obj_set_pos(obj, 122, 39);
                    lv_obj_set_size(obj, LV_PCT(30), 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffa0a0a0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffa0a0a0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffa0a0a0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffa0a0a0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffa0a0a0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffa0a0a0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffa0a0a0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffa0a0a0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // MessagesContainer
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.messages_container = obj;
                    lv_obj_set_pos(obj, LV_PCT(-5), LV_PCT(-7));
                    lv_obj_set_size(obj, LV_PCT(110), LV_PCT(100));
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // MessageInputArea
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.message_input_area = obj;
                    lv_obj_set_pos(obj, 0, 12);
                    lv_obj_set_size(obj, LV_PCT(90), 24);
                    lv_textarea_set_max_length(obj, 237);
                    lv_textarea_set_placeholder_text(obj, "Enter Text ...");
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_height(obj, 24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // KeyboardButton_0
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.keyboard_button_0 = obj;
                    lv_obj_set_pos(obj, 5, 12);
                    lv_obj_set_size(obj, 25, 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "ABCD: Username");
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // ChatDelButton
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.chat_del_button = obj;
                            lv_obj_set_pos(obj, -3, -1);
                            lv_obj_set_size(obj, 40, 23);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
                                    lv_label_set_text(obj, "DEL");
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
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_image_src(obj, &img_worldmap_image, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // BasicSettingsPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.basic_settings_panel = obj;
            lv_obj_set_pos(obj, 39, 25);
            lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // BasicSettingsUserButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.basic_settings_user_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(97), 30);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff505050), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "User: ");
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                        }
                    }
                }
                {
                    // BasicSettingsRegionButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.basic_settings_region_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(97), 30);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff505050), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    // BasicSettingsChannelButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.basic_settings_channel_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(97), 30);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff505050), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "Channel: LongFast");
                        }
                    }
                }
                {
                    // BasicSettingsRoleButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.basic_settings_role_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(97), 30);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff505050), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "Role: Client");
                        }
                    }
                }
                {
                    // BasicSettingsTimeoutButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.basic_settings_timeout_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(97), 30);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff505050), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "Screen Timeout: 60");
                        }
                    }
                }
                {
                    // BasicSettingsBrightnessButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.basic_settings_brightness_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(97), 30);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff505050), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "Screen Brightness: 50%");
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DISABLED);
                        }
                    }
                }
                {
                    // BasicSettingsAlertButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.basic_settings_alert_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(97), 30);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff505050), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "Message Alert Buzzer: on");
                        }
                    }
                }
                {
                    // BasicSettingsLanguageButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.basic_settings_language_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_PCT(97), 30);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff505050), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "Language: English");
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
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // MeshtasticLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.meshtastic_label = obj;
                    lv_obj_set_pos(obj, 73, -12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Meshtastic");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // MeshtasticImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.meshtastic_image = obj;
                    lv_obj_set_pos(obj, 37, -9);
                    lv_obj_set_size(obj, 36, 17);
                    lv_img_set_src(obj, &img_meshtastic_logo_image);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_image_opa(obj, 255, LV_PART_MAIN | LV_STATE_DISABLED);
                    lv_obj_set_style_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DISABLED);
                    lv_obj_set_style_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DISABLED);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopNodesOnlineLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_nodes_online_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "1 of 1 nodes online");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopNodesImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_nodes_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_image_src(obj, &img_top_nodes_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopUserGroupLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_user_group_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Group Channels");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopGroupImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_group_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_image_src(obj, &img_top_group_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopGroupChatLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_group_chat_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "no messages");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopGroupChatImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_group_chat_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_image_src(obj, &img_top_group_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopBasicSettingsLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_basic_settings_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Settings (basic)");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopBasicSettingsImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_basic_settings_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_image_src(obj, &img_top_settings_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopAdvancedSettingsLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_advanced_settings_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Settings (advanced)");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopAdvancedSettingsImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_advanced_settings_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_image_src(obj, &img_top_settings_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopMapLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_map_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Locations Map");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopMapImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_map_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_image_src(obj, &img_top_map_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopChatsLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_chats_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "no chats");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopChatsImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_chats_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_image_src(obj, &img_top_chats_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TopMessagesNodeLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.top_messages_node_label = obj;
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "no messages");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // TopMessagesNodeImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.top_messages_node_image = obj;
                    lv_obj_set_pos(obj, -5, 0);
                    lv_obj_set_size(obj, 24, 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_image_src(obj, &img_top_message_node_images, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff436c70), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // BatteryImage
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.battery_image = obj;
                    lv_obj_set_pos(obj, 13, 0);
                    lv_obj_set_size(obj, 24, 24);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_image_src(obj, &img_battery_full_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // BatteryPercentageLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.battery_percentage_label = obj;
                    lv_obj_set_pos(obj, -10, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, " ");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_label_set_text(obj, "Short Name");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
                    lv_label_set_text(obj, "Long Name");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
                    lv_style_value_t value;
                    value.num = 0;
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, value, LV_PART_MAIN);
                    create_user_widget_ok_cancel_widget(obj, 94);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_margin_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_label_set_text(obj, "Primary Channel");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "<unset>");
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsChannel1Button
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.settings_channel1_button = obj;
                    lv_obj_set_pos(obj, 0, 18);
                    lv_obj_set_size(obj, LV_PCT(100), 25);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "<unset>");
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "<unset>");
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "<unset>");
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "<unset>");
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "<unset>");
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_label_set_text(obj, "<unset>");
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DISABLED);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj1 = obj;
                    lv_obj_set_pos(obj, 4, 14);
                    lv_obj_set_size(obj, 160, 50);
                    lv_style_value_t value;
                    value.num = 0;
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, value, LV_PART_MAIN);
                    create_user_widget_ok_cancel_widget(obj, 98);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj2 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 160, 50);
                    lv_style_value_t value;
                    value.num = 0;
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, value, LV_PART_MAIN);
                    create_user_widget_ok_cancel_widget(obj, 102);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // SettingsDeviceRoleDropdown
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.settings_device_role_dropdown = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 150, 30);
                    lv_dropdown_set_options(obj, "Client\nClient Mute\nRouter\nRouter Client\nRepeater\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker");
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj3 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 160, 50);
                    lv_style_value_t value;
                    value.num = 0;
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, value, LV_PART_MAIN);
                    create_user_widget_ok_cancel_widget(obj, 106);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_label_set_text(obj, "Brightness: 60%");
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
                    objects.obj4 = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, 160, 50);
                    lv_style_value_t value;
                    value.num = 0;
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, value, LV_PART_MAIN);
                    create_user_widget_ok_cancel_widget(obj, 110);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_label_set_text(obj, "Timeout: 60s");
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
                    objects.obj5 = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, 160, 50);
                    lv_style_value_t value;
                    value.num = 0;
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, value, LV_PART_MAIN);
                    create_user_widget_ok_cancel_widget(obj, 114);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // SettingsAlertBuzzerPanel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_alert_buzzer_panel = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(25));
            lv_obj_set_size(obj, 220, 120);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, 125, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Buzzer Alert");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsAlertBuzzerSwitch
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.settings_alert_buzzer_switch = obj;
                    lv_obj_set_pos(obj, 132, 5);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb4b4b4), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xff606060), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_INDICATOR | LV_STATE_PRESSED);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj6 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 160, 50);
                    lv_style_value_t value;
                    value.num = 0;
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, value, LV_PART_MAIN);
                    create_user_widget_ok_cancel_widget(obj, 118);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe0e0e0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj7 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 160, 50);
                    lv_style_value_t value;
                    value.num = 0;
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, value, LV_PART_MAIN);
                    create_user_widget_ok_cancel_widget(obj, 122);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff216ad8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_label_set_text(obj, "Channel Name");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_textarea_set_max_length(obj, 12);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
                    lv_label_set_text(obj, "Pre-shared Key");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // SettingsModifyChannelPSKTextarea
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.settings_modify_channel_psk_textarea = obj;
                    lv_obj_set_pos(obj, 0, 80);
                    lv_obj_set_size(obj, LV_PCT(90), 25);
                    lv_textarea_set_max_length(obj, 44);
                    lv_textarea_set_placeholder_text(obj, "AQ==");
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(obj, &img_keyboard_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(0xff373737), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_recolor_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj8 = obj;
                    lv_obj_set_pos(obj, 4, 14);
                    lv_obj_set_size(obj, 160, 50);
                    lv_style_value_t value;
                    value.num = 0;
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, value, LV_PART_MAIN);
                    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, value, LV_PART_MAIN);
                    create_user_widget_ok_cancel_widget(obj, 126);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // Keyboard
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.keyboard = obj;
            lv_obj_set_pos(obj, 18, 28);
            lv_obj_set_size(obj, LV_PCT(88), 110);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff1b43db), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff8c04), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_color(obj, lv_color_hex(0xffbb6c04), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_spread(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_x(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // MsgPopupButton
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.msg_popup_button = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 36, 36);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                    lv_label_set_text(obj, "New Message from\n");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
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
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0024), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_color(obj, lv_color_hex(0xffa10000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_spread(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_y(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_ofs_x(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // AlertLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alert_label = obj;
                    lv_obj_set_pos(obj, 10, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Resynch...");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
}

void tick_screen_main_screen() {
    tick_user_widget_ok_cancel_widget(94);
    tick_user_widget_ok_cancel_widget(98);
    tick_user_widget_ok_cancel_widget(102);
    tick_user_widget_ok_cancel_widget(106);
    tick_user_widget_ok_cancel_widget(110);
    tick_user_widget_ok_cancel_widget(114);
    tick_user_widget_ok_cancel_widget(118);
    tick_user_widget_ok_cancel_widget(122);
    tick_user_widget_ok_cancel_widget(126);
}

void create_screen_blank_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.blank_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
    }
}

void tick_screen_blank_screen() {
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
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // OkButtonW
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
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
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_boot_screen,
    tick_screen_main_screen,
    tick_screen_blank_screen,
    0,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
