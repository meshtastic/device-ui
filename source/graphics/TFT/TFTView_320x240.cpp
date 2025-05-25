#if HAS_TFT // VIEW_320x240

#include "graphics/view/TFT/TFTView_320x240.h"
#include "Arduino.h"
#include "graphics/common/BatteryLevel.h"
#include "graphics/common/LoRaPresets.h"
#include "graphics/common/Ringtones.h"
#include "graphics/common/ViewController.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverFactory.h"
#include "graphics/view/TFT/Themes.h"
#include "images.h"
#include "input/InputDriver.h"
#include "lv_i18n.h"
#include "lvgl_private.h"
#include "styles.h"
#include "ui.h"
#include "util/ILog.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#include <list>
#include <locale>
#include <random>
#include <time.h>

#ifdef ARCH_PORTDUINO
#include "util/LinuxHelper.h"
#else
#include "SD.h"
#endif

#ifndef MAX_NUM_NODES_VIEW
#define MAX_NUM_NODES_VIEW 250
#endif

#ifndef PACKET_LOGS_MAX
#define PACKET_LOGS_MAX 200
#endif

#define CR_REPLACEMENT 0x0C              // dummy to record several lines in a one line textarea
#define THIS TFTView_320x240::instance() // need to use this in all static methods

#define LV_COLOR_HEX(C)                                                                                                          \
    {                                                                                                                            \
        .blue = (C >> 0) & 0xff, .green = (C >> 8) & 0xff, .red = (C >> 16) & 0xff                                               \
    }

#define VALID_TIME(T) (T > 1000000 && T < UINT32_MAX)

constexpr lv_color_t colorRed = LV_COLOR_HEX(0xff5555);
constexpr lv_color_t colorDarkRed = LV_COLOR_HEX(0xa70a0a);
constexpr lv_color_t colorOrange = LV_COLOR_HEX(0xff8c04);
constexpr lv_color_t colorYellow = LV_COLOR_HEX(0xdbd251);
constexpr lv_color_t colorBlueGreen = LV_COLOR_HEX(0x05f6cb);
constexpr lv_color_t colorBlue = LV_COLOR_HEX(0x436C70);
constexpr lv_color_t colorGray = LV_COLOR_HEX(0x757575);
constexpr lv_color_t colorLightGray = LV_COLOR_HEX(0xAAFBFF);
constexpr lv_color_t colorMidGray = LV_COLOR_HEX(0x808080);
constexpr lv_color_t colorDarkGray = LV_COLOR_HEX(0x303030);
constexpr lv_color_t colorMesh = LV_COLOR_HEX(0x67ea94);

// children index of nodepanel lv objects (see addNode)
enum NodePanelIdx {
    node_img_idx,
    node_btn_idx,
    node_lbl_idx,
    node_lbs_idx,
    node_bat_idx,
    node_lh_idx,
    node_sig_idx,
    node_pos1_idx,
    node_pos2_idx,
    node_tm1_idx,
    node_tm2_idx
};

extern const char *firmware_version;

TFTView_320x240 *TFTView_320x240::gui = nullptr;
lv_obj_t *TFTView_320x240::currentPanel = nullptr;
lv_obj_t *TFTView_320x240::spinnerButton = nullptr;
uint32_t TFTView_320x240::currentNode = 0;
time_t TFTView_320x240::startTime = 0;
uint32_t TFTView_320x240::pinKeys = 0;
bool TFTView_320x240::screenLocked = false;
bool TFTView_320x240::screenUnlockRequest = false;

TFTView_320x240 *TFTView_320x240::instance(void)
{
    if (!gui) {
        gui = new TFTView_320x240(nullptr, DisplayDriverFactory::create(320, 240));
    }
    return gui;
}

TFTView_320x240 *TFTView_320x240::instance(const DisplayDriverConfig &cfg)
{
    if (!gui) {
        gui = new TFTView_320x240(&cfg, DisplayDriverFactory::create(cfg));
    }
    return gui;
}

TFTView_320x240::TFTView_320x240(const DisplayDriverConfig *cfg, DisplayDriver *driver)
    : MeshtasticView(cfg, driver, new ViewController), screensInitialised(false), nodesFiltered(0), nodesChanged(true),
      processingFilter(false), packetLogEnabled(false), detectorRunning(false), packetCounter(0), actTime(0), uptime(0),
      lastHeard(0), hasPosition(false), topNodeLL(nullptr), scans(0), selectedHops(0), chooseNodeSignalScanner(false),
      chooseNodeTraceRoute(false), qr(nullptr), db{}
{
    filter.active = false;
    highlight.active = false;
    objects.main_screen = nullptr;
}

/**
 * @brief Initialize view and boot screen
 * Note: We'll wait until we got our persistent data and then initialize
 *       the remaining screens.
 */
void TFTView_320x240::init(IClientBase *client)
{
    ILOG_DEBUG("TFTView_320x240 init...");
    ILOG_DEBUG("TFTView_320x240 db size: %d", sizeof(TFTView_320x240));
    ILOG_DEBUG("### Images size in flash ###");
    uint32_t total_size = 0;
    for (int i = 0; i < sizeof(images) / sizeof(ext_img_desc_t); i++) {
        total_size += images[i].img_dsc->data_size;
        ILOG_DEBUG("    %s: %d", images[i].name, images[i].img_dsc->data_size);
    }
    ILOG_DEBUG("================================");
    ILOG_DEBUG("### Total size: %d bytes ###", total_size);

    MeshtasticView::init(client);

    ui_init_boot();
    lv_label_set_text(objects.firmware_label, firmware_version);

    time(&lastrun60);
    time(&lastrun10);
    time(&lastrun5);
    time(&lastrun1);

    lv_obj_add_event_cb(objects.boot_logo_button, ui_event_LogoButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.blank_screen_button, ui_event_BlankScreenButton, LV_EVENT_ALL, NULL);
}

/**
 * @brief initialize UI with persistent data
 */
bool TFTView_320x240::setupUIConfig(const meshtastic_DeviceUIConfig &uiconfig)
{
    if (uiconfig.version == 1) {
        ILOG_INFO("setupUIConfig version %d", uiconfig.version);
        db.uiConfig = uiconfig;
        if (db.uiConfig.screen_timeout == 1) {
            db.uiConfig.screen_timeout = 30;
            controller->storeUIConfig(db.uiConfig);
        }
    } else {
        ILOG_WARN("invalid uiconfig version %d, reset UI settings to default", uiconfig.version);
        db.uiConfig.version = 1;
        db.uiConfig.screen_brightness = 153;
        db.uiConfig.screen_timeout = 30;
        controller->storeUIConfig(db.uiConfig);
    }

    lv_i18n_init(lv_i18n_language_pack);
    setLocale(db.uiConfig.language);

    if (state == MeshtasticView::eEnterProgrammingMode || state == MeshtasticView::eProgrammingMode) {
        enterProgrammingMode();
        return false;
    }

    state = MeshtasticView::eSetupUIConfig;

    // now we have set language, continue creating all screens
    if (!screensInitialised)
        init_screens();

    // set language
    setLanguage(db.uiConfig.language);

    // TODO: set virtual keyboard according language
    //  setKeyboard(db.uiConfig.language);

    // set theme
    setTheme(db.uiConfig.theme);

    // grey out bell until we got the ringtone (0 = silent)
    Themes::recolorButton(objects.home_bell_button, false);
    Themes::recolorText(objects.home_bell_label, false);

    lv_obj_set_style_bg_img_recolor(objects.home_button, colorMesh, LV_PART_MAIN | LV_STATE_DEFAULT);

    // set brightness
    if (displaydriver->hasLight())
        THIS->setBrightness(db.uiConfig.screen_brightness);

    // set timeout
    THIS->setTimeout(db.uiConfig.screen_timeout);

    // set screen/settings lock
    char buf[40];
    lv_snprintf(buf, 40, _("Lock: %s/%s"), db.uiConfig.screen_lock ? _("on") : _("off"),
                db.uiConfig.settings_lock ? _("on") : _("off"));
    lv_label_set_text(objects.basic_settings_screen_lock_label, buf);

    // set node filter options
    meshtastic_NodeFilter &filter = db.uiConfig.node_filter;
    lv_obj_set_state(objects.nodes_filter_unknown_switch, LV_STATE_CHECKED, filter.unknown_switch);
    lv_obj_set_state(objects.nodes_filter_offline_switch, LV_STATE_CHECKED, filter.offline_switch);
    lv_obj_set_state(objects.nodes_filter_public_key_switch, LV_STATE_CHECKED, filter.public_key_switch);
    // lv_dropdown_set_selected(objects.nodes_filter_channel_dropdown, filter.channel);
    lv_dropdown_set_selected(objects.nodes_filter_hops_dropdown, filter.hops_away);
    // lv_obj_set_state(objects.nodes_filter_mqtt_switch, LV_STATE_CHECKED, filter.mqtt_switch);
    lv_obj_set_state(objects.nodes_filter_position_switch, LV_STATE_CHECKED, filter.position_switch);
    lv_textarea_set_text(objects.nodes_filter_name_area, filter.node_name);

    // set node highlight options
    meshtastic_NodeHighlight &highlight = db.uiConfig.node_highlight;
    lv_obj_set_state(objects.nodes_hl_active_chat_switch, LV_STATE_CHECKED, highlight.chat_switch);
    lv_obj_set_state(objects.nodes_hl_position_switch, LV_STATE_CHECKED, highlight.position_switch);
    lv_obj_set_state(objects.nodes_hl_telemetry_switch, LV_STATE_CHECKED, highlight.telemetry_switch);
    lv_obj_set_state(objects.nodes_hliaq_switch, LV_STATE_CHECKED, highlight.iaq_switch);
    lv_textarea_set_text(objects.nodes_hl_name_area, highlight.node_name);

    // initialize own node panel
    if (ownNode && objects.node_panel)
        nodes[ownNode] = objects.node_panel;

    // touch screen calibration data
    uint16_t *parameters = (uint16_t *)db.uiConfig.calibration_data.bytes;
    if (db.uiConfig.calibration_data.size == 16 && (parameters[0] || parameters[7])) {
#ifndef IGNORE_CALIBRATION_DATA
        bool done = displaydriver->calibrate(parameters);
        char buf[32];
        lv_snprintf(buf, sizeof(buf), _("Screen Calibration: %s"), done ? _("done") : _("default"));
        lv_label_set_text(objects.basic_settings_calibration_label, buf);
#endif
    }

    // update home panel bell text
    setBellText(db.uiConfig.alert_enabled, !db.silent);
    bool off = !db.uiConfig.alert_enabled && db.silent;
    Themes::recolorButton(objects.home_bell_button, !off);
    Themes::recolorText(objects.home_bell_label, !off);
    objects.home_bell_button->user_data = (void *)off;

    // check SD card
    updateSDCard();

    lv_disp_trig_activity(NULL);
    return true;
}

/**
 * @brief display custom message on boot screen
 *        Note: currently, the firmware version field is used and set in main()/setup()
 */
void TFTView_320x240::updateBootMessage(void)
{
    LOG_DEBUG("updateBootMessage");
    lv_label_set_text(objects.firmware_label, firmware_version);
}

/**
 * @brief Initialize all screens and apply customizations
 *
 */
void TFTView_320x240::init_screens(void)
{
    ILOG_DEBUG("init screens...");
    state = MeshtasticView::eInitScreens;
    ui_init();
    apply_hotfix();

    activeMsgContainer = objects.messages_container;
    // setup the two channel label panels with arrays that allow indexing
    channel = {objects.channel_label0, objects.channel_label1, objects.channel_label2, objects.channel_label3,
               objects.channel_label4, objects.channel_label5, objects.channel_label6, objects.channel_label7};
    ch_label = {objects.settings_channel0_label, objects.settings_channel1_label, objects.settings_channel2_label,
                objects.settings_channel3_label, objects.settings_channel4_label, objects.settings_channel5_label,
                objects.settings_channel6_label, objects.settings_channel7_label};

    channelGroup = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    ui_set_active(objects.home_button, objects.home_panel, objects.top_panel);
    ui_events_init();

    // load main screen
    lv_screen_load_anim(objects.main_screen, LV_SCR_LOAD_ANIM_NONE, 300, 2000, false);

    // re-configuration based on capabilities
    if (!displaydriver->hasLight())
        lv_obj_add_flag(objects.basic_settings_brightness_button, LV_OBJ_FLAG_HIDDEN);

#ifndef CUSTOM_TOUCH_DRIVER
    if (!displaydriver->hasTouch())
#endif
        lv_obj_add_flag(objects.basic_settings_calibration_button, LV_OBJ_FLAG_HIDDEN);

#if LV_USE_LIBINPUT
    lv_obj_clear_flag(objects.basic_settings_input_button, LV_OBJ_FLAG_HIDDEN);
#endif

#if defined(USE_I2S_BUZZER) || defined(USE_PIN_BUZZER)
    lv_obj_clear_flag(objects.basic_settings_alert_button, LV_OBJ_FLAG_HIDDEN);
    db.uiConfig.ring_tone_id = 0;
#else
    lv_obj_add_flag(objects.basic_settings_alert_button, LV_OBJ_FLAG_HIDDEN);
#endif

#ifndef USE_ROUTER_ROLE
    lv_dropdown_set_options(objects.settings_device_role_dropdown,
                            _("Client\nClient Mute\nTracker\nSensor\nTAK\nClient Hidden\nLost & Found\nTAK Tracker"));
#endif

    // signal scanner scale
#if defined(USE_SX127x)
    lv_label_set_text(objects.signal_scanner_rssi_scale_label, "-50\n-60\n-70\n-80\n-90\n-100\n-110\n-120\n-130\n-140\n-150");
    lv_slider_set_range(objects.rssi_slider, -150, -50);
    lv_label_set_text(objects.signal_scanner_snr_scale_label,
                      "14.0\n12.0\n10.0\n8.0\n6.0\n4.0\n2.0\n0.0\n-2.0\n-4.0\n-8.0\n-10.0\n-12.0\n-14.0\n-16.0");
    lv_obj_set_style_text_line_space(objects.signal_scanner_snr_scale_label, -2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_slider_set_range(objects.snr_slider, -17, 15);
#else
    lv_label_set_text(objects.signal_scanner_rssi_scale_label, "-20\n-30\n-40\n-50\n-60\n-70\n-80\n-90\n-100\n-110\n-120");
    lv_slider_set_range(objects.rssi_slider, -125, -25);
    lv_label_set_text(objects.signal_scanner_snr_scale_label,
                      "8.0\n6.0\n4.0\n2.0\n0.0\n-2.0\n-4.0\n-8.0\n-10.0\n-12.0\n-14.0\n-16.0\n-18.0");
    lv_obj_set_style_text_line_space(objects.signal_scanner_snr_scale_label, -2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_slider_set_range(objects.snr_slider, -20, 9);
#endif

    setInputButtonLabel();
    lv_group_focus_obj(objects.home_button);

    // remember position of top node panel button for group linked list
    lv_ll_t *lv_group_ll = &lv_group_get_default()->obj_ll;
    for (lv_obj_t **obj_i = (lv_obj_t **)_lv_ll_get_head(lv_group_ll); obj_i != NULL;
         obj_i = (lv_obj_t **)_lv_ll_get_next(lv_group_ll, obj_i)) {
        if (*obj_i == objects.node_button) {
            topNodeLL = obj_i;
            break;
        }
    }

    // user data
    objects.home_time_button->user_data = (void *)0;
    objects.home_wlan_button->user_data = (void *)0;
    objects.home_memory_button->user_data = (void *)0;

    updateFreeMem();

    screensInitialised = true;
    ILOG_DEBUG("TFTView_320x240 init done.");
}

/**
 * @brief set active button, panel and top panel
 *
 * @param b button to set active
 * @param p main panel to set active
 * @param tp top panel to set active
 */
void TFTView_320x240::ui_set_active(lv_obj_t *b, lv_obj_t *p, lv_obj_t *tp)
{
    if (activeButton) {
        lv_obj_set_style_border_width(activeButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        if (Themes::get() == Themes::eDark)
            lv_obj_set_style_bg_img_recolor_opa(activeButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_img_recolor(activeButton, colorGray, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_obj_set_style_border_width(b, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor(b, colorMesh, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(b, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    if (activePanel) {
        lv_obj_add_flag(activePanel, LV_OBJ_FLAG_HIDDEN);
        if (activePanel == objects.messages_panel) {
            lv_obj_remove_state(objects.message_input_area, LV_STATE_FOCUSED);
            unreadMessages = 0; // TODO: not all messages may be actually read
            updateUnreadMessages();
        } else if (activePanel == objects.node_options_panel) {
            // we're moving away from node options panel, so save latest settings
            storeNodeOptions();
        }
    }

    lv_obj_clear_flag(p, LV_OBJ_FLAG_HIDDEN);

    if (tp) {
        if (activeTopPanel) {
            lv_obj_add_flag(activeTopPanel, LV_OBJ_FLAG_HIDDEN);
        }
        lv_obj_clear_flag(tp, LV_OBJ_FLAG_HIDDEN);
        activeTopPanel = tp;
    }

    activeButton = b;
    activePanel = p;
    if (activePanel == objects.messages_panel) {
        lv_group_focus_obj(objects.message_input_area);
    } else if (inputdriver->hasKeyboardDevice() || inputdriver->hasEncoderDevice()) {
        setGroupFocus(activePanel);
    }

    lv_obj_add_flag(objects.keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.msg_popup_panel, LV_OBJ_FLAG_HIDDEN);
}

void TFTView_320x240::enterProgrammingMode(void)
{
    if (state == eEnterProgrammingMode && !db.config.bluetooth.enabled) {
        ILOG_INFO("rebooting into programming mode");
        lv_label_set_text(objects.meshtastic_url, _("Rebooting ..."));

        if (ownNode) {
            meshtastic_Config_BluetoothConfig &bluetooth = THIS->db.config.bluetooth;
            bluetooth.mode = meshtastic_Config_BluetoothConfig_PairingMode_FIXED_PIN;
            bluetooth.fixed_pin = random(100000, 999999);
            bluetooth.enabled = true;
            THIS->controller->sendConfig(meshtastic_Config_BluetoothConfig{bluetooth}, ownNode);
            state = MeshtasticView::eWaitingForReboot;
        }
    } else {
        state = MeshtasticView::eProgrammingMode;
        lv_label_set_text(objects.meshtastic_url, _(">> Programming mode <<"));
        lv_label_set_text_fmt(objects.firmware_label, "%06d", db.config.bluetooth.fixed_pin);
        lv_obj_set_style_text_font(objects.firmware_label, &ui_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_flag(objects.boot_logo_button, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(objects.bluetooth_button, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_event_cb(objects.bluetooth_button, ui_event_BluetoothButton, LV_EVENT_LONG_PRESSED, NULL);
        ILOG_INFO("### MUI programming mode entered (nodeId=!%08x) ###", ownNode);
    }
    //    lv_obj_set_style_bg_color(objects.boot_screen, lv_color_hex(0x00), LV_PART_MAIN | LV_STATE_DEFAULT);
    //    lv_obj_set_style_image_recolor_opa(objects.boot_logo_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //    lv_obj_set_style_image_recolor(objects.boot_logo_button, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_event_cb(objects.boot_logo_button, ui_event_LogoButton);
    //    lv_obj_set_style_text_color(objects.meshtastic_url, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
}

/**
 * @brief fix quirks in the generated ui
 *
 */
void TFTView_320x240::apply_hotfix(void)
{
    // adapt screens to custom display resolution
    uint32_t h = lv_display_get_horizontal_resolution(displaydriver->getDisplay());
    uint32_t v = lv_display_get_vertical_resolution(displaydriver->getDisplay());

    // resize buttons on larger display (assuming 480x480)
    if (h > 320 && v > 320) {
        lv_obj_t *button[] = {objects.home_button,     objects.nodes_button, objects.groups_button,
                              objects.messages_button, objects.map_button,   objects.settings_button};
        for (int i = 0; i < 6; i++) {
            lv_obj_set_size(button[i], 72, 72);
        }
    }

    // keyboard size limit
    if (v >= 480) {
        lv_obj_set_size(objects.keyboard, LV_PCT(100), LV_PCT(45));

        if (h == 480) {
            lv_img_set_zoom(objects.world_image, 460);
        }
    }

    lv_obj_move_foreground(objects.keyboard);
    lv_obj_add_flag(objects.detector_radar_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.detected_node_button, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(objects.detector_start_label, _("Start"));
    lv_obj_clear_flag(objects.detector_start_button_panel, LV_OBJ_FLAG_HIDDEN);

    lv_textarea_set_placeholder_text(objects.message_input_area, _("Enter Text ..."));
    lv_textarea_set_placeholder_text(objects.nodes_filter_name_area, _("!Enter Filter ..."));
    lv_textarea_set_placeholder_text(objects.nodes_hl_name_area, _("Enter Filter ..."));

    auto applyStyle = [](lv_obj_t *tab_buttons) {
        for (int i = 0; i < lv_obj_get_child_count(tab_buttons); i++) {
            if (tab_buttons->spec_attr->children[i]->class_p == &lv_button_class) {
                lv_obj_add_style(tab_buttons->spec_attr->children[i], &style_btn_default, LV_STATE_DEFAULT);
                lv_obj_add_style(tab_buttons->spec_attr->children[i], &style_btn_active, LV_STATE_CHECKED);
                lv_obj_add_style(tab_buttons->spec_attr->children[i], &style_btn_pressed, LV_STATE_PRESSED);
            }
        }
    };

    lv_obj_t *tab_buttons = lv_tabview_get_tab_bar(objects.node_options_tab_view);
    applyStyle(tab_buttons);
    tab_buttons = lv_tabview_get_tab_bar(objects.controller_tab_view);
    applyStyle(tab_buttons);
    tab_buttons = lv_tabview_get_tab_bar(ui_SettingsTabView);
    applyStyle(tab_buttons);

    // add event callback to to apply custom drawing for statistics table
    lv_obj_add_event_cb(objects.statistics_table, ui_event_statistics_table, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(objects.statistics_table, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    // statistics table item size
    int32_t width = 36;
    int32_t rows = 12;
    if (h > 320) {
        width = (h - 48 - 57) / 6;
    }
    if (v > 240) {
        rows = (v - 32) / 18;
    }
    statisticTableRows = rows;
    lv_table_set_row_count(objects.statistics_table, statisticTableRows);
    lv_table_set_column_count(objects.statistics_table, 7);
    lv_table_set_column_width(objects.statistics_table, 0, 57);
    lv_table_set_column_width(objects.statistics_table, 1, width);
    lv_table_set_column_width(objects.statistics_table, 2, width);
    lv_table_set_column_width(objects.statistics_table, 3, width);
    lv_table_set_column_width(objects.statistics_table, 4, width);
    lv_table_set_column_width(objects.statistics_table, 5, width);
    lv_table_set_column_width(objects.statistics_table, 6, width);
    // fill table heading
    lv_table_set_cell_value(objects.statistics_table, 0, 0, _("Name"));
    lv_table_set_cell_value(objects.statistics_table, 0, 1, "Tel");
    lv_table_set_cell_value(objects.statistics_table, 0, 2, "Pos");
    lv_table_set_cell_value(objects.statistics_table, 0, 3, "Inf");
    lv_table_set_cell_value(objects.statistics_table, 0, 4, "Trc");
    lv_table_set_cell_value(objects.statistics_table, 0, 5, "Nbr");
    lv_table_set_cell_value(objects.statistics_table, 0, 6, "All");
}

void TFTView_320x240::updateTheme(void)
{
    Themes::initStyles();
    Themes::recolorButton(objects.home_lora_button, db.config.lora.tx_enabled);
    Themes::recolorButton(objects.home_bell_button, db.uiConfig.alert_enabled || !db.silent);
    Themes::recolorButton(objects.home_location_button,
                          db.config.position.gps_mode == meshtastic_Config_PositionConfig_GpsMode_ENABLED);
    Themes::recolorButton(objects.home_wlan_button, db.config.network.wifi_enabled);
    Themes::recolorButton(objects.home_mqtt_button, db.module_config.mqtt.enabled);
    Themes::recolorButton(objects.home_memory_button, (bool)objects.home_memory_button->user_data);
    Themes::recolorText(objects.home_lora_label, db.config.lora.tx_enabled);
    Themes::recolorText(objects.home_bell_label, db.uiConfig.alert_enabled || !db.silent);
    Themes::recolorText(objects.home_location_label,
                        db.config.position.gps_mode == meshtastic_Config_PositionConfig_GpsMode_ENABLED);
    Themes::recolorText(objects.home_wlan_label, db.config.network.wifi_enabled);
    Themes::recolorText(objects.home_mqtt_label, db.module_config.mqtt.enabled);
    Themes::recolorText(objects.home_memory_label, (bool)objects.home_memory_button->user_data);

    lv_opa_t opa = (Themes::get() == Themes::eDark) ? 0 : 255;
    lv_obj_set_style_bg_img_recolor_opa(objects.home_button, opa, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(objects.nodes_button, opa, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(objects.groups_button, opa, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(objects.messages_button, opa, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(objects.map_button, opa, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(objects.settings_button, opa, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void TFTView_320x240::ui_events_init(void)
{
    // just a test to implement callback via non-static lambda function
    auto ui_event_HomeButton = [](lv_event_t *e) {
        lv_event_code_t event_code = lv_event_get_code(e);
        if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
            TFTView_320x240 &view = *static_cast<TFTView_320x240 *>(e->user_data);
            view.ui_set_active(objects.home_button, objects.home_panel, objects.top_panel);
        } else if (event_code == LV_EVENT_LONG_PRESSED) {
            // force re-sync with node
            THIS->controller->setConfigRequested(true);
            THIS->notifyResync(true);
        }
    };

    // main button events
    lv_obj_add_event_cb(objects.home_button, ui_event_HomeButton, LV_EVENT_ALL, this); // uses lambda above
    lv_obj_add_event_cb(objects.nodes_button, this->ui_event_NodesButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.groups_button, this->ui_event_GroupsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.messages_button, this->ui_event_MessagesButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.map_button, this->ui_event_MapButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.settings_button, this->ui_event_SettingsButton, LV_EVENT_ALL, NULL);

    // home buttons
    lv_obj_add_event_cb(objects.home_mail_button, this->ui_event_EnvelopeButton, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.home_nodes_button, this->ui_event_OnlineNodesButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.home_time_button, this->ui_event_TimeButton, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.home_lora_button, this->ui_event_LoRaButton, LV_EVENT_LONG_PRESSED, NULL);
    lv_obj_add_event_cb(objects.home_bell_button, this->ui_event_BellButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.home_location_button, this->ui_event_LocationButton, LV_EVENT_LONG_PRESSED, NULL);
    lv_obj_add_event_cb(objects.home_wlan_button, this->ui_event_WLANButton, LV_EVENT_LONG_PRESSED, NULL);
    lv_obj_add_event_cb(objects.home_mqtt_button, this->ui_event_MQTTButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.home_memory_button, this->ui_event_MemoryButton, LV_EVENT_CLICKED, NULL);

    // node and channel buttons
    lv_obj_add_event_cb(objects.node_button, ui_event_NodeButton, LV_EVENT_ALL, (void *)ownNode);

    // 8 channel buttons
    lv_obj_add_event_cb(objects.channel_button0, ui_event_ChannelButton, LV_EVENT_ALL, (void *)0);
    lv_obj_add_event_cb(objects.channel_button1, ui_event_ChannelButton, LV_EVENT_ALL, (void *)1);
    lv_obj_add_event_cb(objects.channel_button2, ui_event_ChannelButton, LV_EVENT_ALL, (void *)2);
    lv_obj_add_event_cb(objects.channel_button3, ui_event_ChannelButton, LV_EVENT_ALL, (void *)3);
    lv_obj_add_event_cb(objects.channel_button4, ui_event_ChannelButton, LV_EVENT_ALL, (void *)4);
    lv_obj_add_event_cb(objects.channel_button5, ui_event_ChannelButton, LV_EVENT_ALL, (void *)5);
    lv_obj_add_event_cb(objects.channel_button6, ui_event_ChannelButton, LV_EVENT_ALL, (void *)6);
    lv_obj_add_event_cb(objects.channel_button7, ui_event_ChannelButton, LV_EVENT_ALL, (void *)7);

    // message popup
    lv_obj_add_event_cb(objects.msg_popup_button, this->ui_event_MsgPopupButton, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.msg_popup_panel, this->ui_event_MsgPopupButton, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.msg_restore_button, this->ui_event_MsgRestoreButton, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.msg_restore_panel, this->ui_event_MsgRestoreButton, LV_EVENT_CLICKED, NULL);

    // keyboard
    lv_obj_add_event_cb(objects.keyboard, ui_event_Keyboard, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(objects.keyboard_button_0, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)0);
    lv_obj_add_event_cb(objects.keyboard_button_1, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)1);
    lv_obj_add_event_cb(objects.keyboard_button_2, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)2);
    lv_obj_add_event_cb(objects.keyboard_button_3, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)3);
    lv_obj_add_event_cb(objects.keyboard_button_4, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)4);
    lv_obj_add_event_cb(objects.keyboard_button_5, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)5);
    lv_obj_add_event_cb(objects.keyboard_button_6, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)6);
    lv_obj_add_event_cb(objects.keyboard_button_7, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)7);
    lv_obj_add_event_cb(objects.keyboard_button_8, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)8);
    lv_obj_add_event_cb(objects.keyboard_button_9, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)9);
    lv_obj_add_event_cb(objects.keyboard_button_10, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)10);
    lv_obj_add_event_cb(objects.keyboard_button_11, ui_event_KeyboardButton, LV_EVENT_CLICKED, (void *)11);

    // message text area
    lv_obj_add_event_cb(objects.message_input_area, ui_event_message_ready, LV_EVENT_ALL, NULL);

    // basic settings buttons
    lv_obj_add_event_cb(objects.basic_settings_user_button, ui_event_user_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_role_button, ui_event_role_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_region_button, ui_event_region_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_modem_preset_button, ui_event_preset_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_wifi_button, ui_event_wifi_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_language_button, ui_event_language_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_channel_button, ui_event_channel_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_timeout_button, ui_event_timeout_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_screen_lock_button, ui_event_screen_lock_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_brightness_button, ui_event_brightness_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_theme_button, ui_event_theme_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_calibration_button, ui_event_calibration_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_input_button, ui_event_input_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_alert_button, ui_event_alert_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_reset_button, ui_event_reset_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.basic_settings_reboot_button, ui_event_reboot_button, LV_EVENT_CLICKED, NULL);

    lv_obj_add_event_cb(objects.reboot_button, ui_event_device_reboot_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.shutdown_button, ui_event_device_shutdown_button, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.cancel_reboot_button, ui_event_device_cancel_button, LV_EVENT_CLICKED, NULL);

    // sliders
    lv_obj_add_event_cb(objects.screen_timeout_slider, ui_event_screen_timeout_slider, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(objects.brightness_slider, ui_event_brightness_slider, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(objects.frequency_slot_slider, ui_event_frequency_slot_slider, LV_EVENT_VALUE_CHANGED, NULL);

    // dropdown
    lv_obj_add_event_cb(objects.settings_modem_preset_dropdown, ui_event_modem_preset_dropdown, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(objects.setup_region_dropdown, ui_event_setup_region_dropdown, LV_EVENT_VALUE_CHANGED, NULL);

    // OK / Cancel widget for basic settings dialog
    lv_obj_add_event_cb(objects.obj2__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj2__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj3__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj3__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj4__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj4__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj5__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj5__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj6__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj6__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj7__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj7__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj8__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj8__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj9__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj9__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj10__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj10__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj11__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj11__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj12__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj12__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj13__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj13__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj14__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj14__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj15__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj15__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj16__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj16__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj19__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj19__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj25__ok_button_w, ui_event_ok, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.obj25__cancel_button_w, ui_event_cancel, LV_EVENT_CLICKED, 0);

    // modify channel buttons
    lv_obj_add_event_cb(objects.settings_channel0_button, ui_event_modify_channel, LV_EVENT_ALL, (void *)0);
    lv_obj_add_event_cb(objects.settings_channel1_button, ui_event_modify_channel, LV_EVENT_ALL, (void *)1);
    lv_obj_add_event_cb(objects.settings_channel2_button, ui_event_modify_channel, LV_EVENT_ALL, (void *)2);
    lv_obj_add_event_cb(objects.settings_channel3_button, ui_event_modify_channel, LV_EVENT_ALL, (void *)3);
    lv_obj_add_event_cb(objects.settings_channel4_button, ui_event_modify_channel, LV_EVENT_ALL, (void *)4);
    lv_obj_add_event_cb(objects.settings_channel5_button, ui_event_modify_channel, LV_EVENT_ALL, (void *)5);
    lv_obj_add_event_cb(objects.settings_channel6_button, ui_event_modify_channel, LV_EVENT_ALL, (void *)6);
    lv_obj_add_event_cb(objects.settings_channel7_button, ui_event_modify_channel, LV_EVENT_ALL, (void *)7);
    // delete channel button
    lv_obj_add_event_cb(objects.settings_modify_trash_button, ui_event_delete_channel, LV_EVENT_CLICKED, NULL);
    // generate PSK
    lv_obj_add_event_cb(objects.settings_modify_channel_key_generate_button, ui_event_generate_psk, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.settings_modify_channel_qr_button, ui_event_qr_code, LV_EVENT_CLICKED, NULL);

    // screen
    lv_obj_add_event_cb(objects.calibration_screen, ui_event_calibration_screen_loaded, LV_EVENT_SCREEN_LOADED, (void *)7);
    lv_obj_add_event_cb(objects.screen_lock_button_matrix, ui_event_pin_screen_button, LV_EVENT_ALL, 0);

    // tools buttons
    lv_obj_add_event_cb(objects.tools_mesh_detector_button, ui_event_mesh_detector, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.tools_signal_scanner_button, ui_event_signal_scanner, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.tools_trace_route_button, ui_event_trace_route, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.tools_neighbors_button, ui_event_node_details, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.tools_statistics_button, ui_event_statistics, LV_EVENT_ALL, 0);
    lv_obj_add_event_cb(objects.tools_packet_log_button, ui_event_packet_log, LV_EVENT_ALL, 0);
    // tools
    lv_obj_add_event_cb(objects.detector_start_button, ui_event_mesh_detector_start, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.signal_scanner_node_button, ui_event_signal_scanner_node, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.signal_scanner_start_button, ui_event_signal_scanner_start, LV_EVENT_ALL, 0);
    lv_obj_add_event_cb(objects.trace_route_to_button, ui_event_trace_route_to, LV_EVENT_CLICKED, 0);
    lv_obj_add_event_cb(objects.trace_route_start_button, ui_event_trace_route_start, LV_EVENT_CLICKED, 0);
}

#if 0 // defined above as lambda function for tests
void TDeckGUI::ui_event_HomeButton(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        TDeckGUI::instance()->ui_set_active(objects.home_button, objects.home_panel, objects.top_panel);
    }
}
#endif

void TFTView_320x240::ui_event_LogoButton(lv_event_t *e)
{

    static uint32_t start = 0;
    static lv_anim_t anim;
    static auto animCB = [](void *var, int32_t v) { lv_arc_set_bg_end_angle((lv_obj_t *)var, v); };

    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        lv_anim_del(&objects.boot_logo_arc, animCB);
        lv_obj_add_flag(objects.boot_logo_arc, LV_OBJ_FLAG_HIDDEN);
        if (millis() - start > 800) {
            THIS->state = MeshtasticView::eEnterProgrammingMode;
            THIS->enterProgrammingMode();
        } else {
            THIS->state = MeshtasticView::eBooting;
        }
    } else if (event_code == LV_EVENT_LONG_PRESSED) {
        if (THIS->state != MeshtasticView::eHoldingBootLogo) {
            THIS->state = MeshtasticView::eHoldingBootLogo;
            start = millis();

            lv_obj_clear_flag(objects.boot_logo_arc, LV_OBJ_FLAG_HIDDEN);
            lv_anim_init(&anim);
            lv_anim_set_var(&anim, objects.boot_logo_arc);
            lv_anim_set_values(&anim, 0, 360);
            lv_anim_set_duration(&anim, 800);
            lv_anim_set_exec_cb(&anim, animCB);
            lv_anim_set_path_cb(&anim, lv_anim_path_linear);
            lv_anim_start(&anim);
        }
    }
}

void TFTView_320x240::ui_event_BluetoothButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_LONG_PRESSED) {
        ILOG_INFO("leaving programming mode");
        lv_label_set_text(objects.meshtastic_url, _("Rebooting ..."));
        lv_label_set_text(objects.firmware_label, "");
        lv_obj_remove_flag(objects.boot_logo_button, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(objects.bluetooth_button, LV_OBJ_FLAG_HIDDEN);

        meshtastic_Config_BluetoothConfig &bluetooth = THIS->db.config.bluetooth;
        bluetooth.enabled = false;
        THIS->controller->sendConfig(meshtastic_Config_BluetoothConfig{bluetooth}, THIS->ownNode);
    }
}

void TFTView_320x240::ui_event_NodesButton(lv_event_t *e)
{
    static bool ignoreClicked = false;
    static bool filterNeedsUpdate = false;
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        if (ignoreClicked) { // prevent long press to enter this setting
            ignoreClicked = false;
            return;
        }
        THIS->ui_set_active(objects.nodes_button, objects.nodes_panel, objects.top_nodes_panel);
        if (filterNeedsUpdate) {
            THIS->updateNodesFiltered(true);
            THIS->updateNodesStatus();
            filterNeedsUpdate = false;
        }
    } else if (event_code == LV_EVENT_LONG_PRESSED) {
        filterNeedsUpdate = true;
        ignoreClicked = true;
        THIS->ui_set_active(objects.nodes_button, objects.node_options_panel, objects.top_node_options_panel);
    }
}

void TFTView_320x240::ui_event_NodeButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        uint32_t nodeNum = (unsigned long)e->user_data;
        if (!nodeNum) // event-handler for own node has value 0 in user_data
            nodeNum = THIS->ownNode;
        lv_obj_t *panel = THIS->nodes[nodeNum];
        if (currentPanel) {
            // create animation to shrink other panel
            lv_anim_t a;
            int32_t height = lv_obj_get_height(currentPanel);
            lv_anim_init(&a);
            lv_anim_set_var(&a, currentPanel);
            lv_anim_set_values(&a, height, 136 - height);
            lv_anim_set_duration(&a, 200);
            lv_anim_set_exec_cb(&a, ui_anim_node_panel_cb);
            lv_anim_set_path_cb(&a, lv_anim_path_linear);
            lv_anim_start(&a);
        }
        if (panel != currentPanel) {
            // create animation to enlarge node panel
            lv_anim_t a;
            int32_t height = lv_obj_get_height(panel);
            lv_anim_init(&a);
            lv_anim_set_var(&a, panel);
            lv_anim_set_values(&a, height, 136 - height);
            lv_anim_set_duration(&a, 200);
            lv_anim_set_exec_cb(&a, ui_anim_node_panel_cb);
            lv_anim_set_path_cb(&a, lv_anim_path_linear);
            lv_anim_start(&a);
            currentPanel = panel;
            currentNode = nodeNum;
        } else {
            currentPanel = nullptr;
            currentNode = 0;
        }
        if (THIS->chooseNodeSignalScanner) {
            THIS->chooseNodeSignalScanner = false;
            ui_event_signal_scanner(NULL);
            // restore previous filter
            lv_dropdown_set_selected(objects.nodes_filter_hops_dropdown, THIS->selectedHops);
            THIS->updateNodesFiltered(true);
            THIS->updateNodesStatus();
        } else if (THIS->chooseNodeTraceRoute) {
            THIS->chooseNodeTraceRoute = false;
            ui_event_trace_route(NULL);
        }
    } else if (event_code == LV_EVENT_LONG_PRESSED) {
        //  set color and text of clicked node
        uint32_t nodeNum = (unsigned long)e->user_data;
        if (nodeNum != THIS->ownNode)
            THIS->showMessages(nodeNum);
    }
}

void TFTView_320x240::ui_event_GroupsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        THIS->ui_set_active(objects.groups_button, objects.groups_panel, objects.top_groups_panel);
    }
}

void TFTView_320x240::ui_event_ChannelButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        uint8_t ch = (uint8_t)(unsigned long)e->user_data;
        if (THIS->db.channel[ch].role != meshtastic_Channel_Role_DISABLED) {
            if (THIS->messagesRestored) {
                THIS->showMessages(ch);
            } else {
                lv_obj_clear_flag(objects.msg_restore_panel, LV_OBJ_FLAG_HIDDEN);
                lv_group_focus_obj(objects.msg_restore_button);
            }
        }
    } else {
        // TODO: click on unset channel should popup config screen
    }
}

void TFTView_320x240::ui_event_MessagesButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        if (THIS->messagesRestored) {
            THIS->ui_set_active(objects.messages_button, objects.chats_panel, objects.top_chats_panel);
        } else {
            lv_obj_clear_flag(objects.msg_restore_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.msg_restore_button);
        }
    }
}

void TFTView_320x240::ui_event_MapButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        THIS->ui_set_active(objects.map_button, objects.map_panel, objects.top_map_panel);
    }
}

