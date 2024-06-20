#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// nodes options tabview
extern lv_obj_t * ui_NodesOptionsPanel;
extern lv_obj_t * ui_NodesOptionsTabView;
extern lv_obj_t * ui_TabPageFilter;
extern lv_obj_t * ui_NodesFilterUnknownLabel;
extern lv_obj_t * ui_NodesFilterUnknownSwitch;
extern lv_obj_t * ui_NodesFilterOfflineLabel;
extern lv_obj_t * ui_NodesFilterOfflineSwitch;
extern lv_obj_t * ui_NodesFilterMQTTLabel;
extern lv_obj_t * ui_NodesFilterMQTTSwitch;
extern lv_obj_t * ui_NodesFilterPositionLabel;
extern lv_obj_t * ui_NodesFilterPositionSwitch;
extern lv_obj_t * ui_NodesFilterNameLabel;
extern lv_obj_t * ui_NodesFilterNameArea;
extern lv_obj_t * ui_TabPageHighlight;
extern lv_obj_t * ui_NodesHLActiveChatLabel;
extern lv_obj_t * ui_NodesHLActiveChatSwitch;
extern lv_obj_t * ui_NodesHLPositionLabel;
extern lv_obj_t * ui_NodesHLPositionSwitch;
extern lv_obj_t * ui_NodesHLTelemetryLabel;
extern lv_obj_t * ui_NodesHLTelemetrySwitch;
extern lv_obj_t * ui_NodesHLIAQLabel;
extern lv_obj_t * ui_NodesHLIAQSwitch;
extern lv_obj_t * ui_NodesHLNameLabel;
extern lv_obj_t * ui_NodesHLNameArea;
extern lv_obj_t * ui_KeyboardButton5;
extern lv_obj_t * ui_KeyboardButton6;


// advanced settings
extern lv_obj_t * ui_AdvancedSettingsPanel;
extern lv_obj_t * ui_SettingsTabView;
extern lv_obj_t * ui_TabPageGeneral;
extern lv_obj_t * ui_GeneralLanguageButton;
extern lv_obj_t * ui_LanguageLabel;
extern lv_obj_t * ui_GeneralTimezoneButton;
extern lv_obj_t * ui_TimezoneLabel;
extern lv_obj_t * ui_GeneralScreenButton;
extern lv_obj_t * ui_ScreenLabel;
extern lv_obj_t * ui_GeneralMapsButton;
extern lv_obj_t * ui_MapsLabel;
extern lv_obj_t * ui_GeneralAudioButton;
extern lv_obj_t * ui_AudioLabel1;
extern lv_obj_t * ui_TabPageRadio;
extern lv_obj_t * ui_RadioBluetoothButton;
extern lv_obj_t * ui_BluetoothLabel;
extern lv_obj_t * ui_RadioDeviceButton;
extern lv_obj_t * ui_DeviceLabel;
extern lv_obj_t * ui_RadioDisplayButton;
extern lv_obj_t * ui_DisplayLabel;
extern lv_obj_t * ui_RadioLoRaButton;
extern lv_obj_t * ui_LoRaLabel;
extern lv_obj_t * ui_RadioNetworkButton;
extern lv_obj_t * ui_NetworkLabel;
extern lv_obj_t * ui_RadioPositionButton;
extern lv_obj_t * ui_PositionLabel;
extern lv_obj_t * ui_RadioPowerButton;
extern lv_obj_t * ui_PowerLabel;
extern lv_obj_t * ui_TabPageModules;
extern lv_obj_t * ui_ModuleCannedMsgButton;
extern lv_obj_t * ui_CannedMsgLabel;
extern lv_obj_t * ui_ModuleSaFButton;
extern lv_obj_t * ui_StoreAndForwardLabel;
extern lv_obj_t * ui_ModuleTelemetryButton;
extern lv_obj_t * ui_TelemetryLabel;
extern lv_obj_t * ui_ModuleMQTTButton;
extern lv_obj_t * ui_MQTTLabel;
extern lv_obj_t * ui_ModuleRangeTestButton;
extern lv_obj_t * ui_RangeTestLabel;
extern lv_obj_t * ui_ModuleAudioButton;
extern lv_obj_t * ui_AudioLabel;
extern lv_obj_t * ui_ModuleSerialButton;
extern lv_obj_t * ui_SerialLabel;
extern lv_obj_t * ui_ModuleExtNotificationButton;
extern lv_obj_t * ui_ExtNotificationLabel;
extern lv_obj_t * ui_ModuleNeighborInfoButton;
extern lv_obj_t * ui_NeighborInfoLabel;
extern lv_obj_t * ui_ModuleAmbientLightingButton;
extern lv_obj_t * ui_AmbientLightingLabel;
extern lv_obj_t * ui_ModuleDetectionSensorButton;
extern lv_obj_t * ui_DetectionSensorLabel;
extern lv_obj_t * ui_ModuleRemoteHardwareButton;
extern lv_obj_t * ui_RemoteHardwareLabel;

void create_tabview_nodes_options(void);
void create_tabview_settings(void);

