#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_home_icon;
extern const lv_img_dsc_t img_nodes_icon;
extern const lv_img_dsc_t img_people_icon;
extern const lv_img_dsc_t img_chats_icon;
extern const lv_img_dsc_t img_map_icon;
extern const lv_img_dsc_t img_clock_icon;
extern const lv_img_dsc_t img_music_icon;
extern const lv_img_dsc_t img_tools_icon;
extern const lv_img_dsc_t img_apps_icon;
extern const lv_img_dsc_t img_statistics_icon;
extern const lv_img_dsc_t img_cogs_icon;
extern const lv_img_dsc_t img_power_icon;
extern const lv_img_dsc_t img_boot_logo;
extern const lv_img_dsc_t img_back_icon;
extern const lv_img_dsc_t img_no_tile_image;
extern const lv_img_dsc_t img_meshtastic_logo_image;
extern const lv_img_dsc_t img_sd_card_icon;
extern const lv_img_dsc_t img_home_signal_icon;
extern const lv_img_dsc_t img_lora_icon;
extern const lv_img_dsc_t img_home_nodes_icon;
extern const lv_img_dsc_t img_home_bell_icon;
extern const lv_img_dsc_t img_home_map_icon;
extern const lv_img_dsc_t img_home_qr_icon;
extern const lv_img_dsc_t img_home_wlan_icon;
extern const lv_img_dsc_t img_home_clock_icon;
extern const lv_img_dsc_t img_home_mail_icon;
extern const lv_img_dsc_t img_home_memory_icon;
extern const lv_img_dsc_t img_home_mqtt_icon;
extern const lv_img_dsc_t img_home_wlan_off_icon;
extern const lv_img_dsc_t img_home_signal_strong_icon;
extern const lv_img_dsc_t img_home_signal_fair_icon;
extern const lv_img_dsc_t img_home_signal_weak_icon;
extern const lv_img_dsc_t img_home_no_signal_icon;
extern const lv_img_dsc_t img_home_signal_good_icon;
extern const lv_img_dsc_t img_heart_image;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[35];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/