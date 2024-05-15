#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *boot_screen;
    lv_obj_t *main_screen;
    lv_obj_t *alert_label;
    lv_obj_t *alert_panel;
    lv_obj_t *battery_image;
    lv_obj_t *battery_label;
    lv_obj_t *battery_panel;
    lv_obj_t *battery_percentage_label;
    lv_obj_t *button_panel;
    lv_obj_t *channel_button0;
    lv_obj_t *channel_button1;
    lv_obj_t *channel_button2;
    lv_obj_t *channel_button3;
    lv_obj_t *channel_button4;
    lv_obj_t *channel_button5;
    lv_obj_t *channel_button6;
    lv_obj_t *channel_button7;
    lv_obj_t *channel_label0;
    lv_obj_t *channel_label1;
    lv_obj_t *channel_label2;
    lv_obj_t *channel_label3;
    lv_obj_t *channel_label4;
    lv_obj_t *channel_label5;
    lv_obj_t *channel_label6;
    lv_obj_t *channel_label7;
    lv_obj_t *chat_del_button;
    lv_obj_t *chats_button;
    lv_obj_t *chats_button_label;
    lv_obj_t *chats_panel;
    lv_obj_t *del_label;
    lv_obj_t *firmware_label;
    lv_obj_t *groups_button;
    lv_obj_t *groups_panel;
    lv_obj_t *home_bluetooth_button;
    lv_obj_t *home_bluetooth_label;
    lv_obj_t *home_button;
    lv_obj_t *home_container;
    lv_obj_t *home_location_button;
    lv_obj_t *home_location_label;
    lv_obj_t *home_mail_button;
    lv_obj_t *home_mail_label;
    lv_obj_t *home_memory_button;
    lv_obj_t *home_memory_label;
    lv_obj_t *home_nodes_button;
    lv_obj_t *home_nodes_label;
    lv_obj_t *home_panel;
    lv_obj_t *home_wlan_button;
    lv_obj_t *home_wlan_label;
    lv_obj_t *keyboard;
    lv_obj_t *keyboard_button;
    lv_obj_t *language_settings_panel;
    lv_obj_t *last_heard_label;
    lv_obj_t *map_button;
    lv_obj_t *map_panel;
    lv_obj_t *meshtastic_image;
    lv_obj_t *meshtastic_label;
    lv_obj_t *meshtastic_logo;
    lv_obj_t *meshtastic_url;
    lv_obj_t *message_input_area;
    lv_obj_t *messages_button;
    lv_obj_t *messages_container;
    lv_obj_t *messages_panel;
    lv_obj_t *msg_popup_button;
    lv_obj_t *msg_popup_label;
    lv_obj_t *msg_popup_panel;
    lv_obj_t *node_button;
    lv_obj_t *node_image;
    lv_obj_t *node_panel;
    lv_obj_t *nodes_button;
    lv_obj_t *nodes_panel;
    lv_obj_t *settings_button;
    lv_obj_t *settings_panel;
    lv_obj_t *signal_label;
    lv_obj_t *top_chats_image;
    lv_obj_t *top_chats_label;
    lv_obj_t *top_chats_panel;
    lv_obj_t *top_group_chat_image;
    lv_obj_t *top_group_chat_label;
    lv_obj_t *top_group_chat_panel;
    lv_obj_t *top_group_image;
    lv_obj_t *top_groups_panel;
    lv_obj_t *top_map_image;
    lv_obj_t *top_map_label;
    lv_obj_t *top_map_panel;
    lv_obj_t *top_messages_node_image;
    lv_obj_t *top_messages_node_label;
    lv_obj_t *top_messages_panel;
    lv_obj_t *top_nodes_image;
    lv_obj_t *top_nodes_online_label;
    lv_obj_t *top_nodes_panel;
    lv_obj_t *top_panel;
    lv_obj_t *top_settings_image;
    lv_obj_t *top_settings_label;
    lv_obj_t *top_settings_panel;
    lv_obj_t *top_user_group_label;
    lv_obj_t *user_name_label;
    lv_obj_t *user_name_short_label;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_BOOT_SCREEN = 1,
    SCREEN_ID_MAIN_SCREEN = 2,
};

void create_screen_boot_screen();
void tick_screen_boot_screen();

void create_screen_main_screen();
void tick_screen_main_screen();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/