typedef struct _objects_t {
    lv_obj_t *boot_screen;
    lv_obj_t *main_screen;
    lv_obj_t *blank_screen;
    lv_obj_t *calibration_screen;
    lv_obj_t *advanced_settings_panel;
    lv_obj_t *alert_label;
    lv_obj_t *alert_panel;
    lv_obj_t *basic_settings_alert_button;
    lv_obj_t *basic_settings_alert_label;
    lv_obj_t *basic_settings_brightness_button;
    lv_obj_t *basic_settings_brightness_label;
    lv_obj_t *basic_settings_calibration_button;
    lv_obj_t *basic_settings_calibration_label;
    lv_obj_t *basic_settings_channel_button;
    lv_obj_t *basic_settings_channel_label;
    lv_obj_t *basic_settings_input_button;
    lv_obj_t *basic_settings_input_label;
    lv_obj_t *basic_settings_language_button;
    lv_obj_t *basic_settings_language_label;
    lv_obj_t *basic_settings_modem_preset_button;
    lv_obj_t *basic_settings_modem_preset_label;
    lv_obj_t *basic_settings_panel;
    lv_obj_t *basic_settings_reboot_button;
    lv_obj_t *basic_settings_reboot_label;
    lv_obj_t *basic_settings_region_button;
    lv_obj_t *basic_settings_region_label;
    lv_obj_t *basic_settings_reset_button;
    lv_obj_t *basic_settings_reset_label;
    lv_obj_t *basic_settings_role_button;
    lv_obj_t *basic_settings_role_label;
    lv_obj_t *basic_settings_timeout_button;
    lv_obj_t *basic_settings_timeout_label;
    lv_obj_t *basic_settings_user_button;
    lv_obj_t *basic_settings_user_label;
    lv_obj_t *battery_image;
    lv_obj_t *battery_label;
    lv_obj_t *battery_panel;
    lv_obj_t *battery_percentage_label;
    lv_obj_t *brightness_slider;
    lv_obj_t *button_panel;
    lv_obj_t *cancel_reboot_button;
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
    lv_obj_t *frequency_slot_label;
    lv_obj_t *frequency_slot_slider;
    lv_obj_t *groups_button;
    lv_obj_t *groups_panel;
    lv_obj_t *home_bluetooth_button;
    lv_obj_t *home_bluetooth_label;
    lv_obj_t *home_button;
    lv_obj_t *home_container;
    lv_obj_t *home_ethernet_button;
    lv_obj_t *home_ethernet_label;
    lv_obj_t *home_location_button;
    lv_obj_t *home_location_label;
    lv_obj_t *home_mail_button;
    lv_obj_t *home_mail_label;
    lv_obj_t *home_memory_button;
    lv_obj_t *home_memory_label;
    lv_obj_t *home_mqtt_button;
    lv_obj_t *home_mqtt_label;
    lv_obj_t *home_nodes_button;
    lv_obj_t *home_nodes_label;
    lv_obj_t *home_panel;
    lv_obj_t *home_time_button;
    lv_obj_t *home_time_label;
    lv_obj_t *home_wlan_button;
    lv_obj_t *home_wlan_label;
    lv_obj_t *keyboard;
    lv_obj_t *keyboard_button_0;
    lv_obj_t *keyboard_button_1;
    lv_obj_t *keyboard_button_2;
    lv_obj_t *keyboard_button_3;
    lv_obj_t *keyboard_button_4;
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
    lv_obj_t *obj0;
    lv_obj_t *obj0__cancel_button_w;
    lv_obj_t *obj0__ok_button_w;
    lv_obj_t *obj0__ok_cancel_panel_w;
    lv_obj_t *obj1;
    lv_obj_t *obj1__cancel_button_w;
    lv_obj_t *obj1__ok_button_w;
    lv_obj_t *obj1__ok_cancel_panel_w;
    lv_obj_t *obj10;
    lv_obj_t *obj10__cancel_button_w;
    lv_obj_t *obj10__ok_button_w;
    lv_obj_t *obj10__ok_cancel_panel_w;
    lv_obj_t *obj11;
    lv_obj_t *obj11__cancel_button_w;
    lv_obj_t *obj11__ok_button_w;
    lv_obj_t *obj11__ok_cancel_panel_w;
    lv_obj_t *obj2;
    lv_obj_t *obj2__cancel_button_w;
    lv_obj_t *obj2__ok_button_w;
    lv_obj_t *obj2__ok_cancel_panel_w;
    lv_obj_t *obj3;
    lv_obj_t *obj3__cancel_button_w;
    lv_obj_t *obj3__ok_button_w;
    lv_obj_t *obj3__ok_cancel_panel_w;
    lv_obj_t *obj4;
    lv_obj_t *obj4__cancel_button_w;
    lv_obj_t *obj4__ok_button_w;
    lv_obj_t *obj4__ok_cancel_panel_w;
    lv_obj_t *obj5;
    lv_obj_t *obj5__cancel_button_w;
    lv_obj_t *obj5__ok_button_w;
    lv_obj_t *obj5__ok_cancel_panel_w;
    lv_obj_t *obj6;
    lv_obj_t *obj6__cancel_button_w;
    lv_obj_t *obj6__ok_button_w;
    lv_obj_t *obj6__ok_cancel_panel_w;
    lv_obj_t *obj7;
    lv_obj_t *obj7__cancel_button_w;
    lv_obj_t *obj7__ok_button_w;
    lv_obj_t *obj7__ok_cancel_panel_w;
    lv_obj_t *obj8;
    lv_obj_t *obj8__cancel_button_w;
    lv_obj_t *obj8__ok_button_w;
    lv_obj_t *obj8__ok_cancel_panel_w;
    lv_obj_t *obj9;
    lv_obj_t *obj9__cancel_button_w;
    lv_obj_t *obj9__ok_button_w;
    lv_obj_t *obj9__ok_cancel_panel_w;
    lv_obj_t *position_label;
    lv_obj_t *position2_label;
    lv_obj_t *reboot_button;
    lv_obj_t *reboot_panel;
    lv_obj_t *screen_timeout_slider;
    lv_obj_t *settings_alert_buzzer_panel;
    lv_obj_t *settings_alert_buzzer_switch;
    lv_obj_t *settings_brightness_label;
    lv_obj_t *settings_brightness_panel;
    lv_obj_t *settings_button;
    lv_obj_t *settings_channel_panel;
    lv_obj_t *settings_channel0_button;
    lv_obj_t *settings_channel0_label;
    lv_obj_t *settings_channel1_button;
    lv_obj_t *settings_channel1_label;
    lv_obj_t *settings_channel2_button;
    lv_obj_t *settings_channel2_label;
    lv_obj_t *settings_channel3_button;
    lv_obj_t *settings_channel3_label;
    lv_obj_t *settings_channel4_button;
    lv_obj_t *settings_channel4_label;
    lv_obj_t *settings_channel5_button;
    lv_obj_t *settings_channel5_label;
    lv_obj_t *settings_channel6_button;
    lv_obj_t *settings_channel6_label;
    lv_obj_t *settings_channel7_button;
    lv_obj_t *settings_channel7_label;
    lv_obj_t *settings_device_role_dropdown;
    lv_obj_t *settings_device_role_panel;
    lv_obj_t *settings_input_control_panel;
    lv_obj_t *settings_keyboard_input_dropdown;
    lv_obj_t *settings_language_dropdown;
    lv_obj_t *settings_language_panel;
    lv_obj_t *settings_modem_preset_dropdown;
    lv_obj_t *settings_modem_preset_panel;
    lv_obj_t *settings_modify_channel_name_textarea;
    lv_obj_t *settings_modify_channel_panel;
    lv_obj_t *settings_modify_channel_psk_textarea;
    lv_obj_t *settings_modify_trash_button;
    lv_obj_t *settings_mouse_input_dropdown;
    lv_obj_t *settings_region_dropdown;
    lv_obj_t *settings_region_panel;
    lv_obj_t *settings_reset_dropdown;
    lv_obj_t *settings_reset_panel;
    lv_obj_t *settings_ringtone_dropdown;
    lv_obj_t *settings_screen_timeout_label;
    lv_obj_t *settings_screen_timeout_panel;
    lv_obj_t *settings_user_long_textarea;
    lv_obj_t *settings_user_short_textarea;
    lv_obj_t *settings_username_panel;
    lv_obj_t *shutdown_button;
    lv_obj_t *signal_label;
    lv_obj_t *telemetry1_label;
    lv_obj_t *telemetry2_label;
    lv_obj_t *top_advanced_settings_image;
    lv_obj_t *top_advanced_settings_label;
    lv_obj_t *top_advanced_settings_panel;
    lv_obj_t *top_basic_settings_image;
    lv_obj_t *top_basic_settings_label;
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
    lv_obj_t *top_node_options_image;
    lv_obj_t *top_node_options_label;
    lv_obj_t *top_node_options_panel;
    lv_obj_t *top_nodes_image;
    lv_obj_t *top_nodes_online_label;
    lv_obj_t *top_nodes_panel;
    lv_obj_t *top_panel;
    lv_obj_t *top_settings_panel;
    lv_obj_t *top_user_group_label;
    lv_obj_t *user_name_label;
    lv_obj_t *user_name_short_label;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_BOOT_SCREEN = 1,
    SCREEN_ID_MAIN_SCREEN = 2,
    SCREEN_ID_BLANK_SCREEN = 3,
    SCREEN_ID_CALIBRATION_SCREEN = 4,
    SCREEN_ID_OK_CANCEL_WIDGET = 5,
};

void create_screen_boot_screen();
void tick_screen_boot_screen();

void create_screen_main_screen();
void tick_screen_main_screen();

void create_screen_blank_screen();
void tick_screen_blank_screen();

void create_screen_calibration_screen();
void tick_screen_calibration_screen();

void create_user_widget_ok_cancel_widget(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_ok_cancel_widget(int startWidgetIndex);

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/