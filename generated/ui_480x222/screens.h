#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_WIDGETS = 1,
    SCREEN_ID_BOOT = 2,
    SCREEN_ID_MENU = 3,
    SCREEN_ID_HOME = 4,
    SCREEN_ID_CHATS = 5,
    SCREEN_ID_NODES = 6,
    SCREEN_ID_GROUPS = 7,
    SCREEN_ID_MAP = 8,
    SCREEN_ID_CLOCK = 9,
    SCREEN_ID_SETTINGS = 10,
    SCREEN_ID_BLANK = 11,
    _SCREEN_ID_LAST = 11
};

typedef struct _objects_t {
    lv_obj_t *widgets;
    lv_obj_t *boot;
    lv_obj_t *menu;
    lv_obj_t *home;
    lv_obj_t *chats;
    lv_obj_t *nodes;
    lv_obj_t *groups;
    lv_obj_t *map;
    lv_obj_t *clock;
    lv_obj_t *settings;
    lv_obj_t *blank;
    lv_obj_t *obj0;
    lv_obj_t *obj0__chat_button;
    lv_obj_t *obj0__chat_channel_image;
    lv_obj_t *obj0__chat_id;
    lv_obj_t *obj0__chat_group_name;
    lv_obj_t *obj1;
    lv_obj_t *obj1__add_message_entry_button;
    lv_obj_t *obj1__add_message_entry_time_label;
    lv_obj_t *obj1__add_message_entry_text_label;
    lv_obj_t *obj2;
    lv_obj_t *obj2__new_message_entry_button;
    lv_obj_t *obj2__new_message_time_label;
    lv_obj_t *obj2__new_message_text_label;
    lv_obj_t *poster_meshtastic;
    lv_obj_t *obj3;
    lv_obj_t *main_panel;
    lv_obj_t *home_button;
    lv_obj_t *nodes_button;
    lv_obj_t *groups_button;
    lv_obj_t *messages_button;
    lv_obj_t *map_button;
    lv_obj_t *clock_button;
    lv_obj_t *music_button;
    lv_obj_t *statistics_button;
    lv_obj_t *tools_button;
    lv_obj_t *apps_button;
    lv_obj_t *settings_button;
    lv_obj_t *power_button;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *menu_label;
    lv_obj_t *top_dashboard_panel;
    lv_obj_t *top_home_back_button;
    lv_obj_t *top_home_label;
    lv_obj_t *obj6;
    lv_obj_t *home_mail_button;
    lv_obj_t *home_mail_label;
    lv_obj_t *home_nodes_button;
    lv_obj_t *home_nodes_label;
    lv_obj_t *home_time_button;
    lv_obj_t *home_time_label;
    lv_obj_t *home_lora_button;
    lv_obj_t *home_lora_label;
    lv_obj_t *home_signal_button;
    lv_obj_t *home_signal_label;
    lv_obj_t *home_signal_pct_label;
    lv_obj_t *home_bell_button;
    lv_obj_t *home_bell_label;
    lv_obj_t *home_location_button;
    lv_obj_t *home_location_label;
    lv_obj_t *home_wlan_button;
    lv_obj_t *home_wlan_label;
    lv_obj_t *home_mqtt_button;
    lv_obj_t *home_mqtt_label;
    lv_obj_t *home_sd_card_button;
    lv_obj_t *home_sd_card_label;
    lv_obj_t *home_memory_button;
    lv_obj_t *home_memory_label;
    lv_obj_t *home_qr_button;
    lv_obj_t *home_qr_label;
    lv_obj_t *chat_panel;
    lv_obj_t *message_input_area;
    lv_obj_t *chats_panel;
    lv_obj_t *top_chat_panel;
    lv_obj_t *top_chat_back_button;
    lv_obj_t *top_chat_label;
    lv_obj_t *top_panel;
    lv_obj_t *top_nodes_back_button;
    lv_obj_t *top_nodes_online_label;
    lv_obj_t *nodes_panel;
    lv_obj_t *node_button;
    lv_obj_t *node_short_name;
    lv_obj_t *node_long_name;
    lv_obj_t *top_groups_panel;
    lv_obj_t *top_groups_back_button;
    lv_obj_t *top_groups_label;
    lv_obj_t *groups_panel;
    lv_obj_t *group_button_0;
    lv_obj_t *group_id_0;
    lv_obj_t *group_name_0;
    lv_obj_t *group_button_1;
    lv_obj_t *group_id_1;
    lv_obj_t *group_name_1;
    lv_obj_t *group_button_2;
    lv_obj_t *group_id_2;
    lv_obj_t *group_name_2;
    lv_obj_t *group_button_3;
    lv_obj_t *group_id_3;
    lv_obj_t *group_name_3;
    lv_obj_t *group_button_4;
    lv_obj_t *group_id_4;
    lv_obj_t *group_name_4;
    lv_obj_t *group_button_5;
    lv_obj_t *group_id_5;
    lv_obj_t *group_name_5;
    lv_obj_t *group_button_6;
    lv_obj_t *group_id_6;
    lv_obj_t *group_name_6;
    lv_obj_t *group_button_7;
    lv_obj_t *group_id_7;
    lv_obj_t *group_name_7;
    lv_obj_t *top_map_panel;
    lv_obj_t *top_map_back_button;
    lv_obj_t *top_map_label;
    lv_obj_t *map_panel;
    lv_obj_t *map_location_label;
    lv_obj_t *top_clock_panel;
    lv_obj_t *top_clock_back_button;
    lv_obj_t *top_clock_label;
    lv_obj_t *clock_time_label;
    lv_obj_t *clock_seconds_label;
    lv_obj_t *top_settings_panel;
    lv_obj_t *top_settings_back_button;
    lv_obj_t *top_settings_label;
    lv_obj_t *settings_panel;
    lv_obj_t *settings_roller;
    lv_obj_t *settings_user;
    lv_obj_t *settings_lora;
    lv_obj_t *settings_device;
    lv_obj_t *settings_network;
    lv_obj_t *settings_display;
    lv_obj_t *settings_panel_1;
    lv_obj_t *settings_menu;
} objects_t;

extern objects_t objects;

void create_screen_widgets();
void tick_screen_widgets();

void create_screen_boot();
void tick_screen_boot();

void create_screen_menu();
void tick_screen_menu();

void create_screen_home();
void tick_screen_home();

void create_screen_chats();
void tick_screen_chats();

void create_screen_nodes();
void tick_screen_nodes();

void create_screen_groups();
void tick_screen_groups();

void create_screen_map();
void tick_screen_map();

void create_screen_clock();
void tick_screen_clock();

void create_screen_settings();
void tick_screen_settings();

void create_screen_blank();
void tick_screen_blank();

void create_user_widget_new_message_entry(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_new_message_entry(int startWidgetIndex);

void create_user_widget_add_message_entry(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_add_message_entry(int startWidgetIndex);

void create_user_widget_add_chat_entry(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_add_chat_entry(int startWidgetIndex);

void create_user_widget_message_container(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_message_container(int startWidgetIndex);

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/