void TFTView_320x240::ui_event_SettingsButton(lv_event_t *e)
{
    static bool ignoreClicked = false;
    lv_event_code_t event_code = lv_event_get_code(e);
    static bool advancedMode = false;
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        if (ignoreClicked) { // prevent long press to enter this setting
            ignoreClicked = false;
            return;
        }
        if (THIS->db.uiConfig.settings_lock) {
            lv_obj_add_flag(objects.tab_page_basic_settings, LV_OBJ_FLAG_HIDDEN);
            THIS->ui_set_active(objects.settings_button, objects.controller_panel, objects.top_settings_panel);
            lv_screen_load_anim(objects.lock_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
        } else {
            THIS->ui_set_active(objects.settings_button, objects.controller_panel, objects.top_settings_panel);
        }
    } else if (event_code == LV_EVENT_LONG_PRESSED && !advancedMode && THIS->activeSettings == eNone) {
        ILOG_DEBUG("screen locked");
        screenLocked = true;
        screenUnlockRequest = false;
        ignoreClicked = true;
    } else if (event_code == LV_EVENT_LONG_PRESSED && advancedMode && THIS->activeSettings == eNone) {
        advancedMode = !advancedMode;
        THIS->ui_set_active(objects.settings_button, ui_AdvancedSettingsPanel, objects.top_advanced_settings_panel);
    }
}

void TFTView_320x240::ui_event_ChatButton(lv_event_t *e)
{
    static bool ignoreClicked = false;
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target_obj(e);
    if (event_code == LV_EVENT_LONG_PRESSED) {
        ignoreClicked = true;
        lv_obj_t *delBtn = target->LV_OBJ_IDX(1);
        lv_obj_clear_flag(delBtn, LV_OBJ_FLAG_HIDDEN);
    } else if (event_code == LV_EVENT_DEFOCUSED || event_code == LV_EVENT_LEAVE) {
        lv_obj_t *delBtn = target->LV_OBJ_IDX(1);
        lv_obj_add_flag(delBtn, LV_OBJ_FLAG_HIDDEN);
    } else if (event_code == LV_EVENT_CLICKED) {
        if (ignoreClicked) { // prevent long press to enter this setting
            ignoreClicked = false;
            return;
        }
        lv_obj_set_style_border_color(target, colorMidGray, LV_PART_MAIN | LV_STATE_DEFAULT);

        uint32_t channelOrNode = (unsigned long)e->user_data;
        if (channelOrNode < c_max_channels) {
            uint8_t ch = (uint8_t)channelOrNode;
            THIS->showMessages(ch);
            THIS->ui_set_active(objects.messages_button, objects.messages_panel, objects.top_group_chat_panel);
        } else {
            uint32_t nodeNum = channelOrNode;
            THIS->showMessages(nodeNum);
            THIS->ui_set_active(objects.messages_button, objects.messages_panel, objects.top_messages_panel);
        }
    }
}

/**
 * @brief Del button pressed, handle deletion or clearance of chat and messages panel
 *
 * @param e
 */
void TFTView_320x240::ui_event_ChatDelButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        lv_obj_t *target = lv_event_get_target_obj(e);
        lv_obj_add_flag(target, LV_OBJ_FLAG_HIDDEN);

        uint32_t channelOrNode = (unsigned long)e->user_data;
        if (channelOrNode < c_max_channels) {
            THIS->eraseChat(channelOrNode);
            THIS->controller->removeTextMessages(THIS->ownNode, UINT32_MAX, channelOrNode);
        } else {
            THIS->eraseChat(channelOrNode);
            THIS->applyNodesFilter(channelOrNode);
            THIS->controller->removeTextMessages(THIS->ownNode, channelOrNode, 0);
        }
        THIS->activeMsgContainer = objects.messages_container;
        THIS->updateActiveChats();
        if (THIS->chats.empty()) {
            // last chat was deleted, now we can get rid of all logs :)
            THIS->controller->removeTextMessages(0, 0, 0);
        }
    }
}

/**
 * @brief hide msgPopupPanel on touch; goto message on button press
 *
 */
void TFTView_320x240::ui_event_MsgPopupButton(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target_obj(e);
    lv_event_code_t event_code = lv_event_get_code(e);

    if (target == objects.msg_popup_panel) {
        THIS->hideMessagePopup();
    } else { // msg button was clicked
        uint32_t channelOrNode = (unsigned long)objects.msg_popup_button->user_data;
        if (channelOrNode < c_max_channels) {
            uint8_t ch = (uint8_t)channelOrNode;
            THIS->showMessages(ch);
        } else {
            uint32_t nodeNum = channelOrNode;
            THIS->showMessages(nodeNum);
        }
    }
}

/**
 * @brief hide msgRestorePanel on touch
 *
 */
void TFTView_320x240::ui_event_MsgRestoreButton(lv_event_t *e)
{
    lv_obj_add_flag(objects.msg_restore_panel, LV_OBJ_FLAG_HIDDEN);
}

void TFTView_320x240::ui_event_EnvelopeButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        if (!THIS->messagesRestored) {
            lv_obj_clear_flag(objects.msg_restore_panel, LV_OBJ_FLAG_HIDDEN);
            return;
        }
        if (THIS->configComplete)
            THIS->ui_set_active(objects.messages_button, objects.chats_panel, objects.top_chats_panel);
    }
}

void TFTView_320x240::ui_event_OnlineNodesButton(lv_event_t *e)
{
    static bool ignoreClicked = false;
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->configComplete) {
        if (ignoreClicked) { // prevent long press to enter this setting
            ignoreClicked = false;
            return;
        }
        lv_obj_set_state(objects.nodes_filter_offline_switch, LV_STATE_CHECKED, true);
        THIS->ui_set_active(objects.nodes_button, objects.nodes_panel, objects.top_nodes_panel);
        THIS->updateNodesFiltered(true);
    } else if (event_code == LV_EVENT_LONG_PRESSED) {
        // reset all filters
        lv_obj_set_state(objects.nodes_filter_unknown_switch, LV_STATE_CHECKED, false);
        lv_obj_set_state(objects.nodes_filter_offline_switch, LV_STATE_CHECKED, false);
        lv_obj_set_state(objects.nodes_filter_public_key_switch, LV_STATE_CHECKED, false);
        lv_obj_set_state(objects.nodes_filter_position_switch, LV_STATE_CHECKED, false);
        lv_dropdown_set_selected(objects.nodes_filter_channel_dropdown, 0);
        lv_dropdown_set_selected(objects.nodes_filter_hops_dropdown, 0);
        lv_textarea_set_text(objects.nodes_filter_name_area, "");
        THIS->ui_set_active(objects.nodes_button, objects.nodes_panel, objects.top_nodes_panel);
        THIS->updateNodesFiltered(true);
        THIS->storeNodeOptions();
        ignoreClicked = true;
    }
}

void TFTView_320x240::ui_event_TimeButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        // toggle date/time <-> uptime display
        uint32_t toggle = (unsigned long)objects.home_time_button->user_data;
        objects.home_time_button->user_data = (void *)(1 - toggle);
        THIS->updateTime();
    }
}

void TFTView_320x240::ui_event_LoRaButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_LONG_PRESSED && THIS->db.config.has_lora) {
        // toggle lora tx on/off
        meshtastic_Config_LoRaConfig &lora = THIS->db.config.lora;
        lora.tx_enabled = !lora.tx_enabled;
        THIS->controller->sendConfig(meshtastic_Config_LoRaConfig{lora});
        THIS->showLoRaFrequency(lora);
    }
}

void TFTView_320x240::ui_event_BellButton(lv_event_t *e)
{
    static bool ignoreClicked = false;
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->db.module_config.has_external_notification) {
        if (ignoreClicked) { // prevent long press to enter this setting
            ignoreClicked = false;
            return;
        }
        // set banner and sound on
        if (THIS->db.silent && (bool)objects.home_bell_button->user_data) {
            if (THIS->db.uiConfig.ring_tone_id == 0) {
                THIS->db.uiConfig.ring_tone_id = 1;
            }
            THIS->db.silent = false;
            THIS->db.uiConfig.alert_enabled = true;
            THIS->controller->sendConfig(ringtone[THIS->db.uiConfig.ring_tone_id].rtttl, THIS->ownNode);
            objects.home_bell_button->user_data = (void *)false;
        }
        // toggle sound only
        else if (THIS->db.uiConfig.alert_enabled && !THIS->db.silent) {
            if (THIS->db.uiConfig.ring_tone_id == 0) {
                THIS->db.uiConfig.ring_tone_id = 1;
            }
            THIS->db.uiConfig.alert_enabled = false;
            THIS->controller->sendConfig(ringtone[THIS->db.uiConfig.ring_tone_id].rtttl, THIS->ownNode);
        }
        // toggle banner only
        else if (!THIS->db.uiConfig.alert_enabled && !THIS->db.silent) {
            THIS->db.silent = true;
            THIS->db.uiConfig.alert_enabled = true;
            THIS->controller->sendConfig(ringtone[0].rtttl, THIS->ownNode);
        }
        // toggle banner & sound
        else {
            if (THIS->db.uiConfig.ring_tone_id == 0) {
                THIS->db.uiConfig.ring_tone_id = 1;
            }
            THIS->db.silent = false;
            THIS->db.uiConfig.alert_enabled = true;
            THIS->controller->sendConfig(ringtone[THIS->db.uiConfig.ring_tone_id].rtttl, THIS->ownNode);
        }
        THIS->setBellText(THIS->db.uiConfig.alert_enabled, !THIS->db.silent);
        THIS->controller->storeUIConfig(THIS->db.uiConfig);
    } else if (event_code == LV_EVENT_LONG_PRESSED) {
        ignoreClicked = true;
        if ((bool)objects.home_bell_button->user_data) {
            if (THIS->db.uiConfig.ring_tone_id == 0) {
                THIS->db.uiConfig.ring_tone_id = 1;
            }
            THIS->db.silent = false;
            THIS->db.uiConfig.alert_enabled = true;
            THIS->controller->sendConfig(ringtone[THIS->db.uiConfig.ring_tone_id].rtttl, THIS->ownNode);
            objects.home_bell_button->user_data = (void *)false;
        } else {
            THIS->db.silent = true;
            THIS->db.uiConfig.alert_enabled = false;
            THIS->controller->sendConfig(ringtone[0].rtttl, THIS->ownNode);
            objects.home_bell_button->user_data = (void *)true;
        }
        THIS->setBellText(THIS->db.uiConfig.alert_enabled, !THIS->db.silent);
        THIS->controller->storeUIConfig(THIS->db.uiConfig);
    }
}

void TFTView_320x240::ui_event_LocationButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED && THIS->configComplete) {
        // TODO: figure out if there is a way to enabled GPS without a reboot (ala triple-click)
        // over phone api and switch between enabled/disabled with short press
        // uint32_t toggle = (unsigned long)objects.home_location_button->user_data;
        // objects.home_location_button->user_data = (void *)(1 - toggle);
        // Themes::recolorButton(objects.home_location_button, toggle);
    } else if (event_code == LV_EVENT_LONG_PRESSED && THIS->configComplete) {
        // toggle GPS not_present <-> enabled
        uint32_t toggle = (unsigned long)objects.home_location_button->user_data;
        objects.home_location_button->user_data = (void *)(1 - toggle);

        meshtastic_Config_PositionConfig &position = THIS->db.config.position;
        if (position.gps_mode == meshtastic_Config_PositionConfig_GpsMode_NOT_PRESENT)
            position.gps_mode = meshtastic_Config_PositionConfig_GpsMode_ENABLED;
        else {
            position.gps_mode = meshtastic_Config_PositionConfig_GpsMode_NOT_PRESENT;
        }
        Themes::recolorButton(objects.home_location_button,
                              position.gps_mode == meshtastic_Config_PositionConfig_GpsMode_ENABLED);
        THIS->controller->sendConfig(meshtastic_Config_PositionConfig{position});
        THIS->notifyReboot(true);
    }
}

void TFTView_320x240::ui_event_WLANButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_LONG_PRESSED && THIS->db.config.has_network) {
        if ((THIS->db.config.network.wifi_ssid[0] == '\0' || THIS->db.config.network.wifi_psk[0] == '\0') &&
            !THIS->db.connectionStatus.wifi.status.is_connected &&
            !THIS->db.config.network.eth_enabled) { // TODO: this is a workaround for bug in portduino layer
            // open settings dialog
            lv_textarea_set_text(objects.settings_wifi_ssid_textarea, THIS->db.config.network.wifi_ssid);
            lv_textarea_set_text(objects.settings_wifi_password_textarea, THIS->db.config.network.wifi_psk);
            lv_obj_clear_flag(objects.settings_wifi_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.settings_wifi_ssid_textarea);
            THIS->disablePanel(objects.home_panel);
            THIS->activeSettings = eWifi;
        } else {
            // toggle WLAN on/off
            uint32_t toggle = (unsigned long)objects.home_wlan_button->user_data;
            objects.home_wlan_button->user_data = (void *)(1 - toggle);
            meshtastic_Config_NetworkConfig &network = THIS->db.config.network;
            network.wifi_enabled = !network.wifi_enabled;
            Themes::recolorButton(objects.home_wlan_button, network.wifi_enabled);
            THIS->controller->sendConfig(meshtastic_Config_NetworkConfig{network});
            THIS->notifyReboot(true);
        }
    }
}

void TFTView_320x240::ui_event_MQTTButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_LONG_PRESSED && THIS->configComplete) {
        // toggle MQTT on/off
        uint32_t toggle = (unsigned long)objects.home_mqtt_button->user_data;
        objects.home_mqtt_button->user_data = (void *)(1 - toggle);

        meshtastic_ModuleConfig_MQTTConfig &mqtt = THIS->db.module_config.mqtt;
        mqtt.enabled = !mqtt.enabled;
        Themes::recolorButton(objects.home_mqtt_button, mqtt.enabled);
        THIS->controller->sendConfig(meshtastic_ModuleConfig_MQTTConfig{mqtt});
        THIS->notifyReboot(true);
    }
}

void TFTView_320x240::ui_event_MemoryButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        // toggle memory display updates
        uint32_t toggle = (unsigned long)objects.home_memory_button->user_data;
        objects.home_memory_button->user_data = (void *)(1 - toggle);
        Themes::recolorButton(objects.home_memory_button, !toggle);
        Themes::recolorText(objects.home_memory_label, !toggle);
        if ((unsigned long)objects.home_memory_button->user_data) {
            THIS->updateFreeMem();
        }
    }
}

void TFTView_320x240::ui_event_BlankScreenButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        ILOG_DEBUG("screen unlocked by button");
        screenUnlockRequest = true;
    }
}

void TFTView_320x240::ui_event_KeyboardButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        uint32_t keyBtnIdx = (unsigned long)e->user_data;
        switch (keyBtnIdx) {
        case 0:
            THIS->showKeyboard(objects.message_input_area);
            lv_group_focus_obj(objects.message_input_area);
            break;
        case 1:
            THIS->showKeyboard(objects.settings_user_short_textarea);
            lv_group_focus_obj(objects.settings_user_short_textarea);
            break;
        case 2:
            THIS->showKeyboard(objects.settings_user_long_textarea);
            lv_group_focus_obj(objects.settings_user_long_textarea);
            break;
        case 3:
            THIS->showKeyboard(objects.settings_modify_channel_name_textarea);
            lv_group_focus_obj(objects.settings_modify_channel_name_textarea);
            break;
        case 4:
            THIS->showKeyboard(objects.settings_modify_channel_psk_textarea);
            lv_group_focus_obj(objects.settings_modify_channel_psk_textarea);
            break;
        case 5:
            THIS->showKeyboard(objects.nodes_filter_name_area);
            lv_group_focus_obj(objects.nodes_filter_name_area);
            break;
        case 6:
            THIS->showKeyboard(objects.nodes_hl_name_area);
            lv_group_focus_obj(objects.nodes_hl_name_area);
            break;
        case 7:
            THIS->showKeyboard(objects.settings_screen_lock_password_textarea);
            lv_group_focus_obj(objects.settings_screen_lock_password_textarea);
            break;
        case 8:
            THIS->showKeyboard(objects.settings_wifi_ssid_textarea);
            lv_group_focus_obj(objects.settings_wifi_ssid_textarea);
            break;
        case 9:
            THIS->showKeyboard(objects.settings_wifi_password_textarea);
            lv_group_focus_obj(objects.settings_wifi_password_textarea);
            break;
        case 10:
            THIS->showKeyboard(objects.setup_user_short_textarea);
            lv_group_focus_obj(objects.setup_user_short_textarea);
            break;
        case 11:
            THIS->showKeyboard(objects.setup_user_long_textarea);
            lv_group_focus_obj(objects.setup_user_long_textarea);
            break;
        default:
            ILOG_ERROR("missing keyboard <-> textarea assignment");
        }
        lv_obj_has_flag(objects.keyboard, LV_OBJ_FLAG_HIDDEN) ? lv_obj_remove_flag(objects.keyboard, LV_OBJ_FLAG_HIDDEN)
                                                              : lv_obj_add_flag(objects.keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

void TFTView_320x240::ui_event_Keyboard(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        lv_obj_t *kb = lv_event_get_target_obj(e);
        uint32_t btn_id = lv_keyboard_get_selected_button(kb);

        switch (btn_id) {
        case 22: { // enter (filtered out by one-liner text input area, so we replace it)
            lv_obj_t *ta = lv_keyboard_get_textarea(kb);
            lv_textarea_add_char(ta, ' ');
            lv_textarea_add_char(ta, CR_REPLACEMENT);
            break;
        }
        case 35: { // keyboard
            lv_keyboard_set_popovers(objects.keyboard, !lv_keyboard_get_popovers(kb));
            break;
        }
        case 36: { // left
            break;
        }
        case 38: { // right
            break;
        }
        case 39: { // checkmark
            lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
            break;
        }
        default:
            break;
            // const char *txt = lv_keyboard_get_button_text(kb, btn_id);
        }
    }
}

void TFTView_320x240::ui_event_message_ready(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_READY) {
        char *txt = (char *)lv_textarea_get_text(objects.message_input_area);
        uint32_t len = strlen(txt);
        if (len) {
            if (txt[len - 1] == ' ') { // use space+return combo to start new line in same message
                lv_textarea_add_char(objects.message_input_area, CR_REPLACEMENT);
            } else {
                THIS->handleAddMessage(txt);
                lv_textarea_set_text(objects.message_input_area, "");
            }
        }
    }
}

// basic settings buttons

void TFTView_320x240::ui_event_user_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        lv_textarea_set_text(objects.settings_user_short_textarea, THIS->db.short_name);
        lv_textarea_set_text(objects.settings_user_long_textarea, THIS->db.long_name);
        lv_obj_clear_flag(objects.settings_username_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_user_short_textarea);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eUsername;
    }
}

void TFTView_320x240::ui_event_role_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone && THIS->db.config.has_device) {
        lv_dropdown_set_selected(objects.settings_device_role_dropdown, THIS->role2val(THIS->db.config.device.role));
        lv_obj_clear_flag(objects.settings_device_role_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_device_role_dropdown);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eDeviceRole;
    }
}

void TFTView_320x240::ui_event_region_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone && THIS->db.config.has_lora) {
        lv_dropdown_set_selected(objects.settings_region_dropdown, THIS->db.config.lora.region - 1);
        lv_obj_clear_flag(objects.settings_region_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_region_dropdown);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eRegion;
    }
}

void TFTView_320x240::ui_event_preset_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone && THIS->db.config.has_lora) {
        THIS->activeSettings = eModemPreset;
        lv_dropdown_set_selected(objects.settings_modem_preset_dropdown, THIS->db.config.lora.modem_preset);

        char buf[40];
        sprintf(buf, _("FrequencySlot: %d (%g MHz)"), THIS->db.config.lora.channel_num,
                LoRaPresets::getRadioFreq(THIS->db.config.lora.region, THIS->db.config.lora.modem_preset,
                                          THIS->db.config.lora.channel_num));
        lv_label_set_text(objects.frequency_slot_label, buf);

        uint32_t numChannels = LoRaPresets::getNumChannels(THIS->db.config.lora.region, THIS->db.config.lora.modem_preset);
        lv_slider_set_range(objects.frequency_slot_slider, 1, numChannels);
        lv_slider_set_value(objects.frequency_slot_slider, THIS->db.config.lora.channel_num, LV_ANIM_OFF);

        lv_obj_clear_flag(objects.settings_modem_preset_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_modem_preset_dropdown);
        THIS->disablePanel(objects.controller_panel);
    }
}

