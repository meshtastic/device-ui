#include "ui.h"

lv_obj_t * ui_AdvancedSettingsPanel;
lv_obj_t * ui_SettingsTabView;
lv_obj_t * ui_TabPageGeneral;
lv_obj_t * ui_GeneralLanguageButton;
lv_obj_t * ui_LanguageLabel;
lv_obj_t * ui_GeneralTimezoneButton;
lv_obj_t * ui_TimezoneLabel;
lv_obj_t * ui_GeneralScreenButton;
lv_obj_t * ui_ScreenLabel;
lv_obj_t * ui_GeneralMapsButton;
lv_obj_t * ui_MapsLabel;
lv_obj_t * ui_GeneralAudioButton;
lv_obj_t * ui_AudioLabel1;
lv_obj_t * ui_TabPageRadio;
lv_obj_t * ui_RadioBluetoothButton;
lv_obj_t * ui_BluetoothLabel;
lv_obj_t * ui_RadioDeviceButton;
lv_obj_t * ui_DeviceLabel;
lv_obj_t * ui_RadioDisplayButton;
lv_obj_t * ui_DisplayLabel;
lv_obj_t * ui_RadioLoRaButton;
lv_obj_t * ui_LoRaLabel;
lv_obj_t * ui_RadioNetworkButton;
lv_obj_t * ui_NetworkLabel;
lv_obj_t * ui_RadioPositionButton;
lv_obj_t * ui_PositionLabel;
lv_obj_t * ui_RadioPowerButton;
lv_obj_t * ui_PowerLabel;
lv_obj_t * ui_TabPageModules;
lv_obj_t * ui_ModuleCannedMsgButton;
lv_obj_t * ui_CannedMsgLabel;
lv_obj_t * ui_ModuleSaFButton;
lv_obj_t * ui_StoreAndForwardLabel;
lv_obj_t * ui_ModuleTelemetryButton;
lv_obj_t * ui_TelemetryLabel;
lv_obj_t * ui_ModuleMQTTButton;
lv_obj_t * ui_MQTTLabel;
lv_obj_t * ui_ModuleRangeTestButton;
lv_obj_t * ui_RangeTestLabel;
lv_obj_t * ui_ModuleAudioButton;
lv_obj_t * ui_AudioLabel;
lv_obj_t * ui_ModuleSerialButton;
lv_obj_t * ui_SerialLabel;
lv_obj_t * ui_ModuleExtNotificationButton;
lv_obj_t * ui_ExtNotificationLabel;
lv_obj_t * ui_ModuleNeighborInfoButton;
lv_obj_t * ui_NeighborInfoLabel;
lv_obj_t * ui_ModuleAmbientLightingButton;
lv_obj_t * ui_AmbientLightingLabel;
lv_obj_t * ui_ModuleDetectionSensorButton;
lv_obj_t * ui_DetectionSensorLabel;
lv_obj_t * ui_ModuleRemoteHardwareButton;
lv_obj_t * ui_RemoteHardwareLabel;

/**
 * @brief Create tabview settings panel (copied from squareline studio generated code)
 * 
 */
