#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_meshtastic_boot_logo_image;
extern const lv_img_dsc_t img_settings_button_image;
extern const lv_img_dsc_t img_map_button_image;
extern const lv_img_dsc_t img_messages_button_image;
extern const lv_img_dsc_t img_groups_button_image;
extern const lv_img_dsc_t img_nodes_button_image;
extern const lv_img_dsc_t img_home_button_image;
extern const lv_img_dsc_t img_home_mail_button_image;
extern const lv_img_dsc_t img_home_nodes_button_image;
extern const lv_img_dsc_t img_home_location_button_image;
extern const lv_img_dsc_t img_home_wlan_button_image;
extern const lv_img_dsc_t img_home_bluetooth_on_button_image;
extern const lv_img_dsc_t img_home_memory_button;
extern const lv_img_dsc_t img_node_client_image;
extern const lv_img_dsc_t img_keyboard_image;
extern const lv_img_dsc_t img_worldmap_image;
extern const lv_img_dsc_t img_top_nodes_image;
extern const lv_img_dsc_t img_top_group_image;
extern const lv_img_dsc_t img_top_chats_image;
extern const lv_img_dsc_t img_top_settings_image;
extern const lv_img_dsc_t img_top_map_image;
extern const lv_img_dsc_t img_msg_popup_button_image;
extern const lv_img_dsc_t img_top_message_node_images;
extern const lv_img_dsc_t img_battery_full_image;
extern const lv_img_dsc_t img_battery_mid_image;
extern const lv_img_dsc_t img_battery_low_image;
extern const lv_img_dsc_t img_battery_empty_warn_image;
extern const lv_img_dsc_t img_battery_bolt_image;
extern const lv_img_dsc_t img_home_wlan_off_image;
extern const lv_img_dsc_t img_home_bluetooth_off_button_image;
extern const lv_img_dsc_t img_node_router_image;
extern const lv_img_dsc_t img_node_sensor_image;
extern const lv_img_dsc_t img_home_mail_unread_button_image;
extern const lv_img_dsc_t img_groups_key_image;
extern const lv_img_dsc_t img_groups_lock_image;
extern const lv_img_dsc_t img_groups_unlock_image;
extern const lv_img_dsc_t img_knob_logo_image;
extern const lv_img_dsc_t img_circle_question_image;
extern const lv_img_dsc_t img_meshtastic_logo_image;
extern const lv_img_dsc_t img_settings_trash_image;
extern const lv_img_dsc_t img_reboot_image;
extern const lv_img_dsc_t img_shutdown_image;
extern const lv_img_dsc_t img_home_clock_image;
extern const lv_img_dsc_t img_home_ethernet_button_image;
extern const lv_img_dsc_t img_top_trace_route_image;
extern const lv_img_dsc_t img_top_node_detector_image;
extern const lv_img_dsc_t img_battery_empty_image;
extern const lv_img_dsc_t img_signal_slider_image;
extern const lv_img_dsc_t img_signal_full_image;
extern const lv_img_dsc_t img_battery_plug_image;
extern const lv_img_dsc_t img_mqtt_logo2_image;
extern const lv_img_dsc_t img_top_chart_image;
extern const lv_img_dsc_t img_debug_log_image;
extern const lv_img_dsc_t img_top_neighbors_image;
extern const lv_img_dsc_t img_radar_beam_image;
extern const lv_img_dsc_t img_lock_channel_image;
extern const lv_img_dsc_t img_lock_secure_image;
extern const lv_img_dsc_t img_home_lora_image;
extern const lv_img_dsc_t img_top_lora_image;
extern const lv_img_dsc_t img_home_bell_image;
extern const lv_img_dsc_t img_home_bell_slash_image;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[61];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/