void TFTView_320x240::ui_event_wifi_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->db.config.has_network && THIS->activeSettings == eNone) {
        lv_textarea_set_text(objects.settings_wifi_ssid_textarea, THIS->db.config.network.wifi_ssid);
        lv_textarea_set_text(objects.settings_wifi_password_textarea, THIS->db.config.network.wifi_psk);
        lv_obj_clear_flag(objects.settings_wifi_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_wifi_ssid_textarea);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eWifi;
    }
}

void TFTView_320x240::ui_event_language_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        lv_dropdown_set_selected(objects.settings_language_dropdown, THIS->language2val(THIS->db.uiConfig.language));
        lv_obj_clear_flag(objects.settings_language_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_language_dropdown);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eLanguage;
    }
}

void TFTView_320x240::ui_event_channel_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        // primary channel is not necessarily channel[0], setup ui with primary on top
        int pos = 1;
        for (int i = 0; i < c_max_channels; i++) {
            meshtastic_Channel &ch = THIS->db.channel[i];
            if (ch.has_settings && ch.role != meshtastic_Channel_Role_DISABLED) {
                const char *channelName = ch.settings.name;
                if (ch.settings.name[0] == '\0' && ch.settings.psk.size == 1 && ch.settings.psk.bytes[0] == 0x01) {
                    channelName = LoRaPresets::modemPresetToString(THIS->db.config.lora.modem_preset);
                }
                if (ch.role == meshtastic_Channel_Role_PRIMARY) {
                    THIS->ch_label[0]->user_data = (void *)i;
                    lv_label_set_text(THIS->ch_label[0], channelName);
                } else {
                    THIS->ch_label[pos]->user_data = (void *)i;
                    lv_label_set_text(THIS->ch_label[pos++], channelName);
                }
            }
        }
        for (int i = pos; i < c_max_channels; i++) {
            THIS->ch_label[i]->user_data = (void *)-1;
            lv_label_set_text(THIS->ch_label[i], _("<unset>"));
        }
        lv_obj_clear_flag(objects.settings_channel_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_channel0_button);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eChannel;

        // create scratch channels to store temporary changes until cancelled or applied
        THIS->channel_scratch = new meshtastic_Channel[c_max_channels];
        for (int i = 0; i < c_max_channels; i++) {
            THIS->channel_scratch[i] = THIS->db.channel[i];
        }
    }
}

void TFTView_320x240::ui_event_brightness_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        char buf[20];
        uint32_t brightness = round(THIS->db.uiConfig.screen_brightness * 100.0 / 255.0);
        lv_snprintf(buf, sizeof(buf), _("Brightness: %d%%"), brightness);
        lv_label_set_text(objects.settings_brightness_label, buf);
        lv_slider_set_value(objects.brightness_slider, brightness, LV_ANIM_OFF);
        lv_obj_clear_flag(objects.settings_brightness_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.brightness_slider);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eScreenBrightness;
    }
}

void TFTView_320x240::ui_event_theme_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        lv_dropdown_set_selected(objects.settings_theme_dropdown, THIS->db.uiConfig.theme);
        lv_obj_clear_flag(objects.settings_theme_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_theme_dropdown);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eTheme;
    }
}

void TFTView_320x240::ui_event_calibration_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        lv_screen_load_anim(objects.calibration_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    }
}

void TFTView_320x240::ui_event_timeout_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        uint32_t timeout = THIS->db.uiConfig.screen_timeout;
        char buf[32];
        if (timeout == 0)
            lv_snprintf(buf, sizeof(buf), _("Timeout: off"));
        else
            lv_snprintf(buf, sizeof(buf), _("Timeout: %ds"), timeout);
        lv_label_set_text(objects.settings_screen_timeout_label, buf);
        lv_obj_clear_flag(objects.settings_screen_timeout_panel, LV_OBJ_FLAG_HIDDEN);
        lv_slider_set_value(objects.screen_timeout_slider, timeout, LV_ANIM_OFF);
        lv_group_focus_obj(objects.screen_timeout_slider);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eScreenTimeout;
    }
}

void TFTView_320x240::ui_event_screen_lock_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        char buf[10];
        lv_snprintf(buf, 7, "%06d", THIS->db.uiConfig.pin_code);
        lv_textarea_set_text(objects.settings_screen_lock_password_textarea, buf);
        if (THIS->db.uiConfig.screen_lock) {
            lv_obj_add_state(objects.settings_screen_lock_switch, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(objects.settings_screen_lock_switch, LV_STATE_CHECKED);
        }
        if (THIS->db.uiConfig.settings_lock) {
            lv_obj_add_state(objects.settings_settings_lock_switch, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(objects.settings_settings_lock_switch, LV_STATE_CHECKED);
        }

        lv_obj_clear_flag(objects.settings_screen_lock_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_screen_lock_switch);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eScreenLock;
    }
}

void TFTView_320x240::ui_event_input_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        std::vector<std::string> ptr_events = THIS->inputdriver->getPointerDevices();
        std::string ptr_dropdown = _("none");
        for (std::string &s : ptr_events) {
            ptr_dropdown += '\n' + s;
        }
        lv_dropdown_set_options(objects.settings_mouse_input_dropdown, ptr_dropdown.c_str());
        std::string current_ptr = THIS->inputdriver->getCurrentPointerDevice();
        uint32_t ptrOption = lv_dropdown_get_option_index(objects.settings_mouse_input_dropdown, current_ptr.c_str());
        lv_dropdown_set_selected(objects.settings_mouse_input_dropdown, ptrOption);

        std::vector<std::string> kbd_events = THIS->inputdriver->getKeyboardDevices();
        std::string kbd_dropdown = _("none");
        for (std::string &s : kbd_events) {
            kbd_dropdown += '\n' + s;
        }
        lv_dropdown_set_options(objects.settings_keyboard_input_dropdown, kbd_dropdown.c_str());
        std::string current_kbd = THIS->inputdriver->getCurrentKeyboardDevice();
        uint32_t kbdOption = lv_dropdown_get_option_index(objects.settings_keyboard_input_dropdown, current_kbd.c_str());
        lv_dropdown_set_selected(objects.settings_keyboard_input_dropdown, kbdOption);

        lv_dropdown_get_selected_str(objects.settings_keyboard_input_dropdown, THIS->old_val1_scratch, sizeof(old_val1_scratch));
        lv_dropdown_get_selected_str(objects.settings_mouse_input_dropdown, THIS->old_val2_scratch, sizeof(old_val2_scratch));

        lv_obj_clear_flag(objects.settings_input_control_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_mouse_input_dropdown);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eInputControl;
    }
}

void TFTView_320x240::ui_event_alert_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone && THIS->db.module_config.has_external_notification) {
        bool alert_enabled = THIS->db.module_config.external_notification.alert_message_buzzer &&
                             THIS->db.module_config.external_notification.enabled && !THIS->db.silent;
        if (alert_enabled) {
            lv_obj_add_state(objects.settings_alert_buzzer_switch, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(objects.settings_alert_buzzer_switch, LV_STATE_CHECKED);
        }
        // populate dropdown
        if (lv_dropdown_get_option_count(objects.settings_ringtone_dropdown) <= 1) {
            for (int i = 2; i < numRingtones; i++) {
                lv_dropdown_add_option(objects.settings_ringtone_dropdown, ringtone[i].name, i);
            }
        }

        lv_dropdown_set_selected(objects.settings_ringtone_dropdown, THIS->db.uiConfig.ring_tone_id - 1);
        lv_obj_clear_flag(objects.settings_alert_buzzer_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_alert_buzzer_switch);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eAlertBuzzer;
    }
}

// configuration reset
void TFTView_320x240::ui_event_reset_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        lv_obj_clear_flag(objects.settings_reset_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_reset_dropdown);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eReset;
    }
}

// reboot / shutdown
void TFTView_320x240::ui_event_reboot_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eNone) {
        lv_obj_remove_flag(objects.boot_logo_button, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(objects.bluetooth_button, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(objects.boot_logo_arc, LV_OBJ_FLAG_HIDDEN);
        lv_screen_load_anim(objects.boot_screen, LV_SCR_LOAD_ANIM_FADE_IN, 1000, 0, false);
        lv_obj_clear_flag(objects.reboot_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.cancel_reboot_button);
        THIS->disablePanel(objects.controller_panel);
        THIS->activeSettings = eReboot;
    }
}

void TFTView_320x240::ui_event_device_reboot_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        THIS->controller->requestReboot(5, THIS->ownNode);
        lv_screen_load_anim(objects.blank_screen, LV_SCR_LOAD_ANIM_FADE_OUT, 4000, 1000, false);
        lv_obj_add_flag(objects.reboot_panel, LV_OBJ_FLAG_HIDDEN);
    }
}

void TFTView_320x240::ui_event_device_shutdown_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        THIS->controller->requestShutdown(5, THIS->ownNode);
        lv_screen_load_anim(objects.blank_screen, LV_SCR_LOAD_ANIM_FADE_OUT, 4000, 1000, false);
        lv_obj_add_flag(objects.reboot_panel, LV_OBJ_FLAG_HIDDEN);
    }
}

void TFTView_320x240::ui_event_device_cancel_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        lv_screen_load_anim(objects.main_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
        lv_obj_add_flag(objects.reboot_panel, LV_OBJ_FLAG_HIDDEN);
        THIS->enablePanel(objects.controller_panel);
        lv_group_focus_obj(objects.basic_settings_reboot_button);
        THIS->activeSettings = eNone;
    }
}

void TFTView_320x240::ui_event_modify_channel(lv_event_t *e)
{
    static bool ignoreClicked = false;
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && THIS->activeSettings == eChannel) {
        if (ignoreClicked) { // prevent long press to enter this setting
            ignoreClicked = false;
            return;
        }
        uint32_t btn_id = (unsigned long)e->user_data;
        int8_t ch = (signed long)THIS->ch_label[btn_id]->user_data;
        if (ch != -1) {
            meshtastic_ChannelSettings_psk_t &psk = THIS->channel_scratch[ch].settings.psk;
            std::string base64 = THIS->pskToBase64(psk);
            lv_textarea_set_text(objects.settings_modify_channel_psk_textarea, base64.c_str());
            lv_textarea_set_text(objects.settings_modify_channel_name_textarea, THIS->channel_scratch[ch].settings.name);
            objects.settings_modify_channel_name_textarea->user_data = (void *)btn_id;
        } else {
            for (int i = 0; i < c_max_channels; i++) {
                if (THIS->channel_scratch[i].role == meshtastic_Channel_Role_DISABLED) {
                    // the first created channel is PRIMARY
                    bool found = false;
                    for (int j = 0; j < c_max_channels; j++) {
                        if (THIS->channel_scratch[j].role == meshtastic_Channel_Role_PRIMARY) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        THIS->channel_scratch[i].role = meshtastic_Channel_Role_PRIMARY;
                        if (i == 0) {
                            btn_id = 0; // place on top
                        } else {
                            // FIXME: swap ids as in long press
                            ILOG_ERROR("node does not have primary channel!");
                        }
                    } else
                        THIS->channel_scratch[i].role = meshtastic_Channel_Role_SECONDARY;

                    lv_textarea_set_text(objects.settings_modify_channel_psk_textarea, "");
                    lv_textarea_set_text(objects.settings_modify_channel_name_textarea, "");
                    THIS->ch_label[btn_id]->user_data = (void *)i;
                    objects.settings_modify_channel_name_textarea->user_data = (void *)btn_id;
                    break;
                }
            }
        }

        THIS->disablePanel(objects.settings_channel_panel);
        lv_obj_clear_flag(objects.settings_modify_channel_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.settings_modify_channel_name_textarea);
        THIS->activeSettings = eModifyChannel;
    } else if (event_code == LV_EVENT_LONG_PRESSED && THIS->activeSettings == eChannel) {
        ignoreClicked = true;
        // make channel primary on long press; swap with current primary (role, id and name)
        uint8_t btn_id = (uint8_t)(unsigned long)e->user_data;
        int8_t ch = (signed long)THIS->ch_label[btn_id]->user_data;
        if (btn_id != 0 && ch != -1) {
            int32_t primary_id = (signed long)THIS->ch_label[0]->user_data;
            THIS->channel_scratch[primary_id].role = meshtastic_Channel_Role_SECONDARY;
            THIS->channel_scratch[ch].role = meshtastic_Channel_Role_PRIMARY;
            THIS->ch_label[0]->user_data = (void *)(uint32_t)ch;
            THIS->ch_label[btn_id]->user_data = (void *)primary_id;
            lv_label_set_text(THIS->ch_label[0], THIS->channel_scratch[ch].settings.name);
            lv_label_set_text(THIS->ch_label[btn_id], THIS->channel_scratch[primary_id].settings.name);
        }
    }
}

void TFTView_320x240::ui_event_generate_psk(lv_event_t *e)
{
    std::string base64 = lv_textarea_get_text(objects.settings_modify_channel_psk_textarea);
    if (base64.size() == 0 || THIS->qr) {
        meshtastic_ChannelSettings_psk_t psk{.size = 32};
        std::mt19937 generator(millis() + psk.bytes[7]); // Mersenne Twister number generator
        for (int i = 0; i < 8; i++) {
            int r = generator();
            memcpy(&psk.bytes[i * 4], &r, 4);
        }
        base64 = THIS->pskToBase64(psk);
        lv_textarea_set_text(objects.settings_modify_channel_psk_textarea, base64.c_str());
    }

    std::string base64Https = base64;
    for (char &c : base64Https) {
        if (c == '+')
            c = '-';
        else if (c == '/')
            c = '_';
        else if (c == '=')
            c = '\0'; // remove paddings at the end of the url
    }
    std::string qr = "https://meshtastic.org/e/#" + base64Https;
    lv_obj_remove_flag(objects.settings_modify_channel_qr_panel, LV_OBJ_FLAG_HIDDEN);
    THIS->qr = THIS->showQrCode(objects.settings_modify_channel_qr_panel, qr.c_str());
    lv_obj_add_state(objects.keyboard_button_3, LV_STATE_DISABLED);
    lv_obj_add_state(objects.keyboard_button_4, LV_STATE_DISABLED);
}

void TFTView_320x240::ui_event_qr_code(lv_event_t *e)
{
    lv_obj_remove_state(objects.keyboard_button_3, LV_STATE_DISABLED);
    lv_obj_remove_state(objects.keyboard_button_4, LV_STATE_DISABLED);
    lv_obj_add_flag(objects.settings_modify_channel_qr_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_delete(THIS->qr);
    THIS->qr = nullptr;
}

void TFTView_320x240::ui_event_delete_channel(lv_event_t *e)
{
    lv_textarea_set_text(objects.settings_modify_channel_psk_textarea, "");
    lv_textarea_set_text(objects.settings_modify_channel_name_textarea, "");
}

void TFTView_320x240::ui_event_calibration_screen_loaded(lv_event_t *e)
{
    uint16_t *parameters = (uint16_t *)THIS->db.uiConfig.calibration_data.bytes;
    memset(parameters, 0, 16); // clear all calibration data
    bool done = THIS->displaydriver->calibrate(parameters);
    THIS->db.uiConfig.calibration_data.size = 16;
    char buf[32];
    lv_snprintf(buf, sizeof(buf), _("Screen Calibration: %s"), done ? _("done") : _("default"));
    lv_label_set_text(objects.basic_settings_calibration_label, buf);
    lv_screen_load_anim(objects.main_screen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
    THIS->controller->storeUIConfig(THIS->db.uiConfig);
}

void TFTView_320x240::ui_event_pin_screen_button(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED && lv_scr_act() == objects.lock_screen) {
        static const char *hidden[7] = {"o o o o o o", "* o o o o o", "* * o o o o", "* * * o o o",
                                        "* * * * o o", "* * * * * o", "* * * * * *"};
        static char pinEntered[7]{};
        lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
        uint32_t id = lv_buttonmatrix_get_selected_button(obj);
        const char *key = lv_buttonmatrix_get_button_text(obj, id);
        switch (*key) {
        case 'X': {
            pinKeys = 0;
            lv_label_set_text(objects.lock_screen_digits_label, hidden[pinKeys]);
            if (!screenLocked) {
                lv_screen_load_anim(objects.main_screen, LV_SCR_LOAD_ANIM_FADE_IN, 100, 0, false);
            } else {
                // TODO: init screen saver
            }
            break;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            if (pinKeys < 6) {
                pinEntered[pinKeys++] = *key;
                lv_label_set_text(objects.lock_screen_digits_label, hidden[pinKeys]);

                char buf[10];
                lv_snprintf(buf, 7, "%06d", THIS->db.uiConfig.pin_code);
                if (pinKeys == 6 && strcmp(pinEntered, buf) == 0) {
                    // unlock screen
                    pinKeys = 0;
                    screenLocked = false;
                    lv_obj_clear_flag(objects.tab_page_basic_settings, LV_OBJ_FLAG_HIDDEN);
                    lv_screen_load_anim(objects.main_screen, LV_SCR_LOAD_ANIM_FADE_IN, 100, 0, false);
                    lv_label_set_text(objects.lock_screen_digits_label, hidden[pinKeys]);
                }
            }
            break;
        }
        case 'D': {
            if (pinKeys > 0) {
                pinEntered[--pinKeys] = '\0';
                lv_label_set_text(objects.lock_screen_digits_label, hidden[pinKeys]);
            }
            break;
        }
        default:
            break;
        }
    }
}

void TFTView_320x240::ui_event_mesh_detector(lv_event_t *e)
{
    THIS->ui_set_active(objects.settings_button, objects.mesh_detector_panel, objects.top_mesh_detector_panel);
}

void TFTView_320x240::ui_event_mesh_detector_start(_lv_event_t *e)
{
    lv_obj_add_flag(objects.detector_contact_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.detector_heard_label, LV_OBJ_FLAG_HIDDEN);
    if (!THIS->detectorRunning) {
        lv_label_set_text(objects.detector_start_label, _("Stop"));

        // create radar animation
        lv_anim_t &a = THIS->radar;
        lv_anim_init(&a);
        lv_anim_set_var(&a, objects.radar_beam);
        lv_anim_set_values(&a, 0, 3600);
        lv_anim_set_repeat_count(&a, 1800);
        lv_anim_set_duration(&a, 7200);
        lv_anim_set_exec_cb(&a, ui_anim_radar_cb);
        lv_anim_start(&a);
        lv_obj_clear_flag(objects.detector_radar_panel, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_label_set_text(objects.detector_start_label, _("Start"));
        lv_anim_del(&objects.radar_beam, ui_anim_radar_cb);
        lv_obj_add_flag(objects.detector_radar_panel, LV_OBJ_FLAG_HIDDEN);
    }
    THIS->detectorRunning = !THIS->detectorRunning;
    THIS->controller->sendPing();
}

void TFTView_320x240::ui_event_signal_scanner(lv_event_t *e)
{
    if (currentPanel) {
        THIS->setNodeImage(currentNode, (MeshtasticView::eRole)(unsigned long)currentPanel->LV_OBJ_IDX(node_img_idx)->user_data,
                           false, objects.signal_scanner_node_image);
        const char *lbs = lv_label_get_text(currentPanel->LV_OBJ_IDX(node_lbs_idx));
        lv_label_set_text(objects.signal_scanner_node_button_label, lbs);
        lv_obj_clear_state(objects.signal_scanner_start_button, LV_STATE_DISABLED);
    } else {
        lv_label_set_text(objects.signal_scanner_node_button_label, _("choose\nnode"));
        lv_obj_add_state(objects.signal_scanner_start_button, LV_STATE_DISABLED);
    }
    lv_label_set_text(objects.signal_scanner_start_label, _("Start"));
    THIS->ui_set_active(objects.settings_button, objects.signal_scanner_panel, objects.top_signal_scanner_panel);
}

void TFTView_320x240::ui_event_signal_scanner_node(lv_event_t *e)
{
    THIS->chooseNodeSignalScanner = true;
    THIS->selectedHops = lv_dropdown_get_selected(objects.nodes_filter_hops_dropdown);
    lv_dropdown_set_selected(objects.nodes_filter_hops_dropdown, 7); // 0 hops away
    THIS->ui_set_active(objects.nodes_button, objects.nodes_panel, objects.top_nodes_panel);
    THIS->updateNodesFiltered(true);
    THIS->updateNodesStatus();
}

void TFTView_320x240::ui_event_signal_scanner_start(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (currentNode) {
        static bool ignoreClicked = false;
        if (event_code == LV_EVENT_CLICKED) {
            if (ignoreClicked) {
                ignoreClicked = false;
                return;
            }
            if (spinnerButton) {
                lv_label_set_text(objects.signal_scanner_start_label, _("Start"));
                lv_obj_delete(spinnerButton);
                spinnerButton = nullptr;
                THIS->scans = 0;
            } else {
                THIS->scanSignal(0);
            }
        } else if (event_code == LV_EVENT_LONG_PRESSED) {
            ignoreClicked = true;
            lv_obj_t *obj = lv_spinner_create(objects.signal_scanner_panel);
            spinnerButton = obj;
            spinnerButton->user_data = (void *)objects.signal_scanner_panel;
            lv_spinner_set_anim_params(obj, 5000, 300);
            lv_obj_set_pos(obj, 0, -50);
            lv_obj_set_size(obj, 68, 68);
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_spinner_style(obj);
            lv_label_set_text(objects.signal_scanner_start_label, "30s");
            THIS->scans = 6 + 1;
        }
    }
}

void TFTView_320x240::ui_event_trace_route(lv_event_t *e)
{
    // show still old route setup while processing is ongoing
    time_t now;
    time(&now);
    if (spinnerButton && (now - startTime) < 30) {
        THIS->ui_set_active(objects.settings_button, objects.trace_route_panel, objects.top_trace_route_panel);
        return;
    }
    THIS->removeSpinner();

    // remove old route except first button and spinner panel
    ILOG_DEBUG("removing old route: %d %d %d", lv_obj_get_child_cnt(objects.trace_route_panel),
               lv_obj_get_child_cnt(objects.route_towards_panel), lv_obj_get_child_cnt(objects.route_back_panel));

    uint16_t children = lv_obj_get_child_cnt(objects.trace_route_panel) - 1;
    while (children > 1) {
        if (objects.trace_route_panel->spec_attr->children[children]->class_p == &lv_button_class) {
            lv_obj_delete(objects.trace_route_panel->spec_attr->children[children]);
        }
        children--;
    }

    // forward route
    children = lv_obj_get_child_cnt(objects.route_towards_panel);
    while (children > 0) {
        children--;
        if (objects.route_towards_panel->spec_attr->children[children]->class_p == &lv_button_class) {
            lv_obj_delete(objects.route_towards_panel->spec_attr->children[children]);
        }
    }

    // backward route
    children = lv_obj_get_child_cnt(objects.route_back_panel);
    while (children > 0) {
        children--;
        if (objects.route_back_panel->spec_attr->children[children]->class_p == &lv_button_class) {
            lv_obj_delete(objects.route_back_panel->spec_attr->children[children]);
        }
    }

    lv_obj_clear_flag(objects.start_button_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.hop_routes_panel, LV_OBJ_FLAG_HIDDEN);

    if (currentPanel) {
        THIS->setNodeImage(THIS->currentNode,
                           (MeshtasticView::eRole)(unsigned long)currentPanel->LV_OBJ_IDX(node_img_idx)->user_data, false,
                           objects.trace_route_to_image);
        const char *lbl = lv_label_get_text(currentPanel->LV_OBJ_IDX(node_lbl_idx));
        lv_label_set_text(objects.trace_route_to_button_label, lbl);
        lv_obj_clear_state(objects.trace_route_start_button, LV_STATE_DISABLED);
    } else {
        lv_label_set_text(objects.trace_route_to_button_label, _("choose target node"));
        lv_obj_add_state(objects.trace_route_start_button, LV_STATE_DISABLED);
    }
    lv_label_set_text(objects.trace_route_start_label, _("Start"));
    THIS->ui_set_active(objects.settings_button, objects.trace_route_panel, objects.top_trace_route_panel);
}

void TFTView_320x240::ui_event_trace_route_to(lv_event_t *e)
{
    if (!spinnerButton) {
        THIS->chooseNodeTraceRoute = true;
        THIS->ui_set_active(objects.nodes_button, objects.nodes_panel, objects.top_nodes_panel);
    }
}

void TFTView_320x240::ui_event_trace_route_start(lv_event_t *e)
{
    if (!spinnerButton) {
        if (currentPanel) {
            time(&startTime);
            lv_obj_t *obj = lv_spinner_create(objects.start_button_panel);
            spinnerButton = obj;
            lv_spinner_set_anim_params(obj, 5000, 300);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 68, 68);
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_spinner_style(obj);
            lv_label_set_text(objects.trace_route_start_label, "30s");

            // retrieve nodeNum from current node
            // FIXME: remove for loop
            for (auto &it : THIS->nodes) {
                if (it.second == currentPanel) {
                    uint32_t requestId;
                    uint32_t to = it.first;
                    uint8_t ch = (uint8_t)(unsigned long)currentPanel->user_data;
                    // trial: hoplimit optimization for direct messages
                    int8_t hopsAway = (signed long)THIS->nodes[to]->LV_OBJ_IDX(node_sig_idx)->user_data;
                    if (hopsAway < 0)
                        hopsAway = 5;
                    uint8_t hopLimit = (hopsAway < THIS->db.config.lora.hop_limit ? hopsAway + 1 : hopsAway);
                    requestId = THIS->requests.addRequest(to, ResponseHandler::TraceRouteRequest);
                    THIS->controller->traceRoute(to, ch, hopLimit, requestId);
                    break;
                }
            }
        }
    } else {
        // restart
        ui_event_trace_route(e);
    }
}

void TFTView_320x240::ui_event_trace_route_node(lv_event_t *e)
{
    // navigate to node in node list
    lv_obj_t *panel = (lv_obj_t *)e->user_data;
    THIS->ui_set_active(objects.nodes_button, objects.nodes_panel, objects.top_nodes_panel);
    lv_obj_scroll_to_view(panel, LV_ANIM_ON);
}

void TFTView_320x240::removeSpinner(void)
{
    if (spinnerButton) {
        lv_obj_delete(spinnerButton);
        spinnerButton = nullptr;
        startTime = 0;
    }
}

void TFTView_320x240::ui_event_node_details(lv_event_t *e)
{
    THIS->ui_set_active(objects.settings_button, objects.details_panel, objects.top_nodes_panel);
}

void TFTView_320x240::ui_event_statistics(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        THIS->ui_set_active(objects.settings_button, objects.tools_statistics_panel, objects.top_statistics_panel);
    } else if (event_code == LV_EVENT_LONG_PRESSED) {
        // clear statistics table
        THIS->updateStatistics(meshtastic_MeshPacket{.from = 0});
    }
}

void TFTView_320x240::ui_event_packet_log(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        THIS->ui_set_active(objects.settings_button, objects.tools_packet_log_panel, objects.top_packet_log_panel);
        THIS->packetLogEnabled = true;
    } else if (event_code == LV_EVENT_LONG_PRESSED) {
        THIS->packetCounter = 0;
        lv_obj_clean(objects.tools_packet_log_panel);
    }
}