void create_tabview_settings()
{
    ui_AdvancedSettingsPanel = lv_obj_create(objects.main_screen);
    lv_obj_set_width(ui_AdvancedSettingsPanel, 284);
    lv_obj_set_height(ui_AdvancedSettingsPanel, lv_pct(90));
    lv_obj_set_align(ui_AdvancedSettingsPanel, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_add_flag(ui_AdvancedSettingsPanel, LV_OBJ_FLAG_HIDDEN);     /// Flags
    lv_obj_clear_flag(ui_AdvancedSettingsPanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                      LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                      LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);     /// Flags
    lv_obj_set_style_radius(ui_AdvancedSettingsPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_AdvancedSettingsPanel, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_AdvancedSettingsPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_AdvancedSettingsPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_SettingsTabView = lv_tabview_create(ui_AdvancedSettingsPanel);
    lv_tabview_set_tab_bar_position(ui_SettingsTabView, LV_DIR_BOTTOM);
    lv_tabview_set_tab_bar_size(ui_SettingsTabView, 25);
    lv_obj_set_width(ui_SettingsTabView, lv_pct(110));
    lv_obj_set_height(ui_SettingsTabView, lv_pct(114));
    lv_obj_set_align(ui_SettingsTabView, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_SettingsTabView, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_SettingsTabView, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_SettingsTabView, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(lv_tabview_get_tab_btns(ui_SettingsTabView), lv_color_hex(0x808080),
                                LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lv_tabview_get_tab_btns(ui_SettingsTabView), 255,  LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(lv_tabview_get_tab_btns(ui_SettingsTabView), LV_TEXT_ALIGN_CENTER,
                                LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(lv_tabview_get_tab_btns(ui_SettingsTabView), lv_color_hex(0x303030),
                              LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lv_tabview_get_tab_btns(ui_SettingsTabView), 255,  LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(lv_tabview_get_tab_btns(ui_SettingsTabView), lv_color_hex(0x303030),
                                   LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(lv_tabview_get_tab_btns(ui_SettingsTabView), lv_color_hex(0x404040),
                                  LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(lv_tabview_get_tab_btns(ui_SettingsTabView), 255,  LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(lv_tabview_get_tab_btns(ui_SettingsTabView), 1,  LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_tabview_get_tab_btns(ui_SettingsTabView), lv_color_hex(0xFFFFFF),
                                LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(lv_tabview_get_tab_btns(ui_SettingsTabView), 255,  LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_radius(lv_tabview_get_tab_btns(ui_SettingsTabView), 0,  LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(lv_tabview_get_tab_btns(ui_SettingsTabView), lv_color_hex(0x303030),
                              LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(lv_tabview_get_tab_btns(ui_SettingsTabView), 255,  LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_border_color(lv_tabview_get_tab_btns(ui_SettingsTabView), lv_color_hex(0x67EA94),
                                  LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_border_opa(lv_tabview_get_tab_btns(ui_SettingsTabView), 255,  LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(lv_tabview_get_tab_btns(ui_SettingsTabView), 3,  LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_border_side(lv_tabview_get_tab_btns(ui_SettingsTabView), LV_BORDER_SIDE_BOTTOM,
                                 LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(lv_tabview_get_tab_btns(ui_SettingsTabView), lv_color_hex(0xFFFFFF),
                                LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(lv_tabview_get_tab_btns(ui_SettingsTabView), 255,  LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(lv_tabview_get_tab_btns(ui_SettingsTabView), lv_color_hex(0x67EA94),
                              LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(lv_tabview_get_tab_btns(ui_SettingsTabView), 255,  LV_PART_ITEMS | LV_STATE_PRESSED);

    ui_TabPageGeneral = lv_tabview_add_tab(ui_SettingsTabView, "General");
    lv_obj_set_flex_flow(ui_TabPageGeneral, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_TabPageGeneral, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_TabPageGeneral,
                      LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                      LV_OBJ_FLAG_SCROLL_ELASTIC);     /// Flags
    lv_obj_set_scroll_dir(ui_TabPageGeneral, LV_DIR_VER);
    lv_obj_set_style_pad_row(ui_TabPageGeneral, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_TabPageGeneral, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_GeneralLanguageButton = lv_btn_create(ui_TabPageGeneral);
    lv_obj_set_height(ui_GeneralLanguageButton, 30);
    lv_obj_set_width(ui_GeneralLanguageButton, lv_pct(97));
    lv_obj_set_align(ui_GeneralLanguageButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_GeneralLanguageButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_GeneralLanguageButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_GeneralLanguageButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GeneralLanguageButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_GeneralLanguageButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_GeneralLanguageButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_GeneralLanguageButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_GeneralLanguageButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_GeneralLanguageButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GeneralLanguageButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_GeneralLanguageButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_GeneralLanguageButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_GeneralLanguageButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_LanguageLabel = lv_label_create(ui_GeneralLanguageButton);
    lv_obj_set_width(ui_LanguageLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LanguageLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_LanguageLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_LanguageLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_LanguageLabel, "Language");

    ui_GeneralTimezoneButton = lv_btn_create(ui_TabPageGeneral);
    lv_obj_set_height(ui_GeneralTimezoneButton, 30);
    lv_obj_set_width(ui_GeneralTimezoneButton, lv_pct(97));
    lv_obj_set_align(ui_GeneralTimezoneButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_GeneralTimezoneButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_GeneralTimezoneButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_GeneralTimezoneButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GeneralTimezoneButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_GeneralTimezoneButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_GeneralTimezoneButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_GeneralTimezoneButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_GeneralTimezoneButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_GeneralTimezoneButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GeneralTimezoneButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_GeneralTimezoneButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_GeneralTimezoneButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_GeneralTimezoneButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_TimezoneLabel = lv_label_create(ui_GeneralTimezoneButton);
    lv_obj_set_width(ui_TimezoneLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_TimezoneLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_TimezoneLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_TimezoneLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_TimezoneLabel, "Timezone");

    ui_GeneralScreenButton = lv_btn_create(ui_TabPageGeneral);
    lv_obj_set_height(ui_GeneralScreenButton, 30);
    lv_obj_set_width(ui_GeneralScreenButton, lv_pct(97));
    lv_obj_set_align(ui_GeneralScreenButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_GeneralScreenButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_GeneralScreenButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_GeneralScreenButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GeneralScreenButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_GeneralScreenButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_GeneralScreenButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_GeneralScreenButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_GeneralScreenButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_GeneralScreenButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GeneralScreenButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_GeneralScreenButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_GeneralScreenButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_GeneralScreenButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_ScreenLabel = lv_label_create(ui_GeneralScreenButton);
    lv_obj_set_width(ui_ScreenLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_ScreenLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_ScreenLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_ScreenLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_ScreenLabel, "Screen");

    ui_GeneralMapsButton = lv_btn_create(ui_TabPageGeneral);
    lv_obj_set_height(ui_GeneralMapsButton, 30);
    lv_obj_set_width(ui_GeneralMapsButton, lv_pct(97));
    lv_obj_set_align(ui_GeneralMapsButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_GeneralMapsButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_GeneralMapsButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_GeneralMapsButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GeneralMapsButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_GeneralMapsButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_GeneralMapsButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_GeneralMapsButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_GeneralMapsButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_GeneralMapsButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GeneralMapsButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_GeneralMapsButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_GeneralMapsButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_GeneralMapsButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MapsLabel = lv_label_create(ui_GeneralMapsButton);
    lv_obj_set_width(ui_MapsLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MapsLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MapsLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_MapsLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_MapsLabel, "Maps");

    ui_GeneralAudioButton = lv_btn_create(ui_TabPageGeneral);
    lv_obj_set_height(ui_GeneralAudioButton, 30);
    lv_obj_set_width(ui_GeneralAudioButton, lv_pct(97));
    lv_obj_set_align(ui_GeneralAudioButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_GeneralAudioButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_GeneralAudioButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_GeneralAudioButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GeneralAudioButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_GeneralAudioButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_GeneralAudioButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_GeneralAudioButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_GeneralAudioButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_GeneralAudioButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GeneralAudioButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_GeneralAudioButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_GeneralAudioButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_GeneralAudioButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_AudioLabel1 = lv_label_create(ui_GeneralAudioButton);
    lv_obj_set_width(ui_AudioLabel1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_AudioLabel1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_AudioLabel1, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_AudioLabel1, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_AudioLabel1, "Audio");

    ui_TabPageRadio = lv_tabview_add_tab(ui_SettingsTabView, "Radio");
    lv_obj_set_flex_flow(ui_TabPageRadio, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_TabPageRadio, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_TabPageRadio, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                      LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLL_ELASTIC);     /// Flags
    lv_obj_set_scroll_dir(ui_TabPageRadio, LV_DIR_VER);
    lv_obj_set_style_pad_row(ui_TabPageRadio, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_TabPageRadio, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_RadioBluetoothButton = lv_btn_create(ui_TabPageRadio);
    lv_obj_set_height(ui_RadioBluetoothButton, 30);
    lv_obj_set_width(ui_RadioBluetoothButton, lv_pct(97));
    lv_obj_set_align(ui_RadioBluetoothButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_RadioBluetoothButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_RadioBluetoothButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_RadioBluetoothButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioBluetoothButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_RadioBluetoothButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_RadioBluetoothButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_RadioBluetoothButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_RadioBluetoothButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_RadioBluetoothButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioBluetoothButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_RadioBluetoothButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_RadioBluetoothButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_RadioBluetoothButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_BluetoothLabel = lv_label_create(ui_RadioBluetoothButton);
    lv_obj_set_width(ui_BluetoothLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_BluetoothLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_BluetoothLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_BluetoothLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_BluetoothLabel, "Bluetooth");

    ui_RadioDeviceButton = lv_btn_create(ui_TabPageRadio);
    lv_obj_set_height(ui_RadioDeviceButton, 30);
    lv_obj_set_width(ui_RadioDeviceButton, lv_pct(97));
    lv_obj_set_align(ui_RadioDeviceButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_RadioDeviceButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_RadioDeviceButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_RadioDeviceButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioDeviceButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_RadioDeviceButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_RadioDeviceButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_RadioDeviceButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_RadioDeviceButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_RadioDeviceButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioDeviceButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_RadioDeviceButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_grad_color(ui_RadioDeviceButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_RadioDeviceButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_RadioDeviceButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_DeviceLabel = lv_label_create(ui_RadioDeviceButton);
    lv_obj_set_width(ui_DeviceLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_DeviceLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_DeviceLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_DeviceLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_DeviceLabel, "Device");

    ui_RadioDisplayButton = lv_btn_create(ui_TabPageRadio);
    lv_obj_set_height(ui_RadioDisplayButton, 30);
    lv_obj_set_width(ui_RadioDisplayButton, lv_pct(97));
    lv_obj_set_align(ui_RadioDisplayButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_RadioDisplayButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_RadioDisplayButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_RadioDisplayButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioDisplayButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_RadioDisplayButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_RadioDisplayButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_RadioDisplayButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_RadioDisplayButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_RadioDisplayButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioDisplayButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_RadioDisplayButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_RadioDisplayButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_RadioDisplayButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_DisplayLabel = lv_label_create(ui_RadioDisplayButton);
    lv_obj_set_width(ui_DisplayLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_DisplayLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_DisplayLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_DisplayLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_DisplayLabel, "Display");

    ui_RadioLoRaButton = lv_btn_create(ui_TabPageRadio);
    lv_obj_set_height(ui_RadioLoRaButton, 30);
    lv_obj_set_width(ui_RadioLoRaButton, lv_pct(97));
    lv_obj_set_align(ui_RadioLoRaButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_RadioLoRaButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_RadioLoRaButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_RadioLoRaButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioLoRaButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_RadioLoRaButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_RadioLoRaButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_RadioLoRaButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_RadioLoRaButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_RadioLoRaButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioLoRaButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_RadioLoRaButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_RadioLoRaButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_RadioLoRaButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_LoRaLabel = lv_label_create(ui_RadioLoRaButton);
    lv_obj_set_width(ui_LoRaLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LoRaLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_LoRaLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_LoRaLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_LoRaLabel, "LoRa");

    ui_RadioNetworkButton = lv_btn_create(ui_TabPageRadio);
    lv_obj_set_height(ui_RadioNetworkButton, 30);
    lv_obj_set_width(ui_RadioNetworkButton, lv_pct(97));
    lv_obj_set_align(ui_RadioNetworkButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_RadioNetworkButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_RadioNetworkButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_RadioNetworkButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioNetworkButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_RadioNetworkButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_RadioNetworkButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_RadioNetworkButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_RadioNetworkButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_RadioNetworkButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioNetworkButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_RadioNetworkButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_RadioNetworkButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_RadioNetworkButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_NetworkLabel = lv_label_create(ui_RadioNetworkButton);
    lv_obj_set_width(ui_NetworkLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_NetworkLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_NetworkLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_NetworkLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_NetworkLabel, "Network");

    ui_RadioPositionButton = lv_btn_create(ui_TabPageRadio);
    lv_obj_set_height(ui_RadioPositionButton, 30);
    lv_obj_set_width(ui_RadioPositionButton, lv_pct(97));
    lv_obj_set_align(ui_RadioPositionButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_RadioPositionButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_RadioPositionButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_RadioPositionButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioPositionButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_RadioPositionButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_RadioPositionButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_RadioPositionButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_RadioPositionButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_RadioPositionButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioPositionButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_RadioPositionButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_RadioPositionButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_RadioPositionButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_PositionLabel = lv_label_create(ui_RadioPositionButton);
    lv_obj_set_width(ui_PositionLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_PositionLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_PositionLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_PositionLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_PositionLabel, "Position");

    ui_RadioPowerButton = lv_btn_create(ui_TabPageRadio);
    lv_obj_set_height(ui_RadioPowerButton, 30);
    lv_obj_set_width(ui_RadioPowerButton, lv_pct(97));
    lv_obj_set_align(ui_RadioPowerButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_RadioPowerButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_RadioPowerButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_RadioPowerButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioPowerButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_RadioPowerButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_RadioPowerButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_RadioPowerButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_RadioPowerButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_RadioPowerButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_RadioPowerButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_RadioPowerButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_RadioPowerButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_RadioPowerButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_PowerLabel = lv_label_create(ui_RadioPowerButton);
    lv_obj_set_width(ui_PowerLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_PowerLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_PowerLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_PowerLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_PowerLabel, "Power");

    ui_TabPageModules = lv_tabview_add_tab(ui_SettingsTabView, "Modules");
    lv_obj_set_flex_flow(ui_TabPageModules, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_TabPageModules, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_TabPageModules,
                      LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                      LV_OBJ_FLAG_SCROLL_ELASTIC);     /// Flags
    lv_obj_set_scroll_dir(ui_TabPageModules, LV_DIR_VER);
    lv_obj_set_style_pad_row(ui_TabPageModules, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_TabPageModules, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ModuleCannedMsgButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleCannedMsgButton, 30);
    lv_obj_set_width(ui_ModuleCannedMsgButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleCannedMsgButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleCannedMsgButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleCannedMsgButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleCannedMsgButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleCannedMsgButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleCannedMsgButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleCannedMsgButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleCannedMsgButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleCannedMsgButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleCannedMsgButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleCannedMsgButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleCannedMsgButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleCannedMsgButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleCannedMsgButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_CannedMsgLabel = lv_label_create(ui_ModuleCannedMsgButton);
    lv_obj_set_width(ui_CannedMsgLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_CannedMsgLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_CannedMsgLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_CannedMsgLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_CannedMsgLabel, "Canned Messages");

    ui_ModuleSaFButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleSaFButton, 30);
    lv_obj_set_width(ui_ModuleSaFButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleSaFButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleSaFButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleSaFButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleSaFButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleSaFButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleSaFButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleSaFButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleSaFButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleSaFButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleSaFButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleSaFButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleSaFButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_grad_color(ui_ModuleSaFButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleSaFButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleSaFButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_StoreAndForwardLabel = lv_label_create(ui_ModuleSaFButton);
    lv_obj_set_width(ui_StoreAndForwardLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_StoreAndForwardLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_StoreAndForwardLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_StoreAndForwardLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_StoreAndForwardLabel, "Store & Forward");

    ui_ModuleTelemetryButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleTelemetryButton, 30);
    lv_obj_set_width(ui_ModuleTelemetryButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleTelemetryButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleTelemetryButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleTelemetryButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleTelemetryButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleTelemetryButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleTelemetryButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleTelemetryButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleTelemetryButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleTelemetryButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleTelemetryButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleTelemetryButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleTelemetryButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleTelemetryButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleTelemetryButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_TelemetryLabel = lv_label_create(ui_ModuleTelemetryButton);
    lv_obj_set_width(ui_TelemetryLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_TelemetryLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_TelemetryLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_TelemetryLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_TelemetryLabel, "Telemetry");

    ui_ModuleMQTTButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleMQTTButton, 30);
    lv_obj_set_width(ui_ModuleMQTTButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleMQTTButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleMQTTButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleMQTTButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleMQTTButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleMQTTButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleMQTTButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleMQTTButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleMQTTButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleMQTTButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleMQTTButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleMQTTButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleMQTTButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleMQTTButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleMQTTButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MQTTLabel = lv_label_create(ui_ModuleMQTTButton);
    lv_obj_set_width(ui_MQTTLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MQTTLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MQTTLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_MQTTLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_MQTTLabel, "MQTT");

    ui_ModuleRangeTestButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleRangeTestButton, 30);
    lv_obj_set_width(ui_ModuleRangeTestButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleRangeTestButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleRangeTestButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleRangeTestButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleRangeTestButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleRangeTestButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleRangeTestButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleRangeTestButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleRangeTestButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleRangeTestButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleRangeTestButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleRangeTestButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleRangeTestButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleRangeTestButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleRangeTestButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_RangeTestLabel = lv_label_create(ui_ModuleRangeTestButton);
    lv_obj_set_width(ui_RangeTestLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_RangeTestLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_RangeTestLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_RangeTestLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_RangeTestLabel, "Range Test");

    ui_ModuleAudioButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleAudioButton, 30);
    lv_obj_set_width(ui_ModuleAudioButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleAudioButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleAudioButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleAudioButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleAudioButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleAudioButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleAudioButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleAudioButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleAudioButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleAudioButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleAudioButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleAudioButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleAudioButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleAudioButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleAudioButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_AudioLabel = lv_label_create(ui_ModuleAudioButton);
    lv_obj_set_width(ui_AudioLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_AudioLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_AudioLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_AudioLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_AudioLabel, "Audio");

    ui_ModuleSerialButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleSerialButton, 30);
    lv_obj_set_width(ui_ModuleSerialButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleSerialButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleSerialButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleSerialButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleSerialButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleSerialButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleSerialButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleSerialButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleSerialButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleSerialButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleSerialButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleSerialButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleSerialButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleSerialButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleSerialButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_SerialLabel = lv_label_create(ui_ModuleSerialButton);
    lv_obj_set_width(ui_SerialLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_SerialLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_SerialLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_SerialLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_SerialLabel, "Serial");

    ui_ModuleExtNotificationButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleExtNotificationButton, 30);
    lv_obj_set_width(ui_ModuleExtNotificationButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleExtNotificationButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleExtNotificationButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleExtNotificationButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleExtNotificationButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleExtNotificationButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleExtNotificationButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleExtNotificationButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleExtNotificationButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleExtNotificationButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleExtNotificationButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleExtNotificationButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleExtNotificationButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleExtNotificationButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleExtNotificationButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_ExtNotificationLabel = lv_label_create(ui_ModuleExtNotificationButton);
    lv_obj_set_width(ui_ExtNotificationLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_ExtNotificationLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_ExtNotificationLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_ExtNotificationLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_ExtNotificationLabel, "External Notification");

    ui_ModuleNeighborInfoButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleNeighborInfoButton, 30);
    lv_obj_set_width(ui_ModuleNeighborInfoButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleNeighborInfoButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleNeighborInfoButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleNeighborInfoButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleNeighborInfoButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleNeighborInfoButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleNeighborInfoButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleNeighborInfoButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleNeighborInfoButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleNeighborInfoButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleNeighborInfoButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleNeighborInfoButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleNeighborInfoButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleNeighborInfoButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleNeighborInfoButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_NeighborInfoLabel = lv_label_create(ui_ModuleNeighborInfoButton);
    lv_obj_set_width(ui_NeighborInfoLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_NeighborInfoLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_NeighborInfoLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_NeighborInfoLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_NeighborInfoLabel, "Neighbor Info");

    ui_ModuleAmbientLightingButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleAmbientLightingButton, 30);
    lv_obj_set_width(ui_ModuleAmbientLightingButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleAmbientLightingButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleAmbientLightingButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleAmbientLightingButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleAmbientLightingButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleAmbientLightingButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleAmbientLightingButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleAmbientLightingButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleAmbientLightingButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleAmbientLightingButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleAmbientLightingButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleAmbientLightingButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleAmbientLightingButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleAmbientLightingButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleAmbientLightingButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_AmbientLightingLabel = lv_label_create(ui_ModuleAmbientLightingButton);
    lv_obj_set_width(ui_AmbientLightingLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_AmbientLightingLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_AmbientLightingLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_AmbientLightingLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_AmbientLightingLabel, "Ambient Lighting");

    ui_ModuleDetectionSensorButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleDetectionSensorButton, 30);
    lv_obj_set_width(ui_ModuleDetectionSensorButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleDetectionSensorButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleDetectionSensorButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleDetectionSensorButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleDetectionSensorButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleDetectionSensorButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleDetectionSensorButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleDetectionSensorButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleDetectionSensorButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleDetectionSensorButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleDetectionSensorButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleDetectionSensorButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleDetectionSensorButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleDetectionSensorButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleDetectionSensorButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_DetectionSensorLabel = lv_label_create(ui_ModuleDetectionSensorButton);
    lv_obj_set_width(ui_DetectionSensorLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_DetectionSensorLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_DetectionSensorLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_DetectionSensorLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_DetectionSensorLabel, "Detection Sensor");

    ui_ModuleRemoteHardwareButton = lv_btn_create(ui_TabPageModules);
    lv_obj_set_height(ui_ModuleRemoteHardwareButton, 30);
    lv_obj_set_width(ui_ModuleRemoteHardwareButton, lv_pct(97));
    lv_obj_set_align(ui_ModuleRemoteHardwareButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ModuleRemoteHardwareButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_ModuleRemoteHardwareButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_ModuleRemoteHardwareButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleRemoteHardwareButton, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ModuleRemoteHardwareButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ModuleRemoteHardwareButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ModuleRemoteHardwareButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_ModuleRemoteHardwareButton, lv_color_hex(0xAAFBFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_ModuleRemoteHardwareButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ModuleRemoteHardwareButton, lv_color_hex(0x4DB270), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_ModuleRemoteHardwareButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_ModuleRemoteHardwareButton, lv_color_hex(0x015114), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_ModuleRemoteHardwareButton, 255, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_RemoteHardwareLabel = lv_label_create(ui_ModuleRemoteHardwareButton);
    lv_obj_set_width(ui_RemoteHardwareLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_RemoteHardwareLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_RemoteHardwareLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_long_mode(ui_RemoteHardwareLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(ui_RemoteHardwareLabel, "Remote Hardware");
}