void TFTView_320x240::packetDetected(const meshtastic_MeshPacket &p)
{
    uint32_t heard = 0;
    if (p.from != ownNode)
        heard = p.from;
    if (p.to != 0xffffffff && p.to != ownNode)
        heard = p.to;

    if (heard) {
        if (p.to == ownNode && p.decoded.portnum == meshtastic_PortNum_NODEINFO_APP) {
            // we finally sensed a two-way contact to us; stop the detector
            detectorRunning = false;
            lv_label_set_text(objects.detector_start_label, _("Start"));
            lv_anim_del(&objects.radar_beam, ui_anim_radar_cb);
            lv_obj_add_flag(objects.detector_radar_panel, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(objects.detector_heard_label, LV_OBJ_FLAG_HIDDEN);

            setNodeImage(p.from, (MeshtasticView::eRole)(unsigned long)nodes[p.from]->LV_OBJ_IDX(node_img_idx)->user_data, false,
                         objects.detector_contact_image);
            const char *lbl = lv_label_get_text(nodes[p.from]->LV_OBJ_IDX(node_lbl_idx));

            char from[5];
            char *userShort = (char *)&(nodes[p.from]->LV_OBJ_IDX(node_lbs_idx)->user_data);
            int pos = 0;
            while (pos < 4 && userShort[pos] != 0) {
                from[pos] = userShort[pos];
                pos++;
            }
            from[pos] = '\0';

            char buf[64];
            lv_snprintf(buf, 64, "%s(%04x)\n%s", from, p.from & 0xffff, lbl);
            lv_label_set_text(objects.detector_contact_label, buf);
            lv_obj_clear_flag(objects.detector_contact_button, LV_OBJ_FLAG_HIDDEN);
        } else {
            char buf[20];
            lv_snprintf(buf, 20, _("heard: !%08x"), heard);
            lv_label_set_text(objects.detector_heard_label, buf);
            lv_obj_clear_flag(objects.detector_heard_label, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void TFTView_320x240::writePacketLog(const meshtastic_MeshPacket &p)
{
    static std::unordered_map<uint16_t, const char *> name = {
        {0, "unknown"},        {1, "text message"},    {2, "remote hardware"}, {3, "position"},    {4, "node info"},
        {5, "routing"},        {6, "admin"},           {7, "text message"},    {8, "waypoint"},    {9, "audio"},
        {10, "sensor"},        {32, "reply"},          {33, "ip tunnel"},      {34, "paxcounter"}, {64, "serial"},
        {65, "store forward"}, {66, "range test"},     {67, "telemetry"},      {68, "ZPS"},        {69, "simulator"},
        {70, "tracert"},       {71, "neighbor info"},  {72, "atax"},           {73, "map report"}, {74, "power stress"},
        {256, "private"},      {257, "atax forwarder"}};

    // ignore admin packages initiated by us
    if (p.from == ownNode && p.decoded.portnum == meshtastic_PortNum_ADMIN_APP)
        return;

    // get actual time
    char timebuf[16];
    time_t curr_time;
#ifdef ARCH_PORTDUINO
    time(&curr_time);
#else
    curr_time = actTime;
#endif
    tm *curr_tm = localtime(&curr_time);
    if (VALID_TIME(curr_time)) {
        strftime(timebuf, 16, "%T", curr_tm);
    } else {
        strcpy(timebuf, "??:??:??");
    }

    // get node name from
    char from[5];
    char *userShort = (char *)&(nodes[p.from]->LV_OBJ_IDX(node_lbs_idx)->user_data);
    int pos = 0;
    while (pos < 4 && userShort[pos] != 0) {
        from[pos] = userShort[pos];
        pos++;
    }
    from[pos] = '\0';

    char buf[256];
    if (p.to == 0xffffffff)
        sprintf(buf, "%s: ch%d %s:%04x->all: %s", timebuf, p.channel, from, p.from & 0xffff,
                name[p.decoded.portnum]); // note: this may crash if there's a new portnum not in this map...
    else
        sprintf(buf, "%s: ch%d %s:%04x->%s%04x: %s", timebuf, p.channel, from, p.from & 0xffff, p.to == ownNode ? "*" : "",
                p.to & 0xffff, name[p.decoded.portnum]);

    if (p.decoded.portnum == meshtastic_PortNum_TELEMETRY_APP) {
        meshtastic_Telemetry telemetry;
        if (pb_decode_from_bytes(p.decoded.payload.bytes, p.decoded.payload.size, &meshtastic_Telemetry_msg, &telemetry)) {
            switch (telemetry.which_variant) {
            case meshtastic_Telemetry_device_metrics_tag: {
                if (p.from == ownNode)
                    return; // suppress (internal) battery level packets
                strcat(buf, " dev");
                break;
            }
            case meshtastic_Telemetry_environment_metrics_tag: {
                strcat(buf, " env");
                break;
            }
            case meshtastic_Telemetry_air_quality_metrics_tag: {
                strcat(buf, " air");
                break;
            }
            case meshtastic_Telemetry_power_metrics_tag: {
                strcat(buf, " pow");
                break;
            }
            case meshtastic_Telemetry_local_stats_tag: {
                strcat(buf, " dev"); // bug in firmware that this is local?
            }
            default:
                break;
            }
        }
    } else if (p.decoded.portnum == meshtastic_PortNum_TRACEROUTE_APP) {
        // print the recorded route and add from/to manually
        strcat(buf, "\n");
        int pos = strlen(buf);
        if (p.to == ownNode) {
            pos += snprintf(&buf[pos], 16, "%04x", ownNode & 0xffff);
        }

        meshtastic_RouteDiscovery route;
        if (pb_decode_from_bytes(p.decoded.payload.bytes, p.decoded.payload.size, &meshtastic_RouteDiscovery_msg, &route)) {
            for (int i = 0; i < route.route_count; i++) {
                uint32_t nodeNum = route.route[i];
                if (nodeNum != UINT32_MAX) {
                    pos += snprintf(&buf[pos], 16, "->%04x", nodeNum & 0xffff);
                } else {
                    strcat(buf, "->unk");
                    pos += 6;
                }
            }
            if (p.to == ownNode) {
                pos += snprintf(&buf[pos], 16, "->%04x", p.from & 0xffff);
            }
        }
    }

    if (packetCounter >= PACKET_LOGS_MAX) {
        // delete oldest entry
        lv_obj_del(objects.tools_packet_log_panel->spec_attr->children[0]);
    } else {
        packetCounter++;
        char top[24];
        sprintf(top, _("Packet Log: %d"), packetCounter);
        lv_label_set_text(objects.top_packet_log_label, top);
    }
    lv_obj_t *pLabel = lv_label_create(objects.tools_packet_log_panel);
    lv_obj_set_pos(pLabel, 0, 0);
    lv_obj_set_size(pLabel, LV_PCT(100), LV_SIZE_CONTENT);
    uint32_t bgColor, fgColor;
    std::tie(bgColor, fgColor) = nodeColor(p.from);
    lv_obj_set_style_bg_color(pLabel, lv_color_hex(bgColor), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(pLabel, lv_color_hex(fgColor), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(pLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(pLabel, buf);

    // auto-scroll if last item is visible
    if (lv_obj_get_scroll_bottom(objects.tools_packet_log_panel) < 20)
        lv_obj_scroll_to_view(pLabel, LV_ANIM_OFF);
}

void TFTView_320x240::updateStatistics(const meshtastic_MeshPacket &p)
{
    struct Stats {
        uint32_t id;
        uint16_t row;
        uint16_t tel;
        uint16_t pos;
        uint16_t inf;
        uint16_t trc;
        uint16_t txt;
        uint16_t nbr;
        uint32_t sum;

        bool operator==(const Stats &rhs) const { return id == rhs.id; }

        Stats &operator+=(const Stats &rhs)
        {
            this->tel += rhs.tel;
            this->pos += rhs.pos;
            this->inf += rhs.inf;
            this->trc += rhs.trc;
            this->txt += rhs.txt;
            this->nbr += rhs.nbr;
            this->sum += 1;
            return *this;
        }

        bool operator<(const Stats &rhs) const
        {
            return sum > rhs.sum; // sort reverse but skip equal values
        }
    };
    static std::list<Stats> stats;

    if (p.from == 0) {
        // clear table
        stats.clear();
        for (int i = 1; i < statisticTableRows; i++) {
            for (int j = 0; j < 7; j++) {
                lv_table_set_cell_value(objects.statistics_table, i, j, "");
            }
        }
        return;
    }

    // update statistic for node
    Stats stat = {p.from};
    switch (p.decoded.portnum) {
    case meshtastic_PortNum_TELEMETRY_APP: {
        meshtastic_Telemetry telemetry;
        if (pb_decode_from_bytes(p.decoded.payload.bytes, p.decoded.payload.size, &meshtastic_Telemetry_msg, &telemetry)) {
            if (telemetry.which_variant == meshtastic_Telemetry_device_metrics_tag) {
                if (p.from == ownNode)
                    return; // suppress (internal) battery level packets
            }
        }
        stat.tel++;
        break;
    }
    case meshtastic_PortNum_POSITION_APP: {
        stat.pos++;
        break;
    }
    case meshtastic_PortNum_NODEINFO_APP: {
        stat.inf++;
        break;
    }
    case meshtastic_PortNum_ROUTING_APP:
    case meshtastic_PortNum_TRACEROUTE_APP: {
        stat.trc++;
        break;
    }
    case meshtastic_PortNum_TEXT_MESSAGE_APP: {
        stat.txt++;
        break;
    }
    case meshtastic_PortNum_NEIGHBORINFO_APP: {
        stat.nbr++;
        break;
    }
    case meshtastic_PortNum_ADMIN_APP: {
        // ignore
        break;
    }
    default:
        ILOG_WARN("packet portnum in stats unhandled: %d", p.decoded.portnum);
        stat.sum++;
        return;
    }

    std::list<Stats>::iterator it = std::find(stats.begin(), stats.end(), stat);
    if (it == stats.end()) {
        stat.row = stats.size();
        stat.sum = 1;
        // TODO: stop if memory limit is reached
        stats.push_back(stat);
    } else {
        *it += stat;
    }

    stats.sort();

    // fill packet statistics table
    char buf[10];
    int row = 1;
    bool move = false;

    for (auto it2 : stats) {
        if (it2.id == p.from || move) {
            buf[0] = '\0';
            auto it = nodes.find(it2.id); // node may have been removed from nodes, so check if still there
            if (it != nodes.end() && it->second) {
                char *userData = (char *)&(it->second->LV_OBJ_IDX(node_lbs_idx)->user_data);
                if (userData) {
                    buf[0] = userData[0];
                    buf[1] = userData[1];
                    buf[2] = userData[2];
                    buf[3] = userData[3];
                    buf[4] = '\0';
                }
            }

            // if name is empty or using glyphs then replace with short id
            uint32_t width = lv_txt_get_width(buf, strlen(buf), &ui_font_montserrat_12, 0);
            if (!width) {
                sprintf(buf, "%04x", it2.id & 0xffff);
            }

            lv_table_set_cell_value(objects.statistics_table, row, 0, buf);
            sprintf(buf, "%d", it2.tel);
            lv_table_set_cell_value(objects.statistics_table, row, 1, buf);
            sprintf(buf, "%d", it2.pos);
            lv_table_set_cell_value(objects.statistics_table, row, 2, buf);
            sprintf(buf, "%d", it2.inf);
            lv_table_set_cell_value(objects.statistics_table, row, 3, buf);
            sprintf(buf, "%d", it2.trc);
            lv_table_set_cell_value(objects.statistics_table, row, 4, buf);
            sprintf(buf, "%d", it2.nbr);
            lv_table_set_cell_value(objects.statistics_table, row, 5, buf);
            sprintf(buf, "%d", it2.sum);
            lv_table_set_cell_value(objects.statistics_table, row, 6, buf);

            if (row != it2.row) {
                it2.row = row;
                move = true;
            } else {
                break;
            }
        }
        row++;
        if (row >= statisticTableRows) // fill rows till bottom of 320x240 display
            break;
    }
}

void TFTView_320x240::ui_event_statistics_table(lv_event_t *e)
{
    lv_draw_task_t *draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t *base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    // if the cells are drawn...
    if (base_dsc->part == LV_PART_ITEMS) {
        // make the texts in the first cell blueish
        lv_draw_fill_dsc_t *fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
        if (fill_draw_dsc) {
            uint32_t row = base_dsc->id1;
            if (row == 0) {
                fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), fill_draw_dsc->color, LV_OPA_20);
            }
            // make every 2nd row grayish
            else {
                Themes::recolorTableRow(fill_draw_dsc, row % 2 == 0);
            }
        }
    }
}

void TFTView_320x240::requestSetup(void)
{
    ui_set_active(objects.settings_button, objects.initial_setup_panel, objects.top_setup_panel);
    lv_dropdown_set_selected(objects.setup_region_dropdown, 0);
    lv_obj_clear_flag(objects.initial_setup_panel, LV_OBJ_FLAG_HIDDEN);
    lv_group_focus_obj(objects.setup_region_dropdown);
    THIS->disablePanel(objects.controller_panel);
    THIS->activeSettings = eSetup;
}

/**
 * update signal strength text and image for home screen
 */
void TFTView_320x240::updateSignalStrength(int32_t rssi, float snr)
{
    // remember time we last heard a node
    time(&lastHeard);

    if (rssi != 0 || snr != 0.0) {
        char buf[40];
        sprintf(buf, "SNR: %.1f\nRSSI: %" PRId32, snr, rssi);
        lv_label_set_text(objects.home_signal_label, buf);

        uint32_t pct = signalStrength2Percent(rssi, snr);
        sprintf(buf, "(%d%%)", pct);
        lv_label_set_text(objects.home_signal_pct_label, buf);
        if (pct > 80) {
            lv_obj_set_style_bg_image_src(objects.home_signal_button, &img_home_signal_button_image,
                                          LV_PART_MAIN | LV_STATE_DEFAULT);
        } else if (pct > 60) {
            lv_obj_set_style_bg_image_src(objects.home_signal_button, &img_home_strong_signal_image,
                                          LV_PART_MAIN | LV_STATE_DEFAULT);
        } else if (pct > 40) {
            lv_obj_set_style_bg_image_src(objects.home_signal_button, &img_home_good_signal_image,
                                          LV_PART_MAIN | LV_STATE_DEFAULT);
        } else if (pct > 20) {
            lv_obj_set_style_bg_image_src(objects.home_signal_button, &img_home_fair_signal_image,
                                          LV_PART_MAIN | LV_STATE_DEFAULT);
        } else if (pct > 1) {
            lv_obj_set_style_bg_image_src(objects.home_signal_button, &img_home_weak_signal_image,
                                          LV_PART_MAIN | LV_STATE_DEFAULT);
        } else {
            lv_obj_set_style_bg_image_src(objects.home_signal_button, &img_home_no_signal_image, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

/**
 * Translate proto role enum value to numerical position in dropdown menu
 */
uint32_t TFTView_320x240::role2val(meshtastic_Config_DeviceConfig_Role role)
{
    int offset = 0;
#ifndef USE_ROUTER_ROLE
    // skipping
    offset = -2;
#endif

    switch (role) {
    case meshtastic_Config_DeviceConfig_Role_CLIENT:
        return 0;
    case meshtastic_Config_DeviceConfig_Role_CLIENT_MUTE:
        return 1;
    case meshtastic_Config_DeviceConfig_Role_ROUTER:
        return 2;
    case meshtastic_Config_DeviceConfig_Role_REPEATER:
        return 3;
    case meshtastic_Config_DeviceConfig_Role_TRACKER:
        return 4 + offset;
    case meshtastic_Config_DeviceConfig_Role_SENSOR:
        return 5 + offset;
    case meshtastic_Config_DeviceConfig_Role_TAK:
        return 6 + offset;
    case meshtastic_Config_DeviceConfig_Role_CLIENT_HIDDEN:
        return 7 + offset;
    case meshtastic_Config_DeviceConfig_Role_LOST_AND_FOUND:
        return 8 + offset;
    case meshtastic_Config_DeviceConfig_Role_TAK_TRACKER:
        return 9 + offset;
    default:
        ILOG_ERROR("unsupported device role: %d", role);
        return 0;
    }
}

/**
 * Translate value from dropdown menu to role proto enum
 */
meshtastic_Config_DeviceConfig_Role TFTView_320x240::val2role(uint32_t val)
{
    int offset = 0;
#ifndef USE_ROUTER_ROLE
    // skipping
    offset = -2;
#endif
    switch (val) {
    case 0:
        return meshtastic_Config_DeviceConfig_Role_CLIENT;
    case 1:
        return meshtastic_Config_DeviceConfig_Role_CLIENT_MUTE;
    case 2:
        return meshtastic_Config_DeviceConfig_Role_ROUTER;
    case 3:
        return meshtastic_Config_DeviceConfig_Role_REPEATER;
    case 4:
        return meshtastic_Config_DeviceConfig_Role(meshtastic_Config_DeviceConfig_Role_TRACKER - offset);
    case 5:
        return meshtastic_Config_DeviceConfig_Role(meshtastic_Config_DeviceConfig_Role_SENSOR - offset);
    case 6:
        return meshtastic_Config_DeviceConfig_Role(meshtastic_Config_DeviceConfig_Role_TAK - offset);
    case 7:
        return meshtastic_Config_DeviceConfig_Role(meshtastic_Config_DeviceConfig_Role_CLIENT_HIDDEN - offset);
    case 8:
        return meshtastic_Config_DeviceConfig_Role(meshtastic_Config_DeviceConfig_Role_LOST_AND_FOUND - offset);
    case 9:
        return meshtastic_Config_DeviceConfig_Role(meshtastic_Config_DeviceConfig_Role_TAK_TRACKER - offset);
    default:
        ILOG_WARN("unknown role value: %d", val);
    }
    return meshtastic_Config_DeviceConfig_Role_CLIENT;
}

/**
 * Translate language proto enum value to (alphabetical) position in dropdown menu
 */
uint32_t TFTView_320x240::language2val(meshtastic_Language lang)
{
    switch (lang) {
    case meshtastic_Language_ENGLISH:
        return 0;
    case meshtastic_Language_FRENCH:
        return 4;
    case meshtastic_Language_GERMAN:
        return 1;
    case meshtastic_Language_ITALIAN:
        return 5;
    case meshtastic_Language_PORTUGUESE:
        return 9;
    case meshtastic_Language_SPANISH:
        return 3;
    case meshtastic_Language_SWEDISH:
        return 14;
    case meshtastic_Language_FINNISH:
        return 13;
    case meshtastic_Language_POLISH:
        return 8;
    case meshtastic_Language_TURKISH:
        return 15;
    case meshtastic_Language_SERBIAN:
        return 12;
    case meshtastic_Language_RUSSIAN:
        return 10;
    case meshtastic_Language_DUTCH:
        return 6;
    case meshtastic_Language_GREEK:
        return 2;
    case meshtastic_Language_NORWEGIAN:
        return 7;
    case meshtastic_Language_SLOVENIAN:
        return 11;
    case meshtastic_Language_SIMPLIFIED_CHINESE:
        return 16;
    case meshtastic_Language_TRADITIONAL_CHINESE:
        return 17;
    default:
        ILOG_WARN("unknown language uiconfig: %d", lang);
    }
    return 0;
}

/**
 * Translate value from dropdown menu to language proto enum
 */
meshtastic_Language TFTView_320x240::val2language(uint32_t val)
{
    switch (val) {
    case 0:
        return meshtastic_Language_ENGLISH;
    case 4:
        return meshtastic_Language_FRENCH;
    case 1:
        return meshtastic_Language_GERMAN;
    case 5:
        return meshtastic_Language_ITALIAN;
    case 9:
        return meshtastic_Language_PORTUGUESE;
    case 3:
        return meshtastic_Language_SPANISH;
    case 14:
        return meshtastic_Language_SWEDISH;
    case 13:
        return meshtastic_Language_FINNISH;
    case 8:
        return meshtastic_Language_POLISH;
    case 15:
        return meshtastic_Language_TURKISH;
    case 12:
        return meshtastic_Language_SERBIAN;
    case 10:
        return meshtastic_Language_RUSSIAN;
    case 6:
        return meshtastic_Language_DUTCH;
    case 2:
        return meshtastic_Language_GREEK;
    case 7:
        return meshtastic_Language_NORWEGIAN;
    case 11:
        return meshtastic_Language_SLOVENIAN;
    case 16:
        return meshtastic_Language_SIMPLIFIED_CHINESE;
    case 17:
        return meshtastic_Language_TRADITIONAL_CHINESE;
    default:
        ILOG_WARN("unknown language val: %d", val);
    }
    return meshtastic_Language_ENGLISH;
}

/**
 * @brief Set lv_i18n language
 */
void TFTView_320x240::setLocale(meshtastic_Language lang)
{
    const char *locale = "en_US.UTF-8";
    switch (lang) {
    case meshtastic_Language_ENGLISH:
        lv_i18n_set_locale("en");
        break;
    case meshtastic_Language_GERMAN:
        lv_i18n_set_locale("de");
        locale = "de_DE.UTF-8";
        break;
    case meshtastic_Language_SPANISH:
        lv_i18n_set_locale("es");
        locale = "es_ES.UTF-8";
        break;
    case meshtastic_Language_FRENCH:
        lv_i18n_set_locale("fr");
        locale = "fr_FR.UTF-8";
        break;
    case meshtastic_Language_ITALIAN:
        lv_i18n_set_locale("it");
        locale = "it_IT.UTF-8";
        break;
    case meshtastic_Language_PORTUGUESE:
        lv_i18n_set_locale("pt");
        locale = "pt_PT.UTF-8";
        break;
    case meshtastic_Language_SWEDISH:
        lv_i18n_set_locale("se");
        locale = "sv_SE.UTF-8";
        break;
    case meshtastic_Language_FINNISH:
        lv_i18n_set_locale("fi");
        locale = "fi_FI.UTF-8";
        break;
    case meshtastic_Language_POLISH:
        lv_i18n_set_locale("pl");
        locale = "pl_PL.UTF-8";
        break;
    case meshtastic_Language_TURKISH:
        lv_i18n_set_locale("tr");
        locale = "tr_TR.UTF-8";
        break;
    case meshtastic_Language_SERBIAN:
        lv_i18n_set_locale("sr");
        locale = "sr_RS.UTF-8";
        break;
    case meshtastic_Language_DUTCH:
        lv_i18n_set_locale("nl");
        locale = "nl_NL.UTF-8";
        break;
    case meshtastic_Language_RUSSIAN:
        lv_i18n_set_locale("ru");
        locale = "ru_RU.UTF-8";
        break;
    case meshtastic_Language_GREEK:
        lv_i18n_set_locale("el");
        locale = "el_GR.UTF-8";
        break;
    case meshtastic_Language_NORWEGIAN:
        lv_i18n_set_locale("no");
        locale = "no_NO.UTF-8";
        break;
    case meshtastic_Language_SLOVENIAN:
        lv_i18n_set_locale("sl");
        locale = "sl_SI.UTF-8";
        break;
    case meshtastic_Language_SIMPLIFIED_CHINESE:
        lv_i18n_set_locale("cn");
        locale = "zh_CN.UTF-8";
        break;
    case meshtastic_Language_TRADITIONAL_CHINESE:
        lv_i18n_set_locale("tw");
        locale = "zh_TW.UTF-8";
        break;
    default:
        ILOG_WARN("Language %d not implemented", lang);
        break;
    }

#ifdef ARCH_PORTDUINO
    // std::locale::global(std::locale(locale));
#endif
}

/**
 * @brief Set language (using dropdown strings)
 */
void TFTView_320x240::setLanguage(meshtastic_Language lang)
{
    char buf1[20], buf2[40];
    lv_dropdown_set_selected(objects.settings_language_dropdown, language2val(lang));
    lv_dropdown_get_selected_str(objects.settings_language_dropdown, buf1, sizeof(buf1));
    lv_snprintf(buf2, sizeof(buf2), _("Language: %s"), buf1);
    lv_label_set_text(objects.basic_settings_language_label, buf2);
}

/**
 * @brief Set timeout
 */
void TFTView_320x240::setTimeout(uint32_t timeout)
{
    char buf[32];
    if (timeout == 0)
        lv_snprintf(buf, sizeof(buf), _("Screen Timeout: off"));
    else
        lv_snprintf(buf, sizeof(buf), _("Screen Timeout: %ds"), timeout);
    lv_label_set_text(objects.basic_settings_timeout_label, buf);
    THIS->displaydriver->setScreenTimeout(timeout);
}

/**
 * @brief Set brightness
 */
void TFTView_320x240::setBrightness(uint32_t brightness)
{
    char buf[32];
    lv_snprintf(buf, sizeof(buf), _("Screen Brightness: %d%%"), uint16_t(round((brightness * 100) / 255.0)));
    lv_label_set_text(objects.basic_settings_brightness_label, buf);
    THIS->displaydriver->setBrightness((uint8_t)brightness);
}

/**
 * @brief Set theme to new value
 */
void TFTView_320x240::setTheme(uint32_t value)
{
    char buf1[30], buf2[30];
    lv_dropdown_set_selected(objects.settings_theme_dropdown, value);
    lv_dropdown_get_selected_str(objects.settings_theme_dropdown, buf1, sizeof(buf1));
    lv_snprintf(buf2, sizeof(buf2), _("Theme: %s"), buf1);
    lv_label_set_text(objects.basic_settings_theme_label, buf2);

    // change theme and redraw UI
    Themes::set(Themes::Theme(value));
    updateTheme();
}

/**
 * @brief Save all data from node options panel
 */
void TFTView_320x240::storeNodeOptions(void)
{
    // store node filter options
    meshtastic_NodeFilter &filter = db.uiConfig.node_filter;
    db.uiConfig.has_node_filter = true;
    filter.unknown_switch = lv_obj_has_state(objects.nodes_filter_unknown_switch, LV_STATE_CHECKED);
    filter.offline_switch = lv_obj_has_state(objects.nodes_filter_offline_switch, LV_STATE_CHECKED);
    filter.public_key_switch = lv_obj_has_state(objects.nodes_filter_public_key_switch, LV_STATE_CHECKED);
    // filter.channel = lv_dropdown_get_selected(objects.nodes_filter_channel_dropdown);
    filter.hops_away = lv_dropdown_get_selected(objects.nodes_filter_hops_dropdown);
    // filter.mqtt_switch = lv_obj_has_state(objects.nodes_filter_mqtt_switch, LV_STATE_CHECKED);
    filter.position_switch = lv_obj_has_state(objects.nodes_filter_position_switch, LV_STATE_CHECKED);
    strncpy(filter.node_name, lv_textarea_get_text(objects.nodes_filter_name_area), sizeof(filter.node_name));

    // store node highlight options
    meshtastic_NodeHighlight &highlight = db.uiConfig.node_highlight;
    db.uiConfig.has_node_highlight = true;
    highlight.chat_switch = lv_obj_has_state(objects.nodes_hl_active_chat_switch, LV_STATE_CHECKED);
    highlight.position_switch = lv_obj_has_state(objects.nodes_hl_position_switch, LV_STATE_CHECKED);
    highlight.telemetry_switch = lv_obj_has_state(objects.nodes_hl_telemetry_switch, LV_STATE_CHECKED);
    highlight.iaq_switch = lv_obj_has_state(objects.nodes_hliaq_switch, LV_STATE_CHECKED);
    strncpy(highlight.node_name, lv_textarea_get_text(objects.nodes_hl_name_area), sizeof(highlight.node_name));

    controller->storeUIConfig(db.uiConfig);
}

/**
 * @brief erase chat and all its resources
 */
void TFTView_320x240::eraseChat(uint32_t channelOrNode)
{
    if (channelOrNode < c_max_channels) {
        uint8_t ch = (uint8_t)channelOrNode;
        lv_obj_delete_delayed(chats[ch], 500);
        lv_obj_del(channelGroup[ch]);
        channelGroup[ch] = nullptr;
        chats.erase(ch);
    } else {
        uint32_t nodeNum = channelOrNode;
        lv_obj_delete_delayed(chats[nodeNum], 500);
        lv_obj_del(messages[nodeNum]);
        messages.erase(nodeNum);
        chats.erase(nodeNum);
    }
}

/**
 * @brief clears all (persistent) chat messages
 */
void TFTView_320x240::clearChatHistory(void)
{
    for (auto &it : chats) {
        lv_obj_delete(it.second);
        if (it.first < c_max_channels) {
            lv_obj_delete(channelGroup[it.first]);
            channelGroup[it.first] = nullptr;
        } else {
            lv_obj_delete(messages[it.first]);
        }
    }
    chats.clear();
    messages.clear();
    updateActiveChats();
    updateNodesFiltered(true);
    controller->removeTextMessages(0, 0, 0);
}

/**
 * @brief User widget OK button handling
 *
 * @param e
 */
void TFTView_320x240::ui_event_ok(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        switch (THIS->activeSettings) {
        case eSetup: {
            meshtastic_Config_LoRaConfig_RegionCode region =
                (meshtastic_Config_LoRaConfig_RegionCode)(lv_dropdown_get_selected(objects.setup_region_dropdown) + 1);

            uint32_t numChannels = LoRaPresets::getNumChannels(region, THIS->db.config.lora.modem_preset);
            // if (numChannels == 0) {
            //     // region not possible for selected preset, revert
            //     lv_dropdown_set_selected(objects.settings_region_dropdown, THIS->db.config.lora.region - 1);
            //     return;
            // }

            if (region != THIS->db.config.lora.region) {
                char buf1[10], buf2[30];
                lv_dropdown_get_selected_str(objects.setup_region_dropdown, buf1, sizeof(buf1));
                lv_snprintf(buf2, sizeof(buf2), _("Region: %s"), buf1);
                lv_label_set_text(objects.basic_settings_region_label, buf2);

                meshtastic_Config_LoRaConfig &lora = THIS->db.config.lora;
                uint32_t defaultSlot = LoRaPresets::getDefaultSlot(region, THIS->db.config.lora.modem_preset);
                lora.region = region;
                lora.channel_num = (defaultSlot <= numChannels ? defaultSlot : 1);
                THIS->controller->sendConfig(meshtastic_Config_LoRaConfig{lora}, THIS->ownNode);
            }

            char buf[30];
            const char *userShort = lv_textarea_get_text(objects.setup_user_short_textarea);
            const char *userLong = lv_textarea_get_text(objects.setup_user_long_textarea);
            if (strcmp(userShort, THIS->db.short_name) || strcmp(userLong, THIS->db.long_name)) {
                lv_snprintf(buf, sizeof(buf), _("User name: %s"), userShort);
                lv_label_set_text(objects.basic_settings_user_label, buf);
                lv_label_set_text(objects.user_name_short_label, userShort);
                lv_label_set_text(objects.user_name_label, userLong);
                strcpy(THIS->db.short_name, userShort);
                strcpy(THIS->db.long_name, userLong);
                meshtastic_User user{}; // TODO: don't overwrite is_licensed
                strcpy(user.short_name, userShort);
                strcpy(user.long_name, userLong);
                THIS->controller->sendConfig(user, THIS->ownNode);
            }
            THIS->notifyReboot(true);

            lv_obj_add_flag(objects.initial_setup_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.home_button);
            break;
        }
        case eUsername: {
            char buf[30];
            const char *userShort = lv_textarea_get_text(objects.settings_user_short_textarea);
            const char *userLong = lv_textarea_get_text(objects.settings_user_long_textarea);
            if (strcmp(userShort, THIS->db.short_name) || strcmp(userLong, THIS->db.long_name)) {
                lv_snprintf(buf, sizeof(buf), _("User name: %s"), userShort);
                lv_label_set_text(objects.basic_settings_user_label, buf);
                lv_label_set_text(objects.user_name_short_label, userShort);
                lv_label_set_text(objects.user_name_label, userLong);
                strcpy(THIS->db.short_name, userShort);
                strcpy(THIS->db.long_name, userLong);
                meshtastic_User user{}; // TODO: don't overwrite is_licensed
                strcpy(user.short_name, userShort);
                strcpy(user.long_name, userLong);
                THIS->controller->sendConfig(user, THIS->ownNode);
                THIS->notifyReboot(true);
            }
            lv_obj_add_flag(objects.settings_username_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_user_button);
            break;
        }
        case eDeviceRole: {
            meshtastic_Config_DeviceConfig &device = THIS->db.config.device;
            meshtastic_Config_DeviceConfig_Role role =
                THIS->val2role(lv_dropdown_get_selected(objects.settings_device_role_dropdown));

            if (role != device.role) {
                char buf1[30], buf2[40];
                lv_dropdown_get_selected_str(objects.settings_device_role_dropdown, buf1, sizeof(buf1));
                lv_snprintf(buf2, sizeof(buf2), _("Device Role: %s"), buf1);
                lv_label_set_text(objects.basic_settings_role_label, buf2);

                device.role = role;
                THIS->controller->sendConfig(meshtastic_Config_DeviceConfig{device}, THIS->ownNode);
                THIS->notifyReboot(true);
            }
            lv_obj_add_flag(objects.settings_device_role_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_role_button);
            break;
        }
        case eRegion: {
            meshtastic_Config_LoRaConfig_RegionCode region =
                (meshtastic_Config_LoRaConfig_RegionCode)(lv_dropdown_get_selected(objects.settings_region_dropdown) + 1);

            uint32_t numChannels = LoRaPresets::getNumChannels(region, THIS->db.config.lora.modem_preset);
            if (numChannels == 0) {
                // region not possible for selected preset, revert
                lv_dropdown_set_selected(objects.settings_region_dropdown, THIS->db.config.lora.region - 1);
                return;
            }

            if (region != THIS->db.config.lora.region) {
                char buf1[10], buf2[30];
                lv_dropdown_get_selected_str(objects.settings_region_dropdown, buf1, sizeof(buf1));
                lv_snprintf(buf2, sizeof(buf2), _("Region: %s"), buf1);
                lv_label_set_text(objects.basic_settings_region_label, buf2);

                meshtastic_Config_LoRaConfig &lora = THIS->db.config.lora;
                uint32_t defaultSlot = LoRaPresets::getDefaultSlot(region, THIS->db.config.lora.modem_preset);
                lora.region = region;
                lora.channel_num = (defaultSlot <= numChannels ? defaultSlot : 1);
                THIS->controller->sendConfig(meshtastic_Config_LoRaConfig{lora}, THIS->ownNode);
                THIS->notifyReboot(true);
            }
            lv_obj_add_flag(objects.settings_region_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_region_button);
            break;
        }
        case eModemPreset: {
            meshtastic_Config_LoRaConfig &lora = THIS->db.config.lora;
            meshtastic_Config_LoRaConfig_ModemPreset preset =
                (meshtastic_Config_LoRaConfig_ModemPreset)(lv_dropdown_get_selected(objects.settings_modem_preset_dropdown));
            uint16_t channelNum = lv_slider_get_value(objects.frequency_slot_slider);
            if (preset != lora.modem_preset || lora.channel_num != channelNum) {
                char buf1[16], buf2[32];
                lv_dropdown_get_selected_str(objects.settings_modem_preset_dropdown, buf1, sizeof(buf1));
                lv_snprintf(buf2, sizeof(buf2), _("Modem Preset: %s"), buf1);
                lv_label_set_text(objects.basic_settings_modem_preset_label, buf2);

                lora.use_preset = true;
                lora.modem_preset = preset;
                lora.channel_num = channelNum;
                THIS->controller->sendConfig(meshtastic_Config_LoRaConfig{lora}, THIS->ownNode);
                THIS->notifyReboot(true);
            }
            lv_obj_add_flag(objects.settings_modem_preset_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_modem_preset_button);
            break;
        }
        case eChannel: {
            for (int i = 0; i < c_max_channels; i++) {
                // check if channel changed, then update and send to radio
                if (memcmp(&THIS->db.channel[i], &THIS->channel_scratch[i], sizeof(THIS->channel_scratch[i])) != 0) {
                    THIS->channel_scratch[i].has_settings = true;
                    THIS->updateChannelConfig(THIS->channel_scratch[i]);
                    THIS->controller->sendConfig(THIS->channel_scratch[i], THIS->ownNode);
                }
            }

            int8_t ch = (signed long)THIS->ch_label[0]->user_data;
            THIS->setChannelName(THIS->db.channel[ch]);
            lv_obj_clear_state(objects.settings_channel_panel, LV_STATE_DISABLED);
            lv_obj_add_flag(objects.settings_channel_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_channel_button);
            delete[] THIS->channel_scratch;
            break;
        }
        case eWifi: {
            char buf[30];
            const char *ssid = lv_textarea_get_text(objects.settings_wifi_ssid_textarea);
            const char *psk = lv_textarea_get_text(objects.settings_wifi_password_textarea);
            lv_snprintf(buf, sizeof(buf), _("WiFi: %s"), ssid[0] ? ssid : _("<not set>"));
            lv_label_set_text(objects.basic_settings_wifi_label, buf);
            if (strcmp(THIS->db.config.network.wifi_ssid, ssid) != 0 || strcmp(THIS->db.config.network.wifi_psk, psk) != 0) {
                strcpy(THIS->db.config.network.wifi_ssid, ssid);
                strcpy(THIS->db.config.network.wifi_psk, psk);
                THIS->controller->sendConfig(meshtastic_Config_NetworkConfig{THIS->db.config.network}, THIS->ownNode);
                THIS->notifyReboot(true);
            }
            // THIS->enablePanel(objects.home_panel);
            lv_obj_add_flag(objects.settings_wifi_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_wifi_button);
            break;
        }
        case eLanguage: {
            uint32_t value = lv_dropdown_get_selected(objects.settings_language_dropdown);
            meshtastic_Language lang = THIS->val2language(value);
            if (lang != THIS->db.uiConfig.language) {
                THIS->db.uiConfig.language = lang;
                THIS->controller->storeUIConfig(THIS->db.uiConfig);
                THIS->controller->requestReboot(3, THIS->ownNode);
                THIS->notifyReboot(true);
            }

            lv_obj_add_flag(objects.settings_language_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_language_button);
            break;
        }
        case eScreenTimeout: {
            uint32_t value = lv_slider_get_value(objects.screen_timeout_slider);
            if (value > 5)
                value -= value % 5;
            if (value != THIS->db.uiConfig.screen_timeout) {
                THIS->setTimeout(value);
                THIS->db.uiConfig.screen_timeout = value;
                THIS->controller->storeUIConfig(THIS->db.uiConfig);
            }
            lv_obj_add_flag(objects.settings_screen_timeout_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_timeout_button);
            break;
        }
        case eScreenLock: {
            const char *pin = lv_textarea_get_text(objects.settings_screen_lock_password_textarea);
            bool screenLock = lv_obj_has_state(objects.settings_screen_lock_switch, LV_STATE_CHECKED);
            bool settingsLock = lv_obj_has_state(objects.settings_settings_lock_switch, LV_STATE_CHECKED);
            if ((screenLock || settingsLock) && (atol(pin) == 0 || strlen(pin) != 6))
                return; // require pin != "000000"
            if ((screenLock != THIS->db.uiConfig.screen_lock) || settingsLock != THIS->db.uiConfig.settings_lock ||
                atol(pin) != THIS->db.uiConfig.pin_code) {
                THIS->db.uiConfig.screen_lock = screenLock;
                THIS->db.uiConfig.settings_lock = settingsLock;
                THIS->db.uiConfig.pin_code = atol(pin);
                THIS->controller->storeUIConfig(THIS->db.uiConfig);
            }

            char buf[40];
            lv_snprintf(buf, 40, _("Lock: %s/%s"), screenLock ? _("on") : _("off"), settingsLock ? _("on") : _("off"));
            lv_label_set_text(objects.basic_settings_screen_lock_label, buf);
            lv_obj_add_flag(objects.settings_screen_lock_panel, LV_OBJ_FLAG_HIDDEN);

            break;
        }
        case eScreenBrightness: {
            int32_t value = lv_slider_get_value(objects.brightness_slider) * 255 / 100;
            if (value != THIS->db.uiConfig.screen_brightness) {
                THIS->setBrightness(value);
                THIS->db.uiConfig.screen_brightness = value;
                THIS->controller->storeUIConfig(THIS->db.uiConfig);
            }
            lv_obj_add_flag(objects.settings_brightness_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_brightness_button);
            break;
        }
        case eTheme: {
            uint32_t value = lv_dropdown_get_selected(objects.settings_theme_dropdown);
            if (value != THIS->db.uiConfig.theme) {
                THIS->setTheme(value);
                THIS->db.uiConfig.theme = meshtastic_Theme(value);
                THIS->controller->storeUIConfig(THIS->db.uiConfig);
                lv_obj_set_style_bg_img_recolor(objects.settings_button, colorMesh, LV_PART_MAIN | LV_STATE_DEFAULT);
            }

            lv_obj_add_flag(objects.settings_theme_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_theme_button);
            lv_obj_invalidate(objects.main_screen);
            break;
        }
        case eInputControl: {
            char new_val_kbd[10], new_val_ptr[10];
            lv_dropdown_get_selected_str(objects.settings_keyboard_input_dropdown, new_val_kbd, sizeof(new_val_kbd));
            lv_dropdown_get_selected_str(objects.settings_mouse_input_dropdown, new_val_ptr, sizeof(new_val_ptr));

            bool error = false;
            if (strcmp(THIS->old_val1_scratch, new_val_kbd) != 0) {
                if (strcmp(THIS->old_val1_scratch, _("none")) != 0) {
                    THIS->inputdriver->releaseKeyboardDevice();
                }
                if (strcmp(new_val_kbd, _("none")) != 0) {
                    error &= THIS->inputdriver->useKeyboardDevice(new_val_kbd);
                }
            }
            if (strcmp(THIS->old_val2_scratch, new_val_ptr) != 0) {
                if (strcmp(THIS->old_val2_scratch, _("none")) != 0) {
                    THIS->inputdriver->releasePointerDevice();
                }
                if (strcmp(new_val_ptr, _("none")) != 0) {
                    error &= THIS->inputdriver->usePointerDevice(new_val_ptr);
                }
            }

            THIS->setInputButtonLabel();

            if (error) {
                ILOG_WARN("failed to use %s/%s", new_val_kbd, new_val_ptr);
                return;
            }

            std::string current_kbd = THIS->inputdriver->getCurrentKeyboardDevice();
            std::string current_ptr = THIS->inputdriver->getCurrentPointerDevice();
            if (strcmp(current_kbd.c_str(), _("none")) == 0 && strcmp(current_ptr.c_str(), _("none")) == 0 && THIS->input_group) {
                lv_group_delete(THIS->input_group);
                THIS->input_group = nullptr;
            } else if (strcmp(THIS->old_val1_scratch, current_kbd.c_str()) != 0 ||
                       strcmp(THIS->old_val2_scratch, current_ptr.c_str()) != 0) {
                THIS->setInputGroup();
            }

            lv_obj_add_flag(objects.settings_input_control_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_input_button);
            break;
        }
        case eAlertBuzzer: {
            meshtastic_ModuleConfig_ExternalNotificationConfig &config = THIS->db.module_config.external_notification;
            int tone = lv_dropdown_get_selected(objects.settings_ringtone_dropdown) + 1;

            bool silent = false;
            bool alert_message = lv_obj_has_state(objects.settings_alert_buzzer_switch, LV_STATE_CHECKED);
            if ((!config.enabled || !config.alert_message_buzzer) && alert_message) {
                if (!config.enabled || !config.alert_message_buzzer || !config.use_pwm || !config.use_i2s_as_buzzer) {
                    config.enabled = true;
                    config.alert_message_buzzer = true;
                    config.use_pwm = true;
                    config.nag_timeout = 0;
#ifdef USE_I2S_BUZZER
                    config.use_i2s_as_buzzer = true;
                    config.use_pwm = false;
#endif
                }
                THIS->notifyReboot(true);
                THIS->controller->sendConfig(meshtastic_ModuleConfig_ExternalNotificationConfig{config}, THIS->ownNode);
            } else if (config.alert_message_buzzer && !alert_message) {
                silent = true;
            }

            THIS->controller->sendConfig(ringtone[silent ? 0 : tone].rtttl, THIS->ownNode);
            THIS->db.uiConfig.ring_tone_id = tone;
            THIS->db.silent = silent;
            THIS->db.uiConfig.alert_enabled = !silent;
            THIS->setBellText(THIS->db.uiConfig.alert_enabled, !silent);
            THIS->controller->storeUIConfig(THIS->db.uiConfig);

            lv_obj_add_flag(objects.settings_alert_buzzer_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_alert_button);
            break;
        }
        case eReset: {
            uint32_t option = lv_dropdown_get_selected(objects.settings_reset_dropdown);
            if (option == 2) {
                THIS->clearChatHistory();
            } else {
                THIS->notifyReboot(true);
                THIS->controller->requestReset(option, THIS->ownNode);
            }
            lv_obj_add_flag(objects.settings_reset_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_reset_button);
            break;
        }
        case eModifyChannel: {
            meshtastic_ChannelSettings_psk_t psk = {};
            const char *name = lv_textarea_get_text(objects.settings_modify_channel_name_textarea);
            const char *base64 = lv_textarea_get_text(objects.settings_modify_channel_psk_textarea);
            uint8_t btn_id = (unsigned long)objects.settings_modify_channel_name_textarea->user_data;
            int8_t ch = (signed long)THIS->ch_label[btn_id]->user_data;

            if (strlen(base64) == 0 && strlen(name) == 0) {
                // delete channel
                THIS->channel_scratch[ch].role = meshtastic_Channel_Role_DISABLED;
                THIS->channel_scratch[ch].settings.psk.size = 0;
                strcpy(THIS->channel_scratch[ch].settings.name, "");
                lv_label_set_text(THIS->ch_label[btn_id], _("<unset>"));
                THIS->activeSettings = eChannel;
            }

            int paddings = (4 - strlen(base64) % 4) % 4;
            while (paddings-- > 0) {
                lv_textarea_add_text(objects.settings_modify_channel_psk_textarea, "=");
            }

            if (THIS->base64ToPsk(lv_textarea_get_text(objects.settings_modify_channel_psk_textarea), psk)) {
                if (strlen(name) || psk.size) {
                    // TODO: fill temp storage -> user data
                    lv_label_set_text(THIS->ch_label[btn_id], name);
                    strcpy(THIS->channel_scratch[ch].settings.name, name);
                    memcpy(THIS->channel_scratch[ch].settings.psk.bytes, psk.bytes, 32);
                    THIS->channel_scratch[ch].settings.psk.size = psk.size;
                    THIS->activeSettings = eChannel;
                }
            }

            if (THIS->activeSettings == eChannel) {
                lv_obj_add_flag(objects.settings_modify_channel_panel, LV_OBJ_FLAG_HIDDEN);
                THIS->enablePanel(objects.settings_channel_panel);
                lv_group_focus_obj(objects.settings_channel0_button);
            }
            return;
        }
        default:
            ILOG_ERROR("Unhandled ok event");
            break;
        }
        THIS->enablePanel(objects.controller_panel);
        THIS->activeSettings = eNone;
    }
}

/**
 * @brief Cancel button user widget handling
 *
 * @param e
 */
void TFTView_320x240::ui_event_cancel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        switch (THIS->activeSettings) {
        case TFTView_320x240::eSetup: {
            THIS->ui_set_active(objects.home_button, objects.home_panel, objects.top_panel);
            // lv_obj_add_flag(objects.initial_setup_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.home_button);
            break;
        }
        case TFTView_320x240::eUsername: {
            lv_obj_add_flag(objects.settings_username_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_user_button);
            break;
        }
        case TFTView_320x240::eDeviceRole: {
            lv_obj_add_flag(objects.settings_device_role_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_role_button);
            break;
        }
        case TFTView_320x240::eRegion: {
            lv_obj_add_flag(objects.settings_region_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_region_button);
            break;
        }
        case TFTView_320x240::eModemPreset: {
            lv_obj_add_flag(objects.settings_modem_preset_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_modem_preset_button);
            break;
        }
        case TFTView_320x240::eChannel: {
            delete[] THIS->channel_scratch;
            lv_obj_add_flag(objects.settings_channel_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_channel_button);
            break;
        }
        case TFTView_320x240::eWifi: {
            lv_obj_add_flag(objects.settings_wifi_panel, LV_OBJ_FLAG_HIDDEN);
            // THIS->enablePanel(objects.home_panel);
            lv_group_focus_obj(objects.home_wlan_button);

            break;
        }
        case TFTView_320x240::eLanguage: {
            lv_obj_add_flag(objects.settings_language_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_language_button);
            break;
        }
        case TFTView_320x240::eScreenTimeout: {
            lv_obj_add_flag(objects.settings_screen_timeout_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_timeout_button);
            break;
        }
        case eScreenLock: {
            lv_obj_add_flag(objects.settings_screen_lock_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_screen_lock_button);
            break;
        }
        case TFTView_320x240::eScreenBrightness: {
            lv_obj_add_flag(objects.settings_brightness_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_brightness_button);
            // revert to old brightness value
            uint32_t old_brightness = THIS->db.uiConfig.screen_brightness;
            THIS->displaydriver->setBrightness((uint8_t)old_brightness);
            break;
        }
        case TFTView_320x240::eTheme: {
            lv_obj_add_flag(objects.settings_theme_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_theme_button);
            break;
        }
        case TFTView_320x240::eInputControl: {
            lv_obj_add_flag(objects.settings_input_control_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_input_button);
            break;
        }
        case TFTView_320x240::eAlertBuzzer: {
            lv_obj_add_flag(objects.settings_alert_buzzer_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_alert_button);
            break;
        }
        case TFTView_320x240::eReset: {
            lv_obj_add_flag(objects.settings_reset_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.basic_settings_reset_button);
            break;
        }
        case TFTView_320x240::eModifyChannel: {
            lv_obj_add_flag(objects.settings_modify_channel_panel, LV_OBJ_FLAG_HIDDEN);
            lv_group_focus_obj(objects.settings_channel0_button);
            THIS->enablePanel(objects.settings_channel_panel);
            THIS->activeSettings = eChannel;
            return;
        }
        default:
            ILOG_ERROR("Unhandled cancel event");
            break;
        }

        THIS->enablePanel(objects.controller_panel);
        THIS->activeSettings = eNone;
    }
}

// end button event handlers

void TFTView_320x240::ui_event_screen_timeout_slider(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target_obj(e);
    char buf[20];
    uint32_t value = lv_slider_get_value(slider);
    if (value > 5)
        value -= value % 5;
    if (value == 0)
        lv_snprintf(buf, sizeof(buf), _("Timeout: off"));
    else
        lv_snprintf(buf, sizeof(buf), _("Timeout: %ds"), value);
    lv_label_set_text(objects.settings_screen_timeout_label, buf);
}

void TFTView_320x240::ui_event_brightness_slider(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target_obj(e);
    char buf[20];
    lv_snprintf(buf, sizeof(buf), _("Brightness: %d%%"), (int)lv_slider_get_value(slider));
    lv_label_set_text(objects.settings_brightness_label, buf);
    THIS->displaydriver->setBrightness((uint8_t)(lv_slider_get_value(slider) * 255 / 100));
}

void TFTView_320x240::ui_event_frequency_slot_slider(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target_obj(e);
    char buf[40];
    uint32_t channel = (uint32_t)lv_slider_get_value(slider);
    sprintf(buf, _("FrequencySlot: %d (%g MHz)"), channel,
            LoRaPresets::getRadioFreq(
                THIS->db.config.lora.region,
                (meshtastic_Config_LoRaConfig_ModemPreset)lv_dropdown_get_selected(objects.settings_modem_preset_dropdown),
                channel));
    lv_label_set_text(objects.frequency_slot_label, buf);
}

void TFTView_320x240::ui_event_modem_preset_dropdown(lv_event_t *e)
{
    lv_obj_t *dropdown = lv_event_get_target_obj(e);
    meshtastic_Config_LoRaConfig_ModemPreset preset =
        (meshtastic_Config_LoRaConfig_ModemPreset)lv_dropdown_get_selected(dropdown);
    uint32_t numChannels = LoRaPresets::getNumChannels(THIS->db.config.lora.region, preset);
    if (preset == meshtastic_Config_LoRaConfig_ModemPreset_VERY_LONG_SLOW || numChannels == 0) {
        // preset deprecated or not possible for this region, revert
        lv_dropdown_set_selected(dropdown, THIS->db.config.lora.modem_preset);
        numChannels = LoRaPresets::getNumChannels(THIS->db.config.lora.region, THIS->db.config.lora.modem_preset);
        return;
    }

    uint32_t channel = LoRaPresets::getDefaultSlot(THIS->db.config.lora.region, preset);
    if (channel > numChannels)
        channel = 1;
    lv_slider_set_range(objects.frequency_slot_slider, 1, numChannels);
    lv_slider_set_value(objects.frequency_slot_slider, channel, LV_ANIM_ON);

    char buf[40];
    sprintf(buf, _("FrequencySlot: %d (%g MHz)"), channel,
            LoRaPresets::getRadioFreq(THIS->db.config.lora.region, preset, channel));
    lv_label_set_text(objects.frequency_slot_label, buf);
}

void TFTView_320x240::ui_event_setup_region_dropdown(lv_event_t *e) {}

// animations
void TFTView_320x240::ui_anim_node_panel_cb(void *var, int32_t v)
{
    lv_obj_set_height((lv_obj_t *)var, v);
}

void TFTView_320x240::ui_anim_radar_cb(void *var, int32_t r)
{
    lv_img_set_angle(objects.radar_beam, r);
}

/**
 * @brief Dynamically show user widget
 *        First a panel is created where the widget is located in, then the widget is drawn.
 *        "active_widget" contains the surrounding panel which must be destroyed
 *        to remove the widget from the screen (e.g. by pressing OK/Cancel).
 *
 * @param func
 */
void TFTView_320x240::showUserWidget(UserWidgetFunc createWidget)
{
    lv_obj_t *obj = lv_obj_create(objects.main_screen);
    lv_obj_set_pos(obj, 39, 25);
    lv_obj_set_size(obj, LV_PCT(88), LV_PCT(90));
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, colorDarkGray, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    activeWidget = obj;

    createWidget(activeWidget, NULL, 0);
}

void TFTView_320x240::handleAddMessage(char *msg)
{
    // retrieve nodeNum + channel from activeMsgContainer
    uint32_t to = UINT32_MAX;
    uint8_t ch = 0;
    uint8_t hopLimit = db.config.lora.hop_limit;
    uint32_t requestId;
    uint32_t channelOrNode = (unsigned long)activeMsgContainer->user_data;
    bool usePkc = false;

    auto callback = [this](const ResponseHandler::Request &req, ResponseHandler::EventType evt, int32_t pass) {
        this->onTextMessageCallback(req, evt, pass);
    };

    if (channelOrNode < c_max_channels) {
        ch = (uint8_t)channelOrNode;
        requestId = requests.addRequest(ch, ResponseHandler::TextMessageRequest, nullptr, callback);
    } else {
        ch = (uint8_t)(unsigned long)nodes[channelOrNode]->user_data;
        to = channelOrNode;
        usePkc = (unsigned long)nodes[to]->LV_OBJ_IDX(node_bat_idx)->user_data; // hasKey
        requestId = requests.addRequest(to, ResponseHandler::TextMessageRequest, (void *)to, callback);
        // trial: hoplimit optimization for direct text messages
        int8_t hopsAway = (signed long)nodes[to]->LV_OBJ_IDX(node_sig_idx)->user_data;
        if (hopsAway < 0)
            hopsAway = db.config.lora.hop_limit;
        hopLimit = (hopsAway < db.config.lora.hop_limit ? hopsAway + 1 : hopsAway);
    }

    // tweak to allow multiple lines in single line text area
    for (int i = 0; i < strlen(msg); i++)
        if (msg[i] == CR_REPLACEMENT)
            msg[i] = '\n';

    controller->sendTextMessage(to, ch, hopLimit, actTime, requestId, usePkc, msg);
    addMessage(activeMsgContainer, actTime, requestId, msg, LogMessage::eNone);
}

/**
 * display message that has just been written and sent out
 */
void TFTView_320x240::addMessage(lv_obj_t *container, uint32_t msgTime, uint32_t requestId, char *msg,
                                 LogMessage::MsgStatus status)
{
    lv_obj_t *hiddenPanel = lv_obj_create(container);
    lv_obj_set_width(hiddenPanel, lv_pct(100));
    lv_obj_set_height(hiddenPanel, LV_SIZE_CONTENT);
    lv_obj_set_align(hiddenPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(hiddenPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(hiddenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    add_style_panel_style(hiddenPanel);

    lv_obj_set_style_border_width(hiddenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(hiddenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(hiddenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(hiddenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(hiddenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    hiddenPanel->user_data = (void *)requestId;

    // add timestamp
    char buf[284]; // 237 + 4 + 40 + 2 + 1
    buf[0] = '\0';
    uint32_t len = timestamp(buf, msgTime, status == LogMessage::eNone);
    strcat(&buf[len], msg);

    lv_obj_t *textLabel = lv_label_create(hiddenPanel);
    // calculate expected size of text bubble, to make it look nicer
    lv_coord_t width = lv_txt_get_width(buf, strlen(buf), &ui_font_montserrat_12, 0);
    lv_obj_set_width(textLabel, std::max<int32_t>(std::min<int32_t>(width, 200) + 10, 40));
    lv_obj_set_height(textLabel, LV_SIZE_CONTENT);
    lv_obj_set_y(textLabel, 0);
    lv_obj_set_align(textLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(textLabel, buf);

    add_style_chat_message_style(textLabel);

    lv_obj_scroll_to_view(hiddenPanel, LV_ANIM_ON);
    lv_obj_move_foreground(objects.message_input_area);

    switch (status) {
    case LogMessage::eHeard:
        lv_obj_set_style_border_color(textLabel, colorYellow, LV_PART_MAIN | LV_STATE_DEFAULT);
        break;
    case LogMessage::eAcked:
        lv_obj_set_style_border_color(textLabel, colorBlueGreen, LV_PART_MAIN | LV_STATE_DEFAULT);
        break;
    case LogMessage::eFailed:
        lv_obj_set_style_border_color(textLabel, colorRed, LV_PART_MAIN | LV_STATE_DEFAULT);
        break;
    default:
        break;
    }
}

void TFTView_320x240::addNode(uint32_t nodeNum, uint8_t ch, const char *userShort, const char *userLong, uint32_t lastHeard,
                              eRole role, bool hasKey, bool viaMqtt)
{
    // lv_obj nodesPanel children  |  user data (4 bytes)
    // ==================================================
    // [0]: img                    | role
    // [1]: btn                    | ll group
    // [2]: lbl user long          | nodeNum
    // [3]: lbl user short         | userShort (4 chars)
    // [4]: lbl battery            | hasKey
    // [5]: lbl lastHeard          | lastHeard / curtime
    // [6]: lbl signal (or hops)   | hops away
    // [7]: lbl position 1         | lat
    // [8]: lbl position 2         | lon
    // [9]: lbl telemetry 1        |
    // [10]: lbl telemetry 2       | iaq
    // panel user_data: ch

    ILOG_DEBUG("addNode(%d): num=0x%08x, lastseen=%d, name=%s(%s), role=%d", nodeCount, nodeNum, lastHeard, userLong, userShort,
               role);
    while (nodeCount >= MAX_NUM_NODES_VIEW) {
        purgeNode(nodeNum);
    }

    lv_obj_t *p = lv_obj_create(objects.nodes_panel);
    lv_ll_t *lv_group_ll = &lv_group_get_default()->obj_ll;

    p->user_data = (void *)(uint32_t)ch;
    nodes[nodeNum] = p;
    nodeCount++;

    // NodePanel
    lv_obj_set_pos(p, LV_PCT(0), 0);
    lv_obj_set_size(p, LV_PCT(100), 53);
    lv_obj_set_align(p, LV_ALIGN_CENTER);
    lv_obj_set_style_pad_top(p, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(p, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_flag(p, lv_obj_flag_t(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                                        LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE));
    add_style_node_panel_style(p);

    // NodeImage
    lv_obj_t *img = lv_img_create(p);
    setNodeImage(nodeNum, role, viaMqtt, img);
    lv_obj_set_pos(img, -5, 3);
    lv_obj_set_size(img, 32, 32);
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(img, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(img, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    if (!hasKey) {
        lv_obj_set_style_border_color(img, colorRed, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    img->user_data = (void *)role;

    // NodeButton
    lv_obj_t *nodeButton = lv_btn_create(p);
    lv_obj_set_pos(nodeButton, 0, 0);
    lv_obj_set_size(nodeButton, LV_PCT(106), LV_PCT(100));
    add_style_node_button_style(nodeButton);
    lv_obj_set_align(nodeButton, LV_ALIGN_CENTER);
    lv_obj_add_flag(nodeButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_shadow_width(nodeButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_max_height(nodeButton, 132, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_min_height(nodeButton, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    nodeButton->user_data = _lv_ll_get_tail(lv_group_ll);

    // UserNameLabel
    lv_obj_t *ln_lbl = lv_label_create(p);
    lv_obj_set_pos(ln_lbl, -5, 35);
    lv_obj_set_size(ln_lbl, LV_PCT(80), LV_SIZE_CONTENT);
    lv_label_set_long_mode(ln_lbl, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ln_lbl, userLong);
    ln_lbl->user_data = (void *)nodeNum;
    lv_obj_set_style_align(ln_lbl, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    // UserNameShortLabel
    lv_obj_t *sn_lbl = lv_label_create(p);
    lv_obj_set_pos(sn_lbl, 30, 10);
    lv_obj_set_size(sn_lbl, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_label_set_long_mode(sn_lbl, LV_LABEL_LONG_WRAP);
    lv_label_set_text(sn_lbl, userShort);
    lv_obj_set_style_align(sn_lbl, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(sn_lbl, &ui_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    char *userData = (char *)&(sn_lbl->user_data);
    userData[0] = userShort[0];
    if (userData[0] == 0x00)
        userData[0] = ' ';
    userData[1] = userShort[1];
    if (userData[1] == 0x00)
        userData[1] = ' ';
    userData[2] = userShort[2];
    if (userData[2] == 0x00)
        userData[2] = ' ';
    userData[3] = userShort[3];
    if (userData[3] == 0x00)
        userData[3] = ' ';

    //  BatteryLabel
    lv_obj_t *ui_BatteryLabel = lv_label_create(p);
    lv_obj_set_pos(ui_BatteryLabel, 8, 17);
    lv_obj_set_size(ui_BatteryLabel, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_BatteryLabel, LV_ALIGN_TOP_RIGHT);
    lv_label_set_text(ui_BatteryLabel, "");
    lv_obj_set_style_text_align(ui_BatteryLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    ui_BatteryLabel->user_data = (void *)hasKey;
    // LastHeardLabel
    lv_obj_t *ui_lastHeardLabel = lv_label_create(p);
    lv_obj_set_pos(ui_lastHeardLabel, 8, 33);
    lv_obj_set_size(ui_lastHeardLabel, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_align(ui_lastHeardLabel, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(ui_lastHeardLabel, LV_LABEL_LONG_CLIP);

    // TODO: devices without actual time will report all nodes as lastseen = now
    if (lastHeard) {
        lastHeard = std::min(curtime, (time_t)lastHeard); // adapt values too large

        char buf[20];
        bool isOnline = lastHeardToString(lastHeard, buf);
        lv_label_set_text(ui_lastHeardLabel, buf);
        if (isOnline) {
            nodesOnline++;
        }
    } else {
        lv_label_set_text(ui_lastHeardLabel, "");
    }

    lv_obj_set_style_text_align(ui_lastHeardLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    ui_lastHeardLabel->user_data = (void *)lastHeard;
    // SignalLabel / hopsAway
    lv_obj_t *ui_SignalLabel = lv_label_create(p);
    lv_obj_set_width(ui_SignalLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_SignalLabel, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_SignalLabel, 8, 1);
    lv_obj_set_align(ui_SignalLabel, LV_ALIGN_TOP_RIGHT);
    lv_label_set_text(ui_SignalLabel, "");
    ui_SignalLabel->user_data = (void *)-1; // TODO viaMqtt; // used for filtering (applyNodesFilter)
    // PositionLabel
    lv_obj_t *ui_PositionLabel = lv_label_create(p);
    lv_obj_set_pos(ui_PositionLabel, -5, 49);
    lv_obj_set_size(ui_PositionLabel, 120, LV_SIZE_CONTENT);
    lv_label_set_long_mode(ui_PositionLabel, LV_LABEL_LONG_CLIP);
    lv_label_set_text(ui_PositionLabel, "");
    lv_obj_set_style_align(ui_PositionLabel, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_PositionLabel, colorBlueGreen, LV_PART_MAIN | LV_STATE_DEFAULT);
    ui_PositionLabel->user_data = 0; // store latitude
    // Position2Label
    lv_obj_t *ui_Position2Label = lv_label_create(p);
    lv_obj_set_pos(ui_Position2Label, -5, 63);
    lv_obj_set_size(ui_Position2Label, 108, LV_SIZE_CONTENT);
    lv_label_set_long_mode(ui_Position2Label, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_Position2Label, "");
    lv_obj_set_style_align(ui_Position2Label, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    ui_Position2Label->user_data = 0; // store longitude
    // Telemetry1Label
    lv_obj_t *ui_Telemetry1Label = lv_label_create(p);
    lv_obj_set_pos(ui_Telemetry1Label, 8, 49);
    lv_obj_set_size(ui_Telemetry1Label, 130, LV_SIZE_CONTENT);
    lv_label_set_long_mode(ui_Telemetry1Label, LV_LABEL_LONG_CLIP);
    lv_label_set_text(ui_Telemetry1Label, "");
    lv_obj_set_style_align(ui_Telemetry1Label, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Telemetry1Label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    // Telemetry2Label
    lv_obj_t *ui_Telemetry2Label = lv_label_create(p);
    lv_obj_set_pos(ui_Telemetry2Label, 8, 63);
    lv_obj_set_size(ui_Telemetry2Label, 130, LV_SIZE_CONTENT);
    lv_label_set_long_mode(ui_Telemetry2Label, LV_LABEL_LONG_CLIP);
    lv_label_set_text(ui_Telemetry2Label, "");
    lv_obj_set_style_align(ui_Telemetry2Label, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Telemetry2Label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(nodeButton, ui_event_NodeButton, LV_EVENT_ALL, (void *)nodeNum);

    // move node into new position within nodePanel
    if (lastHeard) {
        lv_obj_t **children = objects.nodes_panel->spec_attr->children;
        int i = objects.nodes_panel->spec_attr->child_cnt - 1;
        while (i > 1) {
            if (lastHeard <= (time_t)(children[i - 1]->LV_OBJ_IDX(node_lh_idx)->user_data))
                break;
            i--;
        }
        if (i >= 1 && i < objects.nodes_panel->spec_attr->child_cnt - 1) {
            lv_obj_move_to_index(p, i);
            // re-arrange the group linked list by moving the new button (now at the tail) into the right position
            void *after = children[i + 1]->LV_OBJ_IDX(node_btn_idx)->user_data;
            _lv_ll_move_before(lv_group_ll, nodeButton->user_data, after);
        }
    }

    if (!nodesChanged) {
        applyNodesFilter(nodeNum);
        updateNodesStatus();
    }
}

void TFTView_320x240::setMyInfo(uint32_t nodeNum)
{
    ownNode = nodeNum;
}

void TFTView_320x240::setDeviceMetaData(int hw_model, const char *version, bool has_bluetooth, bool has_wifi, bool has_eth,
                                        bool can_shutdown)
{
}

void TFTView_320x240::addOrUpdateNode(uint32_t nodeNum, uint8_t ch, const char *userShort, const char *userLong,
                                      uint32_t lastHeard, eRole role, bool hasKey, bool viaMqtt)
{
    if (nodes.find(nodeNum) == nodes.end()) {
        addNode(nodeNum, ch, userShort, userLong, lastHeard, role, hasKey, viaMqtt);
    } else {
        updateNode(nodeNum, ch, userShort, userLong, lastHeard, role, hasKey, viaMqtt);
    }
}

/**
 * @brief update node userName and image
 *
 * @param nodeNum
 * @param ch
 * @param userShort
 * @param userLong
 * @param lastHeard
 * @param role
 * @param viaMqtt
 */
void TFTView_320x240::updateNode(uint32_t nodeNum, uint8_t ch, const char *userShort, const char *userLong, uint32_t lastHeard,
                                 eRole role, bool hasKey, bool viaMqtt)
{
    auto it = nodes.find(nodeNum);
    if (it != nodes.end() && it->second) {
        if (it->first == ownNode) {
            // update related settings buttons and store role in image user data
            char buf[30];
            lv_snprintf(buf, sizeof(buf), _("User name: %s"), userShort);
            lv_label_set_text(objects.basic_settings_user_label, buf);

            char buf1[30], buf2[40];
            lv_dropdown_set_selected(objects.settings_device_role_dropdown, role2val(meshtastic_Config_DeviceConfig_Role(role)));
            lv_dropdown_get_selected_str(objects.settings_device_role_dropdown, buf1, sizeof(buf1));
            lv_snprintf(buf2, sizeof(buf2), _("Device Role: %s"), buf1);
            lv_label_set_text(objects.basic_settings_role_label, buf2);

            // update DB
            strcpy(db.short_name, userShort);
            strcpy(db.long_name, userLong);
            db.config.device.role = (meshtastic_Config_DeviceConfig_Role)role;
        }
        lv_label_set_text(it->second->LV_OBJ_IDX(node_lbl_idx), userLong);
        it->second->LV_OBJ_IDX(node_lbl_idx)->user_data = (void *)nodeNum;
        lv_label_set_text(it->second->LV_OBJ_IDX(node_lbs_idx), userShort);
        char *userData = (char *)&(it->second->LV_OBJ_IDX(node_lbs_idx)->user_data);
        userData[0] = userShort[0];
        if (userData[0] == 0x00)
            userData[0] = ' ';
        userData[1] = userShort[1];
        if (userData[1] == 0x00)
            userData[1] = ' ';
        userData[2] = userShort[2];
        if (userData[2] == 0x00)
            userData[2] = ' ';
        userData[3] = userShort[3];
        if (userData[3] == 0x00)
            userData[3] = ' ';

        setNodeImage(nodeNum, role, viaMqtt, it->second->LV_OBJ_IDX(node_img_idx));

        if (hasKey) {
            // set border color to bg color
            lv_color_t color = lv_obj_get_style_bg_color(it->second->LV_OBJ_IDX(node_img_idx), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(it->second->LV_OBJ_IDX(node_img_idx), color, LV_PART_MAIN | LV_STATE_DEFAULT);
        } else {
            lv_obj_set_style_border_color(it->second->LV_OBJ_IDX(node_img_idx), colorRed, LV_PART_MAIN | LV_STATE_DEFAULT);
        }

        // update chat name
        auto ct = chats.find(it->first);
        if (ct != chats.end()) {
            char buf[64];
            lv_snprintf(buf, sizeof(buf), "%s: %s", lv_label_get_text(it->second->LV_OBJ_IDX(node_lbs_idx)),
                        lv_label_get_text(it->second->LV_OBJ_IDX(node_lbl_idx)));
            lv_label_set_text(ct->second->spec_attr->children[0], buf);
        }
    }
}

void TFTView_320x240::updatePosition(uint32_t nodeNum, int32_t lat, int32_t lon, int32_t alt, uint32_t sats, uint32_t precision)
{
    if (nodeNum == ownNode) {
        char buf[64];
        int latSeconds = (int)round(lat * 1e-7 * 3600);
        int latDegrees = latSeconds / 3600;
        latSeconds = abs(latSeconds % 3600);
        int latMinutes = latSeconds / 60;
        latSeconds %= 60;
        char latLetter = (lat > 0) ? 'N' : 'S';

        int lonSeconds = (int)round(lon * 1e-7 * 3600);
        int lonDegrees = lonSeconds / 3600;
        lonSeconds = abs(lonSeconds % 3600);
        int lonMinutes = lonSeconds / 60;
        lonSeconds %= 60;
        char lonLetter = (lon > 0) ? 'E' : 'W';

        if (sats)
            sprintf(buf, "%c%02i° %2i'%02i\"   %u sats\n%c%02i° %2i'%02i\"   %dm", latLetter, abs(latDegrees), latMinutes,
                    latSeconds, sats, lonLetter, abs(lonDegrees), lonMinutes, lonSeconds, abs(alt) < 10000 ? alt : 0);
        else
            sprintf(buf, "%c%02i° %2i'%02i\"\n%c%02i° %2i'%02i\"   %dm", latLetter, abs(latDegrees), latMinutes, latSeconds,
                    lonLetter, abs(lonDegrees), lonMinutes, lonSeconds, abs(alt) < 10000 ? alt : 0);

        lv_label_set_text(objects.home_location_label, buf);

        if (lat != 0 && lon != 0) {
            hasPosition = true;
            myLatitude = lat;
            myLongitude = lon;

            // go through existing node list and update distance
            // TODO: need incremental update!?
            for (auto &it : nodes) {
                if (it.first != ownNode) {
                    int32_t nlat = (long)it.second->LV_OBJ_IDX(node_pos1_idx)->user_data;
                    int32_t nlon = (long)it.second->LV_OBJ_IDX(node_pos2_idx)->user_data;
                    if (nlat != 0 && nlon != 0) {
                        updateDistance(it.first, nlat, nlon);
                    }
                }
            }
        }
    } else {
        if (hasPosition) {
            updateDistance(nodeNum, lat, lon);
        }
    }

    if (lat != 0 && lon != 0) {
        char buf[32];
        sprintf(buf, "%.5f %.5f", lat * 1e-7, lon * 1e-7);
        lv_obj_t *panel = nodes[nodeNum];
        lv_label_set_text(panel->LV_OBJ_IDX(node_pos1_idx), buf);
        if (sats)
            sprintf(buf, "%dm MSL  %u sats", abs(alt) < 10000 ? alt : 0, sats);
        sprintf(buf, "%dm MSL", abs(alt) < 10000 ? alt : 0);
        lv_label_set_text(panel->LV_OBJ_IDX(node_pos2_idx), buf);
        // store lat/lon in user_data, because we need these values later to calculate the distance to us
        panel->LV_OBJ_IDX(node_pos1_idx)->user_data = (void *)lat;
        panel->LV_OBJ_IDX(node_pos2_idx)->user_data = (void *)lon;
    }

    applyNodesFilter(nodeNum);
}

void TFTView_320x240::updateDistance(uint32_t nodeNum, int32_t lat, int32_t lon)
{
    // if we know our position then calculate (simple) distance to other node in km
    float dx = 71.5 * 1e-7 * (myLongitude - lon);
    float dy = 111.3 * 1e-7 * (myLatitude - lat);
    float dist = sqrt(dx * dx + dy * dy);

    // add distance to user short field
    char buf[32];
    char *userData = (char *)&(nodes[nodeNum]->LV_OBJ_IDX(node_lbs_idx)->user_data);
    buf[0] = userData[0];
    buf[1] = userData[1];
    buf[2] = userData[2];
    buf[3] = userData[3];
    buf[4] = '\n';

    if (dist > 1.0) {
        sprintf(&buf[5], "%.1f km ", dist);
    } else {
        sprintf(&buf[5], "%d m ", (uint32_t)round(dist * 1000));
    }
    // we used the userShort label to add the distance, so re-arrange a bit the position
    lv_obj_t *userShort = nodes[nodeNum]->LV_OBJ_IDX(node_lbs_idx);
    lv_label_set_text(userShort, buf);
    lv_obj_set_pos(userShort, 30, -1);
}

/**
 * @brief Update battery level and air utilisation
 *
 * @param nodeNum
 * @param bat_level
 * @param voltage
 * @param chUtil
 * @param airUtil
 */
void TFTView_320x240::updateMetrics(uint32_t nodeNum, uint32_t bat_level, float voltage, float chUtil, float airUtil)
{
    auto it = nodes.find(nodeNum);
    if (it != nodes.end()) {
        char buf[48];
        if (it->first == ownNode) {
            sprintf(buf, _("Util %0.1f%%  Air %0.1f%%"), chUtil, airUtil);
            lv_label_set_text(it->second->LV_OBJ_IDX(node_sig_idx), buf);

            // update battery percentage and symbol
            if (bat_level != 0 || voltage != 0) {
                uint32_t shown_level = std::min(bat_level, (uint32_t)100);
                sprintf(buf, "%d%%", shown_level);
                bool alert = false;

                BatteryLevel level;
                BatteryLevel::Status status = level.calcStatus(bat_level, voltage);
                switch (status) {
                case BatteryLevel::Plugged:
                    lv_obj_set_style_bg_image_src(objects.battery_image, &img_battery_plug_image,
                                                  LV_PART_MAIN | LV_STATE_DEFAULT);
                    if (shown_level == 100)
                        buf[0] = '\0';
                    break;
                case BatteryLevel::Charging:
                    lv_obj_set_style_bg_image_src(objects.battery_image, &img_battery_bolt_image,
                                                  LV_PART_MAIN | LV_STATE_DEFAULT);
                    break;
                case BatteryLevel::Full:
                    lv_obj_set_style_bg_image_src(objects.battery_image, &img_battery_full_image,
                                                  LV_PART_MAIN | LV_STATE_DEFAULT);
                    break;
                case BatteryLevel::Mid:
                    lv_obj_set_style_bg_image_src(objects.battery_image, &img_battery_mid_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    break;
                case BatteryLevel::Low:
                    lv_obj_set_style_bg_image_src(objects.battery_image, &img_battery_low_image, LV_PART_MAIN | LV_STATE_DEFAULT);
                    break;
                case BatteryLevel::Empty:
                    lv_obj_set_style_bg_image_src(objects.battery_image, &img_battery_empty_image,
                                                  LV_PART_MAIN | LV_STATE_DEFAULT);
                    break;
                case BatteryLevel::Warn:
                    lv_obj_set_style_bg_image_src(objects.battery_image, &img_battery_empty_warn_image,
                                                  LV_PART_MAIN | LV_STATE_DEFAULT);
                    buf[0] = '\0';
                    alert = true;
                    break;
                default:
                    ILOG_ERROR("unhandled battery level %d", status);
                    break;
                }
                Themes::recolorTopLabel(objects.battery_percentage_label, alert);
                lv_obj_set_style_bg_image_recolor_opa(objects.battery_image, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_label_set_text(objects.battery_percentage_label, buf);
            }
        }

        if (bat_level != 0 || voltage != 0) {
            bat_level = std::min(bat_level, (uint32_t)100);
            sprintf(buf, "%d%% %0.2fV", bat_level, voltage);
            lv_label_set_text(it->second->LV_OBJ_IDX(node_bat_idx), buf);
        }
    }
}

void TFTView_320x240::updateEnvironmentMetrics(uint32_t nodeNum, const meshtastic_EnvironmentMetrics &metrics)
{
    auto it = nodes.find(nodeNum);
    if (it != nodes.end()) {
        char buf[50];
        if ((int)metrics.relative_humidity > 0) {
            sprintf(buf, "%2.1f°C %d%% %3.1fhPa", metrics.temperature, (int)metrics.relative_humidity,
                    metrics.barometric_pressure);
        } else {
            sprintf(buf, "%2.1f°C %3.1fhPa", metrics.temperature, metrics.barometric_pressure);
        }
        lv_label_set_text(it->second->LV_OBJ_IDX(node_tm1_idx), buf);

        if (metrics.iaq > 0 && metrics.iaq < 1000) {
            sprintf(buf, "IAQ: %d %.1fV %.1fmA", metrics.iaq, metrics.voltage, metrics.current);
            lv_label_set_text(it->second->LV_OBJ_IDX(node_tm2_idx), buf);
            it->second->LV_OBJ_IDX(node_tm2_idx)->user_data = (void *)(uint32_t)metrics.iaq;
        }
        applyNodesFilter(nodeNum);
    }
}

void TFTView_320x240::updateAirQualityMetrics(uint32_t nodeNum, const meshtastic_AirQualityMetrics &metrics)
{
    auto it = nodes.find(nodeNum);
    if (it != nodes.end() && it->first != ownNode) {
        // TODO
        // char buf[32];
        // sprintf(buf, "%d %d", metrics.particles_03um, metrics.pm100_environmental);
        // lv_label_set_text(it->second->LV_OBJ_IDX(node_tm2_idx), buf);
    }
}

void TFTView_320x240::updatePowerMetrics(uint32_t nodeNum, const meshtastic_PowerMetrics &metrics)
{
    auto it = nodes.find(nodeNum);
    if (it != nodes.end() && it->first != ownNode) {
        // TODO
        // char buf[32];
        // sprintf(buf, "%0.1fmA %0.2fV", metrics.ch1_current, metrics.ch1_voltage);
        // lv_label_set_text(it->second->LV_OBJ_IDX(node_tm2_idx), buf);
    }
}

/**
 * update signal strength for direct neighbors
 */
void TFTView_320x240::updateSignalStrength(uint32_t nodeNum, int32_t rssi, float snr)
{
    if (nodeNum != ownNode) {
        auto it = nodes.find(nodeNum);
        if (it != nodes.end()) {
            char buf[32];
            if (rssi == 0 && snr == 0.0) {
                buf[0] = '\0';
            } else {
                sprintf(buf, "rssi: %d snr: %.1f", rssi, snr);
            }
            lv_label_set_text(it->second->LV_OBJ_IDX(node_sig_idx), buf);
            it->second->LV_OBJ_IDX(node_sig_idx)->user_data = 0;
        }
    }
}

void TFTView_320x240::updateHopsAway(uint32_t nodeNum, uint8_t hopsAway)
{
    if (nodeNum != ownNode) {
        auto it = nodes.find(nodeNum);
        if (it != nodes.end()) {
            char buf[32];
            sprintf(buf, _("hops: %d"), (int)hopsAway);
            lv_label_set_text(it->second->LV_OBJ_IDX(node_sig_idx), buf);
            it->second->LV_OBJ_IDX(node_sig_idx)->user_data = (void *)(unsigned long)hopsAway;
        }
    }
}

void TFTView_320x240::updateConnectionStatus(const meshtastic_DeviceConnectionStatus &status)
{
    db.connectionStatus = status;
    if (status.has_wifi) {
        if (db.config.network.wifi_enabled || db.config.network.eth_enabled) {
            if (status.wifi.has_status) {
                char buf[20];
                uint32_t ip = status.wifi.status.ip_address;
                sprintf(buf, "%d.%d.%d.%d", ip & 0xff, (ip & 0xff00) >> 8, (ip & 0xff0000) >> 16, (ip & 0xff000000) >> 24);
                lv_label_set_text(objects.home_wlan_label, buf);
                Themes::recolorButton(objects.home_wlan_button, true);
                Themes::recolorText(objects.home_wlan_label, true);
                if (status.wifi.status.is_connected) {
                    lv_obj_set_style_bg_img_src(objects.home_wlan_button, &img_home_wlan_button_image,
                                                LV_PART_MAIN | LV_STATE_DEFAULT);
                } else {
                    lv_obj_set_style_bg_img_src(objects.home_wlan_button, &img_home_wlan_off_image,
                                                LV_PART_MAIN | LV_STATE_DEFAULT);
                }

                if (status.wifi.status.is_mqtt_connected) {
                    Themes::recolorButton(objects.home_mqtt_button, true, 255);
                    Themes::recolorText(objects.home_mqtt_label, true);
                } else {
                    Themes::recolorButton(objects.home_mqtt_button, db.module_config.mqtt.enabled);
                    Themes::recolorText(objects.home_mqtt_label, false);
                }
            }
        } else {
            Themes::recolorButton(objects.home_wlan_button, false);
            Themes::recolorText(objects.home_wlan_label, false);
            if (status.wifi.status.is_mqtt_connected) {
                Themes::recolorButton(objects.home_mqtt_button, true, 255);
                Themes::recolorText(objects.home_mqtt_label, true);
            } else {
                Themes::recolorButton(objects.home_mqtt_button, db.module_config.mqtt.enabled, 100);
                Themes::recolorText(objects.home_mqtt_label, false);
            }
            lv_obj_set_style_bg_img_src(objects.home_wlan_button, &img_home_wlan_off_image, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    } else {
        lv_obj_add_flag(objects.home_wlan_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(objects.home_wlan_button, LV_OBJ_FLAG_HIDDEN);
    }

    if (status.has_bluetooth) {
        if (db.config.bluetooth.enabled) {
            if (status.bluetooth.is_connected) {
                char buf[20];
                uint32_t mac = ownNode;
                lv_obj_set_style_text_color(objects.home_bluetooth_label, colorLightGray, LV_PART_MAIN | LV_STATE_DEFAULT);
                sprintf(buf, "??:??:%02x:%02x:%02x:%02x", mac & 0xff, (mac & 0xff00) >> 8, (mac & 0xff0000) >> 16,
                        (mac & 0xff000000) >> 24);
                lv_label_set_text(objects.home_bluetooth_label, buf);
                lv_obj_set_style_bg_opa(objects.home_bluetooth_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_img_src(objects.home_bluetooth_button, &img_home_bluetooth_on_button_image,
                                            LV_PART_MAIN | LV_STATE_DEFAULT);
            } else {
                lv_obj_set_style_text_color(objects.home_bluetooth_label, colorMidGray, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_img_src(objects.home_bluetooth_button, &img_home_bluetooth_on_button_image,
                                            LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_img_recolor_opa(objects.home_bluetooth_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        } else {
            lv_obj_set_style_text_color(objects.home_bluetooth_label, colorMidGray, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_img_src(objects.home_bluetooth_button, &img_home_bluetooth_off_button_image,
                                        LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_img_recolor_opa(objects.home_bluetooth_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    } else {
        lv_obj_add_flag(objects.home_bluetooth_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(objects.home_bluetooth_button, LV_OBJ_FLAG_HIDDEN);
    }

    if (status.has_ethernet) {
        if (status.ethernet.status.is_connected) {
            char buf[20];
            uint32_t mac = ownNode;
            sprintf(buf, "??:??:%02x:%02x:%02x:%02x", mac & 0xff000000, mac & 0xff0000, mac & 0xff00, mac & 0xff);
            lv_label_set_text(objects.home_ethernet_label, buf);
            lv_obj_set_style_text_color(objects.home_ethernet_label, colorLightGray, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(objects.home_ethernet_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        } else {
            lv_obj_set_style_bg_img_recolor_opa(objects.home_ethernet_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(objects.home_ethernet_label, colorMidGray, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    } else {
        lv_obj_add_flag(objects.home_ethernet_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(objects.home_ethernet_button, LV_OBJ_FLAG_HIDDEN);
    }
}

// ResponseHandler callbacks

void TFTView_320x240::onTextMessageCallback(const ResponseHandler::Request &req, ResponseHandler::EventType evt, int32_t result)
{
    ILOG_DEBUG("onTextMessageCallback: %d %d", evt, result);
    if (evt == ResponseHandler::found) {
        handleTextMessageResponse((unsigned long)req.cookie, req.id, false, result);
    } else if (evt == ResponseHandler::removed) {
        handleTextMessageResponse((unsigned long)req.cookie, req.id, true, result);
    } else {
        ILOG_DEBUG("onTextMessageCallback: timeout!");
    }
}

void TFTView_320x240::onPositionCallback(const ResponseHandler::Request &req, ResponseHandler::EventType evt, int32_t) {}

void TFTView_320x240::onTracerouteCallback(const ResponseHandler::Request &req, ResponseHandler::EventType evt, int32_t) {}

/**
 * handle response from routing
 */
void TFTView_320x240::handleResponse(uint32_t from, const uint32_t id, const meshtastic_Routing &routing,
                                     const meshtastic_MeshPacket &p)
{
    ResponseHandler::Request req{};
    bool ack = false;
    if (from == ownNode) {
        req = requests.findRequest(id);
    } else {
        req = requests.removeRequest(id);
        ack = true;
    }

    if (req.type == ResponseHandler::noRequest) {
        ILOG_WARN("request id 0x%08x not valid (anymore)", id);
    } else {
        ILOG_DEBUG("handleResponse request id 0x%08x", id);
    }
    ILOG_DEBUG("routing tag variant: %d, error: %d", routing.which_variant, routing.error_reason);
    switch (routing.which_variant) {
    case meshtastic_Routing_error_reason_tag: {
        if (routing.error_reason == meshtastic_Routing_Error_NONE) {
            if (req.type == ResponseHandler::TraceRouteRequest) {
                handleTraceRouteResponse(routing);
            } else if (req.type == ResponseHandler::TextMessageRequest) {
                handleTextMessageResponse((unsigned long)req.cookie, id, ack, false);
            } else if (req.type == ResponseHandler::PositionRequest) {
                handlePositionResponse(from, id, p.rx_rssi, p.rx_snr, p.hop_limit == p.hop_start);
            }
        } else if (routing.error_reason == meshtastic_Routing_Error_MAX_RETRANSMIT) {
            ResponseHandler::Request req = requests.removeRequest(id);
            if (req.type == ResponseHandler::TraceRouteRequest) {
                handleTraceRouteResponse(routing);
            } else if (req.type == ResponseHandler::TextMessageRequest) {
                handleTextMessageResponse((unsigned long)req.cookie, id, ack, true);
            }
        } else if (routing.error_reason == meshtastic_Routing_Error_NO_RESPONSE) {
            if (req.type == ResponseHandler::PositionRequest) {
                handlePositionResponse(from, id, p.rx_rssi, p.rx_snr, p.hop_limit == p.hop_start);
            }
        } else if (routing.error_reason == meshtastic_Routing_Error_NO_CHANNEL) {
            if (req.type == ResponseHandler::TextMessageRequest) {
                handleTextMessageResponse((unsigned long)req.cookie, id, ack, true);
                // we probably have a wrong key; mark it as bad and don't use in future
                if ((unsigned long)nodes[from]->LV_OBJ_IDX(node_bat_idx)->user_data == 1) {
                    ILOG_DEBUG("public key mismatch");
                    nodes[from]->LV_OBJ_IDX(node_bat_idx)->user_data = (void *)2;
                    lv_obj_set_style_border_color(nodes[from]->LV_OBJ_IDX(node_img_idx), colorRed,
                                                  LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_image_src(objects.top_messages_node_image, &img_lock_slash_image,
                                                  LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        } else {
            ILOG_DEBUG("got Routing_Error %d", routing.error_reason);
        }
        break;
    }
    case meshtastic_Routing_route_request_tag: {
        ILOG_ERROR("got meshtastic_Routing_route_request_tag");
        break;
    }
    case meshtastic_Routing_route_reply_tag: {
        ILOG_DEBUG("got meshtastic_Routing_route_reply_tag");
        handleResponse(from, id, routing.route_reply);
        break;
    }
    default:
        ILOG_ERROR("unhandled meshtastic_Routing tag");
        break;
    }
}

/**
 * Signal scanner
 */
void TFTView_320x240::scanSignal(uint32_t scanNo)
{
    if (scans == 1 && spinnerButton) {
        lv_label_set_text(objects.signal_scanner_start_label, _("Start"));
        removeSpinner();
    } else {
        uint32_t requestId;
        uint32_t to = currentNode;
        uint8_t ch = (uint8_t)(unsigned long)currentPanel->user_data;
        requestId = requests.addRequest(to, ResponseHandler::PositionRequest, (void *)to);
        controller->requestPosition(to, ch, requestId);
        objects.signal_scanner_panel->user_data = (void *)requestId;
    }
}

void TFTView_320x240::handlePositionResponse(uint32_t from, uint32_t request_id, int32_t rx_rssi, float rx_snr, bool isNeighbor)
{
    if (request_id == (unsigned long)objects.signal_scanner_panel->user_data) {
        requests.removeRequest(request_id);

        if (from == currentNode && isNeighbor) {
            char buf[20];
            sprintf(buf, "SNR\n%.1f", rx_snr);
            lv_label_set_text(objects.signal_scanner_snr_label, buf);
            sprintf(buf, "RSSI\n%d", rx_rssi);
            lv_label_set_text(objects.signal_scanner_rssi_label, buf);
            lv_slider_set_value(objects.snr_slider, rx_snr, LV_ANIM_ON);
            lv_slider_set_value(objects.rssi_slider, rx_rssi, LV_ANIM_ON);
            sprintf(buf, "%d%%", signalStrength2Percent(rx_rssi, rx_snr));
            lv_label_set_text(objects.signal_scanner_start_label, buf);
        }
    } else {
        ILOG_DEBUG("handlePositionResponse: drop reply with not matching request 0x%08x", request_id);
    }
}

/**
 * Trace Route: handle  ack or timeout
 */
void TFTView_320x240::handleTraceRouteResponse(const meshtastic_Routing &routing)
{
    ILOG_DEBUG("handleTraceRouteResponse: route has %d hops", routing.route_reply.route_count);
    if (routing.error_reason != meshtastic_Routing_Error_NONE) {
        lv_label_set_text(objects.trace_route_start_label, _("Start"));
        removeSpinner();
    } else {
        // we got a first ACK to our route request
        if (spinnerButton) {
            lv_obj_set_style_outline_color(objects.trace_route_start_button, lv_color_hex(0xDBD251),
                                           LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void TFTView_320x240::handleResponse(uint32_t from, uint32_t id, const meshtastic_RouteDiscovery &route)
{
    ILOG_DEBUG("handleResponse: trace route has %d / %d hops", route.route_count, route.route_back_count);
    lv_obj_add_flag(objects.start_button_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.hop_routes_panel, LV_OBJ_FLAG_HIDDEN);

    if (id && requests.findRequest(id).type == ResponseHandler::TraceRouteRequest) {
        requests.removeRequest(id);
    }

    for (int i = route.route_count; i > 0; i--) {
        addNodeToTraceRoute(route.route[i - 1], objects.route_towards_panel);
    }

    for (int i = 0; i < route.route_back_count; i++) {
        addNodeToTraceRoute(route.route_back[i], objects.route_back_panel);
    }

    // route contains only intermediate nodes, so add our node
    addNodeToTraceRoute(ownNode, objects.trace_route_panel);
}

void TFTView_320x240::addNodeToTraceRoute(uint32_t nodeNum, lv_obj_t *panel)
{
    // check if node exists, and get its panel
    lv_obj_t *nodePanel = nullptr;
    auto it = nodes.find(nodeNum);
    if (it != nodes.end()) {
        nodePanel = it->second;
    }
    lv_obj_t *btn = lv_btn_create(panel);
    // objects.trace_route_to_button = btn;
    lv_obj_set_pos(btn, 0, 0);
    lv_obj_set_size(btn, LV_PCT(100), 38);
    add_style_settings_button_style(btn);
    lv_obj_set_style_align(btn, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, colorMidGray, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        {
            lv_obj_t *img = lv_img_create(btn);
            if (nodePanel) {
                setNodeImage(nodeNum, (MeshtasticView::eRole)(unsigned long)nodePanel->LV_OBJ_IDX(node_img_idx)->user_data, false,
                             img);
            } else {
                setNodeImage(0, eRole::unknown, false, img);
            }
            lv_obj_set_pos(img, -5, -7);
            lv_obj_set_size(img, 32, 32);
            lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_border_width(img, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_image_recolor_opa(img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(img, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(img, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // TraceRouteToButtonLabel
            lv_obj_t *label = lv_label_create(btn);
            lv_obj_set_pos(label, 40, 0);
            lv_obj_set_size(label, LV_PCT(80), LV_SIZE_CONTENT);
            lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL);
            if (nodePanel) {
                if (nodeNum != ownNode) {
                    lv_obj_add_event_cb(btn, ui_event_trace_route_node, LV_EVENT_CLICKED, nodePanel);
                    lv_label_set_text(label, lv_label_get_text(nodePanel->LV_OBJ_IDX(node_lbs_idx)));
                } else {
                    lv_label_set_text(label, lv_label_get_text(nodePanel->LV_OBJ_IDX(node_lbl_idx)));
                }
            } else {
                char buf[20];
                if (nodeNum != UINT32_MAX) {
                    lv_snprintf(buf, 16, "!%08x", nodeNum);
                    lv_label_set_text(label, buf);
                } else
                    lv_label_set_text(label, _("unknown"));
            }
            lv_obj_set_style_align(label, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

/**
 * @brief purge oldest node from node list (and all its memory)
 * @param nodeNum node that is being added and already contained in nodes[], so don't remove it!
 */
void TFTView_320x240::purgeNode(uint32_t nodeNum)
{
    if (nodeCount <= 1)
        return;

    lv_obj_t **children = objects.nodes_panel->spec_attr->children;
    int last = objects.nodes_panel->spec_attr->child_cnt - 1;
    int i = last;

#ifndef ALWAYS_PURGE_OLDEST_NODE
    time_t curr_time;
#ifdef ARCH_PORTDUINO
    time(&curr_time);
#else
    curr_time = actTime;
#endif
    // prefer purging older unknown nodes first (but not the brand new ones)
    while ((eRole)(long)(children[i]->LV_OBJ_IDX(node_img_idx)->user_data) != eRole::unknown ||
           curr_time < (time_t)(children[i]->LV_OBJ_IDX(node_lh_idx)->user_data) + 120 ||
           (unsigned long)(children[i]->LV_OBJ_IDX(node_lbl_idx)->user_data) == nodeNum ||
           chats.find((unsigned long)(children[i]->LV_OBJ_IDX(node_lbl_idx)->user_data)) != chats.end()) {
        if (i < (last + 1) / 5) { // keep 80% named nodes and 20% unknown (not fresh) nodes
            i = last;
            break;
        }
        i--;
    }
#endif
    lv_obj_t *p = children[i];
    uint32_t oldest = (unsigned long)(p->LV_OBJ_IDX(node_lbl_idx)->user_data);
    uint32_t lastHeard = (unsigned long)p->LV_OBJ_IDX(node_lh_idx)->user_data;
    if (lastHeard > 0 && (curtime - lastHeard <= secs_until_offline))
        nodesOnline--;

    ILOG_INFO("removing oldest node 0x%08x", oldest);
    lv_obj_delete(p);
    {
        auto it = messages.find(oldest);
        if (it != messages.end()) {
            lv_obj_delete(it->second);
            messages.erase(oldest);
        }
    }

    {
        auto it = chats.find(oldest);
        if (it != chats.end()) {
            lv_obj_delete(it->second);
            chats.erase(oldest);
            updateActiveChats();
        }
    }
    nodes.erase(oldest);
    nodeCount--;
    nodesChanged = true; // flag to force re-apply node filter
}

/**
 * @brief apply enabled filters and highlight node
 *
 * @param nodeNum
 * @param reset : set true when filter has changed (to recalculate number of filtered nodes)
 * @return true
 * @return false
 */
bool TFTView_320x240::applyNodesFilter(uint32_t nodeNum, bool reset)
{
    lv_obj_t *panel = nodes[nodeNum];
    bool hide = false;
    if (nodeNum != ownNode /* && filter.active*/) { // TODO
        if (lv_obj_has_state(objects.nodes_filter_unknown_switch, LV_STATE_CHECKED)) {
            if (lv_img_get_src(panel->LV_OBJ_IDX(node_img_idx)) == &img_circle_question_image) {
                hide = true;
            }
        }
        if (lv_obj_has_state(objects.nodes_filter_offline_switch, LV_STATE_CHECKED)) {
            time_t lastHeard = (time_t)panel->LV_OBJ_IDX(node_lh_idx)->user_data;
            if (lastHeard == 0 || curtime - lastHeard > secs_until_offline)
                hide = true;
        }
        if (lv_obj_has_state(objects.nodes_filter_public_key_switch, LV_STATE_CHECKED)) {
            bool hasKey = (unsigned long)panel->LV_OBJ_IDX(node_bat_idx)->user_data == 1;
            if (!hasKey)
                hide = true;
        }
        if (lv_dropdown_get_selected(objects.nodes_filter_channel_dropdown) != 0) {
            int selected = lv_dropdown_get_selected(objects.nodes_filter_channel_dropdown);
            if (selected != 0) {
                uint8_t ch = (uint8_t)(unsigned long)panel->user_data;
                if (selected - 1 != ch)
                    hide = true;
            }
        }
        if (lv_dropdown_get_selected(objects.nodes_filter_hops_dropdown) != 0) {
            int32_t hopsAway = (signed long)panel->LV_OBJ_IDX(node_sig_idx)->user_data;
            int selected = lv_dropdown_get_selected(objects.nodes_filter_hops_dropdown) - 7;
            if (hopsAway < 0)
                hide = true;
            else if (selected <= 0) {
                if (hopsAway > -selected)
                    hide = true;
            } else {
                if (hopsAway < selected)
                    hide = true;
            }
        }
#if 0
        if (lv_obj_has_state(objects.nodes_filter_mqtt_switch, LV_STATE_CHECKED)) {
            bool viaMqtt = false; // TODO (unsigned long)panel->LV_OBJ_IDX(node_sig_idx)->user_data;
            if (viaMqtt)
                hide = true;
        }
#endif
        if (lv_obj_has_state(objects.nodes_filter_position_switch, LV_STATE_CHECKED)) {
            if (lv_label_get_text(panel->LV_OBJ_IDX(node_pos1_idx))[0] == '\0')
                hide = true;
        }
        const char *name = lv_textarea_get_text(objects.nodes_filter_name_area);
        if (name[0] != '\0') {
            if (name[0] != '!') { // use '!' char to negate search result
                if (!strcasestr(lv_label_get_text(panel->LV_OBJ_IDX(node_lbl_idx)), name) &&
                    !strcasestr(lv_label_get_text(panel->LV_OBJ_IDX(node_lbs_idx)), name)) {
                    hide = true;
                }
            } else {
                if (strcasestr(lv_label_get_text(panel->LV_OBJ_IDX(node_lbl_idx)), &name[1]) ||
                    strcasestr(lv_label_get_text(panel->LV_OBJ_IDX(node_lbs_idx)), &name[1])) {
                    hide = true;
                }
            }
        }
    }
    if (hide) {
        if (reset || !lv_obj_has_flag(panel, LV_OBJ_FLAG_HIDDEN)) {
            lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);
            nodesFiltered++;
        }
    } else {
        lv_obj_clear_flag(panel, LV_OBJ_FLAG_HIDDEN);
    }

    bool highlight = false;
    if (true /*highlight.active*/) { // TODO
        if (lv_obj_has_state(objects.nodes_hl_active_chat_switch, LV_STATE_CHECKED)) {
            auto it = chats.find(nodeNum);
            if (it != nodes.end()) {
                lv_obj_set_style_border_color(panel, colorOrange, LV_PART_MAIN | LV_STATE_DEFAULT);
                highlight = true;
            }
        }
        if (lv_obj_has_state(objects.nodes_hl_position_switch, LV_STATE_CHECKED)) {
            if (lv_label_get_text(panel->LV_OBJ_IDX(node_pos1_idx))[0] != '\0') {
                lv_obj_set_style_border_color(panel, colorBlueGreen, LV_PART_MAIN | LV_STATE_DEFAULT);
                highlight = true;
            }
        }
        if (lv_obj_has_state(objects.nodes_hl_telemetry_switch, LV_STATE_CHECKED)) {
            if (lv_label_get_text(panel->LV_OBJ_IDX(node_tm1_idx))[0] != '\0') {
                lv_obj_set_style_border_color(panel, colorBlue, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_border_width(panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                highlight = true;
            }
        }
        if (lv_obj_has_state(objects.nodes_hliaq_switch, LV_STATE_CHECKED)) {
            if (lv_label_get_text(panel->LV_OBJ_IDX(node_tm2_idx))[0] != '\0') {
                uint32_t iaq = (unsigned long)panel->LV_OBJ_IDX(node_tm2_idx)->user_data;
                // IAQ color code
                lv_color_t fg, bg;
                if (iaq <= 50) {
                    fg = lv_color_hex(0x00000000);
                    bg = lv_color_hex(0x000ce810);
                } else if (iaq <= 100) {
                    fg = lv_color_hex(0x00000000);
                    bg = lv_color_hex(0x00faf646);
                } else if (iaq <= 150) {
                    fg = lv_color_hex(0x00000000);
                    bg = lv_color_hex(0x00f98204);
                } else if (iaq <= 200) {
                    fg = lv_color_hex(0x00000000);
                    bg = lv_color_hex(0x00e42104);
                } else if (iaq <= 300) {
                    fg = lv_color_hex(0xffffffff);
                    bg = lv_color_hex(0x009b2970);
                } else {
                    fg = lv_color_hex(0xffffffff);
                    bg = lv_color_hex(0x001d1414);
                }
                lv_obj_set_style_text_color(panel->LV_OBJ_IDX(node_tm2_idx), fg, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_color(panel->LV_OBJ_IDX(node_tm2_idx), bg, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_opa(panel->LV_OBJ_IDX(node_tm2_idx), 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_border_color(panel, bg, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_border_width(panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                highlight = true;
            }
        }
        const char *name = lv_textarea_get_text(objects.nodes_hl_name_area);
        if (name[0] != '\0') {
            if (strcasestr(lv_label_get_text(panel->LV_OBJ_IDX(node_lbl_idx)), name) ||
                strcasestr(lv_label_get_text(panel->LV_OBJ_IDX(node_lbs_idx)), name)) {
                lv_obj_set_style_border_color(panel, colorMesh, LV_PART_MAIN | LV_STATE_DEFAULT);
                highlight = true;
            }
        }
    }
    if (!highlight) {
        lv_obj_set_style_border_color(panel, colorMidGray, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    return hide; // TODO || filter.active;
}

void TFTView_320x240::messageAlert(const char *alert, bool show)
{
    lv_label_set_text(objects.alert_label, alert);
    if (show)
        lv_obj_clear_flag(objects.alert_panel, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(objects.alert_panel, LV_OBJ_FLAG_HIDDEN);
}

/**
 * @brief mark the sent message as either heard or acknowledged or failed
 *
 * @param channelOrNode
 * @param id
 * @param ack
 */
void TFTView_320x240::handleTextMessageResponse(uint32_t channelOrNode, const uint32_t id, bool ack, bool err)
{
    lv_obj_t *msgContainer;
    if (channelOrNode < c_max_channels) {
        msgContainer = channelGroup[(uint8_t)channelOrNode];
        ack = true; // treat messages sent to group channel same as ack
    } else {
        msgContainer = messages[channelOrNode];
    }
    if (!msgContainer) {
        ILOG_WARN("received unexpected response nodeNum/channel 0x%08x for request id 0x%08x", channelOrNode, id);
        return;
    }
    // go through all hiddenPanels and search for requestId
    uint16_t i = msgContainer->spec_attr->child_cnt;
    while (i-- > 0) {
        lv_obj_t *panel = msgContainer->spec_attr->children[i];
        uint32_t requestId = (unsigned long)panel->user_data;
        if (requestId == id) {
            // now give the textlabel border another color
            lv_obj_t *textLabel = panel->spec_attr->children[0];
            lv_obj_set_style_border_color(textLabel,
                                          err   ? colorRed
                                          : ack ? colorBlueGreen
                                                : colorYellow,
                                          LV_PART_MAIN | LV_STATE_DEFAULT);

            // store message
            break;
        }
    }
}

void TFTView_320x240::packetReceived(const meshtastic_MeshPacket &p)
{
    MeshtasticView::packetReceived(p);

    // try update time from packet
    if (!VALID_TIME(actTime) && VALID_TIME(p.rx_time))
        updateTime(p.rx_time);

    if (detectorRunning) {
        packetDetected(p);
    }
    if (packetLogEnabled) {
        writePacketLog(p);
    }
    if (p.from != ownNode) {
        updateSignalStrength(p.rx_rssi, p.rx_snr);
    }
    updateStatistics(p);
}

void TFTView_320x240::notifyResync(bool show)
{
    messageAlert(_("Resynch ..."), show);
    if (!show) {
        lv_screen_load_anim(objects.main_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    }
}

void TFTView_320x240::notifyReboot(bool show)
{
    messageAlert(_("Rebooting ..."), show);
}

void TFTView_320x240::notifyShutdown(void)
{
    messageAlert(_("Shutting down ..."), true);
}

void TFTView_320x240::blankScreen(bool enable)
{
    ILOG_DEBUG("%s screen (%s)", enable ? "blank" : "unblank", screenLocked ? "locked" : "timeout");
    if (enable)
        lv_screen_load_anim(objects.blank_screen, LV_SCR_LOAD_ANIM_FADE_OUT, 1000, 0, false);
    else {
        if (objects.main_screen)
            lv_screen_load_anim(objects.main_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
        else
            lv_screen_load_anim(objects.boot_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    }
}

void TFTView_320x240::screenSaving(bool enabled)
{
    if (enabled) {
        // overlay main screen with blank screen to prevent accidentally pressing buttons
        lv_screen_load_anim(objects.blank_screen, LV_SCR_LOAD_ANIM_FADE_OUT, 0, 0, false);
        lv_group_focus_obj(objects.blank_screen_button);
        screenLocked = true;
        screenUnlockRequest = false;
    } else {
        if (THIS->db.uiConfig.screen_lock) {
            ILOG_DEBUG("showing lock screen");
            lv_screen_load_anim(objects.lock_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
        } else if (objects.main_screen) {
            ILOG_DEBUG("showing main screen");
            lv_screen_load_anim(objects.main_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
            screenLocked = false;
        } else {
            ILOG_DEBUG("showing boot screen");
            lv_screen_load_anim(objects.boot_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
            screenLocked = false;
        }
    }
}

bool TFTView_320x240::isScreenLocked(void)
{
    return screenLocked && !screenUnlockRequest;
}

void TFTView_320x240::updateChannelConfig(const meshtastic_Channel &ch)
{
    static lv_obj_t *btn[c_max_channels] = {objects.channel_button0, objects.channel_button1, objects.channel_button2,
                                            objects.channel_button3, objects.channel_button4, objects.channel_button5,
                                            objects.channel_button6, objects.channel_button7};
    db.channel[ch.index] = ch;

    if (ch.role != meshtastic_Channel_Role_DISABLED) {
        setChannelName(ch);

        lv_obj_set_width(btn[ch.index], lv_pct(70));
        lv_obj_set_style_pad_left(btn[ch.index], 8, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *lockImage = NULL;
        if (lv_obj_get_child_cnt(btn[ch.index]) == 1)
            lockImage = lv_img_create(btn[ch.index]);
        else
            lockImage = lv_obj_get_child(btn[ch.index], 1);

        uint32_t recolor = 0;

        if (memcmp(ch.settings.psk.bytes, "\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000", 16) == 0) {
            lv_image_set_src(lockImage, &img_groups_key_image);
            recolor = 0xF2E459; // yellow
        } else if (memcmp(ch.settings.psk.bytes, "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000", 16) == 0) {
            lv_image_set_src(lockImage, &img_groups_unlock_image);
            recolor = 0xF72B2B; // reddish
        } else {
            lv_image_set_src(lockImage, &img_groups_lock_image);
            recolor = 0x1EC174; // green
        }
        lv_obj_set_width(lockImage, LV_SIZE_CONTENT);  /// 1
        lv_obj_set_height(lockImage, LV_SIZE_CONTENT); /// 1
        lv_obj_set_align(lockImage, LV_ALIGN_LEFT_MID);
        lv_obj_add_flag(lockImage, LV_OBJ_FLAG_ADV_HITTEST);  /// Flags
        lv_obj_clear_flag(lockImage, LV_OBJ_FLAG_SCROLLABLE); /// Flags
        lv_obj_set_style_img_recolor(lockImage, lv_color_hex(recolor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(lockImage, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        // display smaller button with just the channel number
        char buf[10];
        lv_snprintf(buf, sizeof(buf), "%d", ch.index);
        lv_label_set_text(channel[ch.index], buf);
        lv_obj_set_width(btn[ch.index], lv_pct(30));

        if (lv_obj_get_child_cnt(btn[ch.index]) == 2) {
            lv_obj_delete(lv_obj_get_child(btn[ch.index], 1));
        }
    }
}

void TFTView_320x240::updateDeviceConfig(const meshtastic_Config_DeviceConfig &cfg)
{
    db.config.device = cfg;
    db.config.has_device = true;

    char buf1[30], buf2[40];
    lv_dropdown_set_selected(objects.settings_device_role_dropdown, role2val(cfg.role));
    lv_dropdown_get_selected_str(objects.settings_device_role_dropdown, buf1, sizeof(buf1));
    lv_snprintf(buf2, sizeof(buf2), _("Device Role: %s"), buf1);
    lv_label_set_text(objects.basic_settings_role_label, buf2);
}

void TFTView_320x240::updatePositionConfig(const meshtastic_Config_PositionConfig &cfg)
{
    db.config.position = cfg;
    db.config.has_position = true;
    Themes::recolorButton(objects.home_location_button, cfg.gps_mode == meshtastic_Config_PositionConfig_GpsMode_ENABLED);
    Themes::recolorText(objects.home_location_label, cfg.gps_mode == meshtastic_Config_PositionConfig_GpsMode_ENABLED);
}

void TFTView_320x240::updatePowerConfig(const meshtastic_Config_PowerConfig &cfg)
{
    db.config.power = cfg;
    db.config.has_power = true;
}

void TFTView_320x240::updateNetworkConfig(const meshtastic_Config_NetworkConfig &cfg)
{
    db.config.network = cfg;
    db.config.has_network = true;

    char buf[40];
    lv_snprintf(buf, sizeof(buf), _("WiFi: %s"), cfg.wifi_ssid[0] ? cfg.wifi_ssid : _("<not set>"));
    lv_label_set_text(objects.basic_settings_wifi_label, buf);
}

void TFTView_320x240::updateDisplayConfig(const meshtastic_Config_DisplayConfig &cfg)
{
    db.config.display = cfg;
    db.config.has_display = true;
}

void TFTView_320x240::updateLoRaConfig(const meshtastic_Config_LoRaConfig &cfg)
{
    db.config.lora = cfg;
    db.config.has_lora = true;
    showLoRaFrequency(cfg);

    char region[30];
    lv_snprintf(region, sizeof(region), _("Region: %s"), LoRaPresets::loRaRegionToString(cfg.region));
    lv_label_set_text(objects.basic_settings_region_label, region);

    char buf1[20], buf2[32];
    lv_dropdown_set_selected(objects.settings_modem_preset_dropdown, cfg.modem_preset);
    lv_dropdown_get_selected_str(objects.settings_modem_preset_dropdown, buf1, sizeof(buf1));
    lv_snprintf(buf2, sizeof(buf2), _("Modem Preset: %s"), buf1);
    lv_label_set_text(objects.basic_settings_modem_preset_label, buf2);

    uint32_t numChannels = LoRaPresets::getNumChannels(cfg.region, cfg.modem_preset);
    lv_slider_set_range(objects.frequency_slot_slider, 1, numChannels);

    if (!db.config.lora.channel_num) {
        db.config.lora.channel_num = LoRaPresets::getDefaultSlot(db.config.lora.region, THIS->db.config.lora.modem_preset);
    }
    lv_slider_set_value(objects.frequency_slot_slider, db.config.lora.channel_num, LV_ANIM_OFF);

    if (db.config.lora.region != meshtastic_Config_LoRaConfig_RegionCode_UNSET) {
        // update channel names again now that region is known
        for (int i = 0; i < c_max_channels; i++) {
            if (db.channel[i].has_settings && db.channel[i].role != meshtastic_Channel_Role_DISABLED) {
                setChannelName(db.channel[i]);
            }
        }
    } else {
        requestSetup();
    }
}

void TFTView_320x240::showLoRaFrequency(const meshtastic_Config_LoRaConfig &cfg)
{
    char loraFreq[48];
    float frequency = LoRaPresets::getRadioFreq(cfg.region, cfg.modem_preset, cfg.channel_num);
    if (frequency > 1.0 && frequency < 10000.0) {
        sprintf(loraFreq, "LoRa %g MHz\n[%s kHz]", frequency, LoRaPresets::getBandwidthString(cfg.modem_preset));
    } else {
        strcpy(loraFreq, _("region unset"));
    }
    lv_label_set_text(objects.home_lora_label, loraFreq);
    Themes::recolorButton(objects.home_lora_button, cfg.tx_enabled);
    Themes::recolorText(objects.home_lora_label, cfg.tx_enabled);
    if (!cfg.tx_enabled) {
        lv_obj_clear_flag(objects.top_lora_tx_panel, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(objects.top_lora_tx_panel, LV_OBJ_FLAG_HIDDEN);
    }
}

void TFTView_320x240::setBellText(bool banner, bool sound)
{
    if (banner && sound) {
        lv_label_set_text(objects.home_bell_label, _("Banner & Sound"));
    } else if (banner) {
        lv_label_set_text(objects.home_bell_label, _("Banner only"));
    } else if (sound) {
        lv_label_set_text(objects.home_bell_label, _("Sound only"));
    } else {
        lv_label_set_text(objects.home_bell_label, _("silent"));
    }

    char buf[40];
    lv_snprintf(buf, sizeof(buf), _("Message Alert: %s"),
                db.module_config.external_notification.alert_message_buzzer
                    ? (!sound ? _("silent") : ringtone[db.uiConfig.ring_tone_id].name)
                    : "off");
    lv_label_set_text(objects.basic_settings_alert_label, buf);

    Themes::recolorButton(objects.home_bell_button, banner || sound);
    Themes::recolorText(objects.home_bell_label, banner || sound);
}

/**
 * auto set primary(secondary) channel name (based on region)
 */
void TFTView_320x240::setChannelName(const meshtastic_Channel &ch)
{
    char buf[40];
    if (ch.role == meshtastic_Channel_Role_PRIMARY) {
        sprintf(buf, _("Channel: %s"),
                strlen(ch.settings.name) ? ch.settings.name
                : db.config.lora.region == meshtastic_Config_LoRaConfig_RegionCode_UNSET
                    ? ("<unset>")
                    : LoRaPresets::modemPresetToString(db.config.lora.modem_preset));
        lv_label_set_text(objects.basic_settings_channel_label, buf);

        sprintf(buf, "*%s",
                strlen(ch.settings.name) ? ch.settings.name
                : db.config.lora.region == meshtastic_Config_LoRaConfig_RegionCode_UNSET
                    ? ("<unset>")
                    : LoRaPresets::modemPresetToString(db.config.lora.modem_preset));
    } else {
        if (ch.settings.name[0] == '\0' && ch.settings.psk.size == 1 && ch.settings.psk.bytes[0] == 0x01) {
            sprintf(buf, "%s", LoRaPresets::modemPresetToString(db.config.lora.modem_preset));
        } else {
            strcpy(buf, ch.settings.name);
        }
    }

    lv_label_set_text(channel[ch.index], buf);

    // rename chat
    auto it = chats.find(ch.index);
    if (it != chats.end()) {
        char buf2[64];
        sprintf(buf2, "%d: %s", (int)ch.index, buf);
        lv_label_set_text(it->second->spec_attr->children[0], buf2);
    }
}

/**
 * @brief write local time stamp into buffer
 *        if date is not current also add day/month
 *        Note: time string ends with linefeed
 *
 * @param buf allocated buffer
 * @param datetime date/time to write
 * @param update update with actual time, otherwise using time from parameter 'time'
 * @return length of time string
 */
uint32_t TFTView_320x240::timestamp(char *buf, uint32_t datetime, bool update)
{
    time_t local = datetime;
    if (update) {
#ifdef ARCH_PORTDUINO
        time(&local);
#else
        if (VALID_TIME(actTime))
            local = actTime;
#endif
    }
    if (VALID_TIME(local)) {
        std::tm date_tm{};
        localtime_r(&local, &date_tm);
        if (!update)
            return strftime(buf, 20, "%y/%m/%d %R\n", &date_tm);
        else
            return strftime(buf, 20, "%R\n", &date_tm);
    } else
        return 0;
}

/**
 * calculate percentage value from rssi and snr
 * Note: ranges are based on the axis values of the signal scanner
 */
int32_t TFTView_320x240::signalStrength2Percent(int32_t rx_rssi, float rx_snr)
{
#if defined(USE_SX127x)
    int p_snr = ((std::max<int32_t>(rx_snr, -19.0f) + 19.0f) / 33.0f) * 100.0f; // range -19..14
    int p_rssi = ((std::max<int32_t>(rx_rssi, -145L) + 145) * 100) / 90;        // range -145..-55
#else
    int p_snr = ((std::max<int32_t>(rx_snr, -18.0f) + 18.0f) / 26.0f) * 100.0f; // range -18..8
    int p_rssi = ((std::max<int32_t>(rx_rssi, -125) + 125) * 100) / 100;        // range -125..-25
#endif
    return std::min<int32_t>((p_snr + p_rssi * 2) / 3, 100);
}

void TFTView_320x240::updateBluetoothConfig(const meshtastic_Config_BluetoothConfig &cfg, uint32_t id)
{
    db.config.bluetooth = cfg;
    db.config.has_bluetooth = true;

    if (ownNode == 0) {
        ownNode = id;
    }

    if (state <= MeshtasticView::eProgrammingMode && state != MeshtasticView::eWaitingForReboot) {
        enterProgrammingMode();
    }
}

void TFTView_320x240::updateSecurityConfig(const meshtastic_Config_SecurityConfig &cfg)
{
    db.config.security = cfg;
    db.config.has_security = true;
}

void TFTView_320x240::updateSessionKeyConfig(const meshtastic_Config_SessionkeyConfig &cfg)
{
    // TODO
}

/// ---- module updates ----

void TFTView_320x240::updateMQTTModule(const meshtastic_ModuleConfig_MQTTConfig &cfg)
{
    db.module_config.mqtt = cfg;
    db.module_config.has_mqtt = true;

    char buf[32];
    lv_snprintf(buf, sizeof(buf), "%s", db.module_config.mqtt.root);
    lv_label_set_text(objects.home_mqtt_label, buf);

    if (!db.module_config.mqtt.enabled) {
        Themes::recolorButton(objects.home_mqtt_button, false);
        Themes::recolorText(objects.home_mqtt_label, false);
    }
}

void TFTView_320x240::updateExtNotificationModule(const meshtastic_ModuleConfig_ExternalNotificationConfig &cfg)
{
    db.module_config.external_notification = cfg;
    db.module_config.has_external_notification = true;

    char buf[32];
    lv_snprintf(buf, sizeof(buf), _("Message Alert: %s"),
                db.module_config.external_notification.alert_message_buzzer && db.module_config.external_notification.enabled
                    ? _("on")
                    : _("off"));
    lv_label_set_text(objects.basic_settings_alert_label, buf);
}

void TFTView_320x240::updateRingtone(const char rtttl[231])
{
    // retrieving ringtone index for dropdown
    uint16_t rtIndex = 0;
    for (int i = 0; i < numRingtones; i++) {
        if (strncmp(ringtone[i].rtttl, rtttl, 16) == 0) {
            rtIndex = i;
            break;
        }
    }
    if (rtIndex != 0)
        db.uiConfig.ring_tone_id = rtIndex;
    if (db.uiConfig.ring_tone_id == 0)
        db.uiConfig.ring_tone_id = 1;
    db.silent = rtIndex == 0;

    // update home panel bell text
    setBellText(db.uiConfig.alert_enabled, !db.silent);
    bool off = !db.uiConfig.alert_enabled && db.silent;
    Themes::recolorButton(objects.home_bell_button, !off);
    Themes::recolorText(objects.home_bell_label, !off);
    objects.home_bell_button->user_data = (void *)off;
}

void TFTView_320x240::updateTime(uint32_t timeVal)
{
    time_t localtime;
    time(&localtime);

    if (VALID_TIME(localtime)) {
        if (actTime != localtime) {
            ILOG_DEBUG("update (local)time: %d -> %d", actTime, localtime);
            actTime = localtime;
        }
    } else {
        if (timeVal > actTime) {
            ILOG_DEBUG("update (act)time: %d -> %d", actTime, timeVal);
            actTime = timeVal;
        }
    }
}

/**
 * @brief Create a new container for a node or group channel if it does not exist
 *
 * @param from
 * @param to: UINT32_MAX for broadcast, ownNode (= us) otherwise
 * @param channel
 */
lv_obj_t *TFTView_320x240::newMessageContainer(uint32_t from, uint32_t to, uint8_t ch)
{
    if (to == UINT32_MAX || from == 0) {
        if (channelGroup[ch] != nullptr)
            return channelGroup[ch];
    } else {
        auto it = messages.find(from);
        if (it != messages.end() && it->second)
            return it->second;
    }

    // create container for new messages
    lv_obj_t *container = lv_obj_create(objects.messages_panel);
    lv_obj_remove_style_all(container);
    lv_obj_set_width(container, lv_pct(100));
    lv_obj_set_height(container, lv_pct(88));
    lv_obj_set_x(container, 0);
    lv_obj_set_y(container, 0);
    lv_obj_set_align(container, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(container, lv_obj_flag_t(LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                                               LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLL_ELASTIC)); /// Flags
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_ACTIVE);
    lv_obj_set_scroll_dir(container, LV_DIR_VER);
    lv_obj_set_style_pad_left(container, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(container, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // store new message container
    if (to == UINT32_MAX || from == 0) {
        channelGroup[ch] = container;
    } else {
        messages[from] = container;
    }

    // optionally add chat to chatPanel to access the container
    addChat(from, to, ch);

    return container;
}

/**
 * @brief insert a mew message that arrived into a <channel group> or <from node> container
 *
 * @param from source node
 * @param to destination node
 * @param ch channel
 * @param size length of msg
 * @param msg text message
 * @param time in/out: message time (maybe overwritten when 0)
 * @param restore if restoring then skip banners and highlight
 */
void TFTView_320x240::newMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t &msgTime, bool restore)
{
    ILOG_DEBUG("newMessage: from:0x%08x, to:0x%08x, ch:%d, time:%d", from, to, ch, msgTime);
    int pos = 0;
    char buf[284]; // 237 + 4 + 40 + 2 + 1
    lv_obj_t *container = nullptr;
    if (to == UINT32_MAX) { // message for group, prepend short name to msg
        if (nodes.find(from) == nodes.end()) {
            pos += sprintf(buf, "%04x ", from & 0xffff);
        } else {
            // original short name is held in userData, extract it and add msg
            char *userData = (char *)&(nodes[from]->LV_OBJ_IDX(node_lbs_idx)->user_data);
            while (pos < 4 && userData[pos] != 0) {
                buf[pos] = userData[pos];
                pos++;
            }
        }
        buf[pos++] = ' ';
        container = channelGroup[ch];
    } else { // message for us
        container = messages[from];
    }

    // if it's the first message we need a container
    if (!container) {
        container = newMessageContainer(from, to, ch);
    }

    pos += timestamp(&buf[pos], msgTime, !restore);
    sprintf(&buf[pos], "%s", msg);

    // place message into container
    newMessage(from, container, ch, buf);

    if (!restore) {
        // display msg popup if not already viewing the messages
        if (container != activeMsgContainer || activePanel != objects.messages_panel) {
            unreadMessages++;
            updateUnreadMessages();
            if (activePanel != objects.messages_panel && db.uiConfig.alert_enabled) {
                showMessagePopup(from, to, ch, lv_label_get_text(nodes[from]->LV_OBJ_IDX(node_lbl_idx)));
            }
            lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
        }
        if (container != activeMsgContainer)
            highlightChat(from, to, ch);
    } else {
        if (container != activeMsgContainer)
            lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
    }
}

/**
 * @brief Display message bubble in related message container
 *
 * @param nodeNum
 * @param container
 * @param ch
 * @param msg
 */
void TFTView_320x240::newMessage(uint32_t nodeNum, lv_obj_t *container, uint8_t ch, const char *msg)
{
    lv_obj_t *hiddenPanel = lv_obj_create(container);
    lv_obj_set_width(hiddenPanel, lv_pct(100));
    lv_obj_set_height(hiddenPanel, LV_SIZE_CONTENT); /// 50
    lv_obj_set_align(hiddenPanel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(hiddenPanel, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_radius(hiddenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    add_style_panel_style(hiddenPanel);
    lv_obj_set_style_pad_left(hiddenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(hiddenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(hiddenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(hiddenPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *msgLabel = lv_label_create(hiddenPanel);
    // calculate expected size of text bubble, to make it look nicer
    lv_coord_t width = lv_txt_get_width(msg, strlen(msg), &ui_font_montserrat_12, 0);
    lv_obj_set_width(msgLabel, std::max<int32_t>(std::min<int32_t>((int32_t)(width), 160) + 10, 40));
    lv_obj_set_height(msgLabel, LV_SIZE_CONTENT);
    lv_obj_set_align(msgLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(msgLabel, msg);
    add_style_new_message_style(msgLabel);

    lv_obj_scroll_to_view(hiddenPanel, LV_ANIM_ON);
    lv_obj_move_foreground(objects.message_input_area);
}

/**
 * restore messages from persistent log
 */
void TFTView_320x240::restoreMessage(const LogMessage &msg)
{
    ((uint8_t *)msg.bytes)[msg._size] = 0;
    ILOG_DEBUG("restoring msg from:0x%08x, to:0x%08x, ch:%d, time:%d, status:%d, trash:%d, size:%d, '%s'", msg.from, msg.to,
               msg.ch, msg.time, (int)msg.status, msg.trashFlag, msg._size, msg.bytes);

    if (msg.from == ownNode) {
        lv_obj_t *container = nullptr;
        if (msg.to == UINT32_MAX) {
            if (msg.trashFlag && chats.find(msg.ch) != chats.end()) {
                ILOG_DEBUG("trashFlag set for channel %d", msg.ch);
                eraseChat(msg.ch);
                return;
            } else {
                container = newMessageContainer(msg.from, msg.to, msg.ch);
            }
        } else {
            if (nodes.find(msg.to) != nodes.end()) {
                if (msg.trashFlag && chats.find(msg.to) != chats.end()) {
                    ILOG_DEBUG("trashFlag set for node %08x", msg.to);
                    eraseChat(msg.to);
                    return;
                } else {
                    container = newMessageContainer(msg.to, msg.from, msg.ch);
                }
            } else {
                ILOG_DEBUG("to node 0x%08x not in db", msg.to);
                MeshtasticView::addOrUpdateNode(msg.to, msg.ch, 0, eRole::unknown, false, false);
            }
        }
        if (container) {
            if (container != activeMsgContainer)
                lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
            addMessage(container, msg.time, 0, (char *)msg.bytes, msg.status);
        }
    } else if (nodes.find(msg.from) != nodes.end()) {
        if (msg.trashFlag && chats.find(msg.from) != chats.end()) {
            ILOG_DEBUG("trashFlag set for node %08x", msg.from);
            eraseChat(msg.from);
            return;
        } else {
            uint32_t time = msg.time ? msg.time : UINT32_MAX; // don't overwrite 0 with actual time
            newMessage(msg.from, msg.to, msg.ch, (const char *)msg.bytes, time);
        }
    } else {
        int pos = 0;
        char buf[284]; // 237 + 4 + 40 + 2 + 1
        if (msg.to != UINT32_MAX) {
            // from node not in db
            ILOG_DEBUG("from node 0x%08x not in db", msg.from);
            MeshtasticView::addOrUpdateNode(msg.from, msg.ch, 0, eRole::unknown, false, false);
        } else {
            ILOG_DEBUG("from node 0x%08x not in db and no need to insert", msg.from);
            pos += sprintf(buf, "%04x ", msg.from & 0xffff);
        }
        uint32_t len = timestamp(buf + pos, msg.time, false);
        memcpy(buf + pos + len, msg.bytes, msg.size());
        buf[pos + len + msg.size()] = 0;

        lv_obj_t *container = newMessageContainer(msg.from, msg.to, msg.ch);
        lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
        newMessage(msg.from, container, msg.ch, buf);
    }
}

/**
 * @brief Add a new chat to the chat panel to access the message container
 *
 * @param from
 * @param to
 * @param ch
 */
void TFTView_320x240::addChat(uint32_t from, uint32_t to, uint8_t ch)
{
    uint32_t index = ((to == UINT32_MAX || from == 0) ? ch : from);
    auto it = chats.find(index);
    if (it != chats.end())
        return;

    lv_obj_t *chatDelBtn = nullptr;
    lv_obj_t *parent_obj = objects.chats_panel;

    // ChatsButton
    lv_obj_t *chatBtn = lv_btn_create(parent_obj);
    lv_obj_set_pos(chatBtn, 0, 0);
    lv_obj_set_size(chatBtn, LV_PCT(100), 36);
    lv_obj_add_flag(chatBtn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(chatBtn, LV_OBJ_FLAG_SCROLLABLE);
    add_style_home_button_style(chatBtn);
    lv_obj_set_style_align(chatBtn, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(chatBtn, colorMidGray, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(chatBtn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(chatBtn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(chatBtn, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(chatBtn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(chatBtn, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(chatBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(chatBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(chatBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(chatBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(chatBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    char buf[64];
    if (to == UINT32_MAX || from == 0) {
        sprintf(buf, "%d: %s", (int)ch, lv_label_get_text(channel[ch]));
    } else {
        auto it = nodes.find(from);
        if (it != nodes.end()) {
            sprintf(buf, "%s: %s", lv_label_get_text(it->second->LV_OBJ_IDX(node_lbs_idx)),
                    lv_label_get_text(it->second->LV_OBJ_IDX(node_lbl_idx)));
        } else {
            sprintf(buf, "!%08x", from);
        }
    }

    {
        lv_obj_t *parent_obj = chatBtn;
        {
            // ChatsButtonLabel
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.chats_button_label = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            lv_label_set_text(obj, buf);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // ChatDelButton
            lv_obj_t *obj = lv_btn_create(parent_obj);
            chatDelBtn = obj;
            lv_obj_set_pos(obj, -3, -1);
            lv_obj_set_size(obj, 40, 23);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, colorDarkRed, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // DelLabel
                    lv_obj_t *chatDelBtn = lv_label_create(parent_obj);
                    lv_obj_set_pos(chatDelBtn, 0, 0);
                    lv_obj_set_size(chatDelBtn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(chatDelBtn, _("DEL"));
                    lv_obj_set_style_align(chatDelBtn, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }

    chats[index] = chatBtn;
    updateActiveChats();
    if (index > c_max_channels) {
        if (nodes.find(index) != nodes.end())
            applyNodesFilter(index);
    }

    lv_obj_add_event_cb(chatBtn, ui_event_ChatButton, LV_EVENT_ALL, (void *)index);
    lv_obj_add_event_cb(chatDelBtn, ui_event_ChatDelButton, LV_EVENT_CLICKED, (void *)index);
}

void TFTView_320x240::highlightChat(uint32_t from, uint32_t to, uint8_t ch)
{
    uint32_t index = ((to == UINT32_MAX || from == 0) ? ch : from);
    auto it = chats.find(index);
    if (it != chats.end()) {
        // mark chat in color
        lv_obj_set_style_border_color(it->second, colorOrange, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

void TFTView_320x240::updateActiveChats(void)
{
    char buf[40];
    sprintf(buf, _p("%d active chat(s)", chats.size()), chats.size());
    lv_label_set_text(objects.top_chats_label, buf);
}

/**
 * @brief Display banner showing to be patient while restoring messages
 */
void TFTView_320x240::notifyRestoreMessages(int32_t percentage)
{
    lv_bar_set_value(objects.message_restore_bar, percentage, LV_ANIM_OFF);
}

void TFTView_320x240::notifyMessagesRestored(void)
{
    MeshtasticView::notifyMessagesRestored();
    lv_obj_add_flag(objects.msg_restore_panel, LV_OBJ_FLAG_HIDDEN);
    updateActiveChats();
    updateNodesFiltered(true);
}

/**
 * @brief display new message popup panel
 *
 * @param from sender (NULL for removing popup)
 * @param to individual or group message
 * @param ch received channel
 */
void TFTView_320x240::showMessagePopup(uint32_t from, uint32_t to, uint8_t ch, const char *name)
{
    if (name) {
        static char buf[64];
        sprintf(buf, _("New message from \n%s"), name);
        buf[38] = '\0'; // cut too long userName
        lv_label_set_text(objects.msg_popup_label, buf);
        if (to == UINT32_MAX)
            objects.msg_popup_button->user_data = (void *)(uint32_t)ch; // store the channel in the button's data
        else
            objects.msg_popup_button->user_data = (void *)from; // store the node in the button's data
        lv_obj_clear_flag(objects.msg_popup_panel, LV_OBJ_FLAG_HIDDEN);

        if (db.module_config.external_notification.alert_message)
            lv_disp_trig_activity(NULL);

        lv_group_focus_obj(objects.msg_popup_button);
    }
}

void TFTView_320x240::hideMessagePopup(void)
{
    lv_obj_add_flag(objects.msg_popup_panel, LV_OBJ_FLAG_HIDDEN);
}

/**
 * @brief Display messages of a group channel
 *
 * @param ch
 */
void TFTView_320x240::showMessages(uint8_t ch)
{
    if (!messagesRestored) {
        // display message restoration progress banner
        lv_obj_clear_flag(objects.msg_popup_panel, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.msg_popup_button);
        return;
    }

    lv_obj_add_flag(activeMsgContainer, LV_OBJ_FLAG_HIDDEN);
    activeMsgContainer = channelGroup[ch];
    if (!activeMsgContainer) {
        activeMsgContainer = newMessageContainer(0, UINT32_MAX, ch);
    }

    activeMsgContainer->user_data = (void *)(uint32_t)ch;
    lv_obj_clear_flag(activeMsgContainer, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(objects.top_group_chat_label, lv_label_get_text(channel[ch]));
    ui_set_active(objects.messages_button, objects.messages_panel, objects.top_group_chat_panel);
}

/**
 * @brief Display messages from a node
 *
 * @param nodeNum
 */
void TFTView_320x240::showMessages(uint32_t nodeNum)
{
    lv_obj_add_flag(activeMsgContainer, LV_OBJ_FLAG_HIDDEN);
    activeMsgContainer = messages[nodeNum];
    if (!activeMsgContainer) {
        activeMsgContainer = newMessageContainer(nodeNum, 0, 0);
    }
    activeMsgContainer->user_data = (void *)nodeNum;
    lv_obj_clear_flag(activeMsgContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_t *p = nodes[nodeNum];
    if (p) {
        lv_label_set_text(objects.top_messages_node_label, lv_label_get_text(p->LV_OBJ_IDX(node_lbl_idx)));
        ui_set_active(objects.messages_button, objects.messages_panel, objects.top_messages_panel);
        switch ((unsigned long)p->LV_OBJ_IDX(node_bat_idx)->user_data) {
        case 0:
            lv_obj_set_style_bg_image_src(objects.top_messages_node_image, &img_lock_channel_image,
                                          LV_PART_MAIN | LV_STATE_DEFAULT);
            break;
        case 1:
            lv_obj_set_style_bg_image_src(objects.top_messages_node_image, &img_lock_secure_image,
                                          LV_PART_MAIN | LV_STATE_DEFAULT);
            break;
        default:
            lv_obj_set_style_bg_image_src(objects.top_messages_node_image, &img_lock_slash_image,
                                          LV_PART_MAIN | LV_STATE_DEFAULT);
            break;
        }
        unreadMessages = 0; // TODO: not all messages may be actually read
        updateUnreadMessages();
    } else {
        // TODO: log error
    }
}

/**
 * @brief Place keyboard at a suitable space above or below the text input area
 *
 * @param textArea
 */
void TFTView_320x240::showKeyboard(lv_obj_t *textArea)
{
    lv_area_t text_coords, kb_coords;
    lv_obj_get_coords(textArea, &text_coords);
    lv_obj_get_coords(objects.keyboard, &kb_coords);
    uint32_t kb_h = kb_coords.y2 - kb_coords.y1;
    uint32_t v = lv_display_get_vertical_resolution(displaydriver->getDisplay());

    if (text_coords.y1 > kb_h + 30) {
        // if enough place above put under top panel
        lv_obj_set_pos(objects.keyboard, 0, 28);
    } else if ((text_coords.y1 + 10) > v / 2) {
        // if text area is at lower half then place above text area
        lv_obj_set_pos(objects.keyboard, 0, text_coords.y1 - kb_h - 2);
    } else {
        // place below text area
        lv_obj_set_pos(objects.keyboard, 0, text_coords.y2 + 3);
    }

    lv_keyboard_set_textarea(objects.keyboard, textArea);
}

lv_obj_t *TFTView_320x240::showQrCode(lv_obj_t *parent, const char *data)
{
    lv_color_t bg_color = colorMesh;
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);
    qr = lv_qrcode_create(parent);
    int32_t size = std::min<int32_t>(lv_obj_get_width(parent), lv_obj_get_height(parent)) - 8;
    lv_qrcode_set_size(qr, size);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_center(qr);
    lv_obj_set_style_border_color(qr, fg_color, 0);
    lv_obj_set_style_border_width(qr, 4, 0);
    return qr;
}

/**
 * Enable underlying panel, buttons and scrollbar after it was disabled
 */
void TFTView_320x240::enablePanel(lv_obj_t *panel)
{
    lv_obj_clear_state(panel, LV_STATE_DISABLED);
    lv_obj_set_scrollbar_mode(panel, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_add_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    auto enableButtons = [](lv_obj_t *obj, void *) -> lv_obj_tree_walk_res_t {
        if (obj->class_p == &lv_button_class) {
            lv_obj_clear_state(obj, LV_STATE_DISABLED);
        }
        return LV_OBJ_TREE_WALK_NEXT;
    };

    lv_obj_tree_walk(panel, enableButtons, NULL);
}

/**
 * Disable underlying panel with it's children buttons and scrollbar
 */
void TFTView_320x240::disablePanel(lv_obj_t *panel)
{
    lv_obj_add_state(panel, LV_STATE_DISABLED);
    lv_obj_set_scrollbar_mode(panel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    auto disableButtons = [](lv_obj_t *obj, void *) -> lv_obj_tree_walk_res_t {
        if (obj->class_p == &lv_button_class) {
            lv_obj_add_state(obj, LV_STATE_DISABLED);
        }
        return LV_OBJ_TREE_WALK_NEXT;
    };

    lv_obj_tree_walk(panel, disableButtons, NULL);
}

/**
 * Set focus to first button of a panel
 */
void TFTView_320x240::setGroupFocus(lv_obj_t *panel)
{
    if (panel == objects.home_panel) {
        lv_group_focus_obj(objects.home_mail_button);
    } else if (panel == objects.nodes_panel) {
        lv_group_focus_obj(objects.node_button);
    } else if (panel == objects.groups_panel) {
        lv_group_focus_obj(objects.channel_button0);
    } else if (panel == objects.messages_panel) {
        lv_group_focus_obj(objects.message_input_area);
    } else if (panel == objects.chats_panel) {
        if (chats.size() > 0) {
            lv_group_focus_obj(panel->spec_attr->children[1]); // TODO: does not work
        }
    } else if (panel == objects.map_panel) {

    } else if (panel == objects.settings_screen_lock_panel) {
        lv_group_focus_obj(objects.screen_lock_button_matrix);
    } else if (panel == objects.controller_panel) {
        lv_group_focus_obj(objects.basic_settings_user_button);
    } else {
        for (int i = 0; i < lv_obj_get_child_count(panel); i++) {
            if (panel->spec_attr->children[i]->class_p == &lv_button_class) {
                lv_group_focus_obj(panel->spec_attr->children[i]);
                break;
            }
        }
    }
}

/**
 * input group used by keyboard and/or pointer for dynamic assignment
 */
void TFTView_320x240::setInputGroup(void)
{
    lv_group_t *group = lv_group_get_default();

    if (group && inputdriver->hasKeyboardDevice())
        lv_indev_set_group(inputdriver->getKeyboard(), group);

    if (group && inputdriver->hasPointerDevice())
        lv_indev_set_group(inputdriver->getPointer(), group);
}

void TFTView_320x240::setInputButtonLabel(void)
{
    // update input button label
    std::string current_kbd = inputdriver->getCurrentKeyboardDevice();
    std::string current_ptr = inputdriver->getCurrentPointerDevice();

    char label[40];
    lv_snprintf(label, sizeof(label), _("Input Control: %s/%s"), current_ptr.c_str(), current_kbd.c_str());
    lv_label_set_text(objects.basic_settings_input_label, label);
}
// -------- helpers --------

void TFTView_320x240::removeNode(uint32_t nodeNum)
{
    auto it = nodes.find(nodeNum);
    if (it != nodes.end()) {
    }
}

void TFTView_320x240::setNodeImage(uint32_t nodeNum, eRole role, bool viaMqtt, lv_obj_t *img)
{
    uint32_t bgColor, fgColor;
    std::tie(bgColor, fgColor) = nodeColor(nodeNum);
    // if (viaMqtt) {
    //     lv_image_set_src(img, &//TODO );
    // }
    // else
    switch (role) {
    case client:
    case client_mute:
    case client_hidden:
    case tak: {
        lv_image_set_src(img, &img_node_client_image);
        break;
    }
    case router_client: {
        lv_image_set_src(img, &img_top_nodes_image);
        break;
    }
    case repeater:
    case router: {
        lv_image_set_src(img, &img_node_router_image);
        break;
    }
    case tracker:
    case sensor:
    case lost_and_found:
    case tak_tracker: {
        lv_image_set_src(img, &img_node_sensor_image);
        break;
    }
    case unknown: {
        lv_image_set_src(img, &img_circle_question_image);
        break;
    }
    default:
        lv_image_set_src(img, &img_node_client_image);
        break;
    }

    lv_obj_set_style_bg_color(img, lv_color_hex(bgColor), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(img, lv_color_hex(bgColor), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(img, fgColor ? 0 : 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void TFTView_320x240::updateNodesStatus(void)
{
    char buf[40];
    lv_snprintf(buf, sizeof(buf), _p("%d of %d nodes online", nodeCount), nodesOnline, nodeCount);
    lv_label_set_text(objects.home_nodes_label, buf);

    if (nodesFiltered)
        lv_snprintf(buf, sizeof(buf), _("Filter: %d of %d nodes"), nodeCount - nodesFiltered, nodeCount);
    lv_label_set_text(objects.top_nodes_online_label, buf);
}

/**
 * @brief Dynamically update all nodes filter and highlight
 *        Because the update can take quite some time (tens of ms) it is done in smaller
 *        chunks of 10 nodes per invocation, so it must be periodically called
 *        TODO: check for side effects if new nodes are inserted or removed during filter processing
 * @param reset indicates to start update from beginning of node list otherwise
 *        continue with iterator position or skip if done
 */
void TFTView_320x240::updateNodesFiltered(bool reset)
{
    static auto it = nodes.begin();
    if (reset || nodesChanged) {
        nodesFiltered = 0;
        nodesChanged = false;
        processingFilter = true;
        it = nodes.begin();
    }

    for (int i = 0; i < 10 && it != nodes.end(); i++) {
        applyNodesFilter(it->first, true);
        it++;
    }

    if (it == nodes.end()) {
        processingFilter = false;
    }
    updateNodesStatus();
}

/**
 * @brief Update last heard display/user_data/counter to current time
 *
 * @param nodeNum
 */
void TFTView_320x240::updateLastHeard(uint32_t nodeNum)
{
    auto it = nodes.find(nodeNum);
    if (it != nodes.end() && it->second) {
        time_t lastHeard = (time_t)it->second->LV_OBJ_IDX(node_lh_idx)->user_data;
        it->second->LV_OBJ_IDX(node_lh_idx)->user_data = (void *)curtime;
        lv_label_set_text(it->second->LV_OBJ_IDX(node_lh_idx), _("now"));
        if (it->first != ownNode) {
            if (lastHeard > 0 && curtime - lastHeard >= secs_until_offline) {
                nodesOnline++;
                applyNodesFilter(nodeNum);
                updateNodesStatus();
            }
            // move to top position
            lv_obj_move_to_index(it->second, 1);

            // re-arrange the group linked list i.e. move the node after the top position
            lv_ll_t *lv_group_ll = &lv_group_get_default()->obj_ll;
            void *act = it->second->LV_OBJ_IDX(node_btn_idx)->user_data;
            if (lv_group_ll && act)
                _lv_ll_move_before(lv_group_ll, act, _lv_ll_get_next(lv_group_ll, topNodeLL));
        }
    }
}

/**
 * @brief update last heard display for all nodes; also update nodes online
 *
 */
void TFTView_320x240::updateAllLastHeard(void)
{
    uint16_t online = 0;
    time_t lastHeard;
    for (auto it : nodes) {
        char buf[32];
        if (it.first == ownNode) { // own node is always now, so do update
            lastHeard = curtime;
            it.second->LV_OBJ_IDX(node_lh_idx)->user_data = (void *)lastHeard;
        } else {
            lastHeard = (time_t)it.second->LV_OBJ_IDX(node_lh_idx)->user_data;
        }
        if (lastHeard) {
            bool isOnline = lastHeardToString(lastHeard, buf);
            lv_label_set_text(it.second->LV_OBJ_IDX(node_lh_idx), buf);
            if (isOnline)
                online++;
        }
    }
    nodesOnline = online;
    updateNodesFiltered(true);
    updateNodesStatus();
}

void TFTView_320x240::updateUnreadMessages(void)
{
    char buf[32];
    if (unreadMessages > 0) {
        sprintf(buf, unreadMessages == 1 ? _("%d new message") : _("%d new messages"), unreadMessages);
        lv_obj_set_style_bg_img_src(objects.home_mail_button, &img_home_mail_unread_button_image,
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        strcpy(buf, _("no new messages"));
        lv_obj_set_style_bg_img_src(objects.home_mail_button, &img_home_mail_button_image, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_label_set_text(objects.home_mail_label, buf);
}

/**
 * @brief Called once a second to update time label
 *
 */
void TFTView_320x240::updateTime(void)
{
    char buf[80];
    time_t curr_time;
#ifdef ARCH_PORTDUINO
    time(&curr_time);
#else
    curr_time = actTime;
#endif
    tm *curr_tm = localtime(&curr_time);

    int len = 0;
    if (VALID_TIME(curr_time) && (unsigned long)objects.home_time_button->user_data == 0) {
        len = strftime(buf, 40, "%T %Z\n%a %d-%b-%g", curr_tm);
    } else {
        uint32_t uptime = millis() / 1000;
        int hours = uptime / 3600;
        uptime -= hours * 3600;
        int minutes = uptime / 60;
        int seconds = uptime - minutes * 60;

        sprintf(&buf[len], _("uptime: %02d:%02d:%02d"), hours, minutes, seconds);
    }
    lv_label_set_text(objects.home_time_label, buf);
}

void TFTView_320x240::updateSDCard(void)
{
#if !defined(ARCH_PORTDUINO) && HAS_SDCARD
    char buf[64];
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        lv_snprintf(buf, sizeof(buf), _("no SD card detected"));
        Themes::recolorButton(objects.home_sd_card_button, false);
        Themes::recolorText(objects.home_sd_card_label, false);
    } else {
        uint32_t usedSpace = SD.usedBytes() / (1024 * 1024);
        uint32_t totalSpace = SD.cardSize() / (1024 * 1024);
        float totalSpaceGB = totalSpace / 1024.0f;

        sprintf(buf, _("%s (%0.1f GB)\nUsed: %d MB (%d%%)"),
                cardType == CARD_MMC    ? "MMC"
                : cardType == CARD_SD   ? "SDSC"
                : cardType == CARD_SDHC ? "SDHC"
                                        : "Unknown",
                totalSpaceGB, usedSpace, ((usedSpace * 100) + totalSpace / 2) / totalSpace);
        Themes::recolorButton(objects.home_sd_card_button, true);
        Themes::recolorText(objects.home_sd_card_label, true);
    }
    lv_label_set_text(objects.home_sd_card_label, buf);
#else
    lv_obj_add_flag(objects.home_sd_card_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.home_sd_card_label, LV_OBJ_FLAG_HIDDEN);
#endif
}

void TFTView_320x240::updateFreeMem(void)
{
    // only update if HomePanel is active (since this is some critical code that did crash sporadically)
    if (activePanel == objects.home_panel && (unsigned long)objects.home_memory_button->user_data) {
        char buf[64];
        uint32_t freeHeap = 0;
        uint32_t freeHeap_pct = 0;

        lv_mem_monitor_t mon;
        lv_mem_monitor(&mon);

#ifdef ARDUINO_ARCH_ESP32
        freeHeap = ESP.getFreeHeap();
        freeHeap_pct = 100 * freeHeap / ESP.getHeapSize();
        sprintf(buf, _("Heap: %d (%d%%)\nLVGL: %d (%d%%)"), freeHeap, freeHeap_pct, mon.free_size, 100 - mon.used_pct);
#elif defined(ARCH_PORTDUINO)
        static uint32_t totalMem = LinuxHelper::getTotalMem();
        if (totalMem != 0) {
            freeHeap = LinuxHelper::getAvailableMem();
            freeHeap_pct = 100 * freeHeap / totalMem;
        }
        sprintf(buf, _("Heap: %d (%d%%)\nLVGL: %d (%d%%)"), freeHeap, freeHeap_pct, mon.free_size / 1024, 100 - mon.used_pct);
#else
        buf[0] = '\0';
#endif
        lv_label_set_text(objects.home_memory_label, buf);
    }
}

void TFTView_320x240::task_handler(void)
{
    MeshtasticView::task_handler();

    if (screensInitialised) {
        if (curtime - lastrun1 >= 1) { // call every 1s
            lastrun1 = curtime;
            actTime++;
            updateTime();

            if (curtime - lastrun5 >= 5) { // call every 5s
                lastrun5 = curtime;
                if (scans > 0 && activePanel == objects.signal_scanner_panel) {
                    scanSignal(scans);
                    scans--;
                }
                if (startTime) {
                    if (curtime - startTime > 30) {
                        lv_label_set_text(objects.trace_route_start_label, _("Start"));
                        lv_obj_set_style_outline_color(objects.trace_route_start_button, colorMesh,
                                                       LV_PART_MAIN | LV_STATE_DEFAULT);
                        removeSpinner();
                    } else {
                        char buf[16];
                        sprintf(buf, "%ds", ((35 - (curtime - startTime)) / 5) * 5);
                        lv_label_set_text(objects.trace_route_start_label, buf);
                    }
                }
            }
            if (curtime - lastrun10 >= 10) { // call every 10s
                lastrun10 = curtime;
                updateFreeMem();

                if ((db.config.network.wifi_enabled || db.module_config.mqtt.enabled) && !displaydriver->isPowersaving()) {
                    controller->requestDeviceConnectionStatus();
                }
            }
            if (curtime - lastrun60 >= 60) { // call every 60s
                lastrun60 = curtime;
                updateAllLastHeard();

                if (detectorRunning) {
                    controller->sendPing();
                }

                // if we didn't hear any node for 1h assume we have no signal
                if (curtime - lastHeard > secs_until_offline) {
                    lv_obj_set_style_bg_image_src(objects.home_signal_button, &img_home_no_signal_image,
                                                  LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(objects.home_signal_label, _("no signal"));
                    lv_label_set_text(objects.home_signal_pct_label, "");
                }
            }
        }
        if (processingFilter || nodesChanged) {
            updateNodesFiltered(nodesChanged);
        }
    } else { // CYD scenario only
        if (state == MeshtasticView::eBooting) {
            if (curtime - lastrun1 >= 1) { // call every 1s
                lastrun1 = curtime;
                updateBootMessage();
            }
        }
    }
}

// === lvgl C style callbacks ===

extern "C" {

void action_on_boot_screen_displayed(lv_event_t *e)
{
    ILOG_DEBUG("action_on_boot_screen_displayed()");
}
}

#endif
