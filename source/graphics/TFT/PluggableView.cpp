#if defined(VIEW_160x80) || defined(VIEW_320x170) || defined(VIEW_480x222) || defined(VIEW_410x502) || defined(VIEW_240x135) ||  \
    defined(VIEW_240x320)

#include "graphics/view/TFT/PluggableView.h"
#include "graphics/common/BatteryLevel.h"
#include "graphics/common/LoRaPresets.h"
#include "graphics/common/ViewController.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverFactory.h"
#include "graphics/plugin/ListRowStyle.h"
#include "images.h"
#include "input/InputDriver.h"
#include "lv_i18n.h"
#include "styles.h"
#include "ui.h"
#include "util/Colors.h"
#include "util/ILog.h"
#include <cstdio>
#include <ctime>
#include <functional>
#include <time.h>

#if defined(ARCH_PORTDUINO)
#include "PortduinoFS.h"
fs::FS &fileSystem = PortduinoFS;
#else
#include "LittleFS.h"
fs::FS &fileSystem = LittleFS;
#endif

#if defined(ARCH_PORTDUINO)
#include "util/LinuxHelper.h"
#endif
#include "graphics/common/SdCard.h"

#define THIS PluggableView::instance() // need to use this in all static methods

#define VALID_TIME(T) (T > 1000000 && T < UINT32_MAX)

PluggableView *PluggableView::gui = nullptr;

PluggableView::PluggableView(const DisplayDriverConfig *cfg, DisplayDriver *driver)
    : MeshtasticView(cfg, driver, new ViewController), actTime(0), uptime(0), lastHeard(0), hasPosition(false),
      screensInitialised(false), myLatitude(0), myLongitude(0), formatSD(false), db{}
{
}

PluggableView *PluggableView::instance(void)
{
    // has been initialized in derived class
    return gui;
}

void PluggableView::init(IClientBase *client)
{
    ILOG_DEBUG("PluggableView init...");
    MeshtasticView::init(client);

#if defined(INPUTDRIVER_ROTARY_TYPE) || defined(INPUTDRIVER_ENCODER_TYPE)
    indev = InputDriver::instance()->getEncoder();
#elif LV_USE_LIBINPUT
    indev = InputDriver::instance()->getPointer();
#endif

    // Decorative boot widgets must never become encoder targets.
    lv_group_t *bootGroup = lv_group_get_default();
    lv_group_set_default(nullptr);
    ui_init_boot();
    lv_group_set_default(bootGroup);

    time(&lastrun60);
    time(&lastrun10);
    time(&lastrun5);
    time(&lastrun1);

    lv_timer_create(timer_event_bootscreen_done, 3000, NULL);
}

void PluggableView::timer_event_bootscreen_done(lv_timer_t *timer)
{
    if (THIS->state == eBooting)
        THIS->state = MeshtasticView::eBootScreenDone;
    lv_timer_delete(timer);
}

/**
 * @brief initialize UI with persistent data
 */
bool PluggableView::setupUIConfig(const meshtastic_DeviceUIConfig &uiconfig)
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

    state = MeshtasticView::eSetupUIConfig;

    // now we have set language, continue creating all screens
    if (!screensInitialised)
        init_screens();

    // initialize own node panel
    if (ownNode && objects.node_button) {
        nodes[ownNode] = objects.node_button;
        node->bindRow(objects.node_button, ownNode, 0);
    }
    setMyInfo(ownNode);
    applyUIConfig();
    if (db.uiConfig.screen_lock && settings)
        settings->unlockScreen([this]() {
            if (messages)
                messages->setNotificationsSuppressed(false);
        });

    // check SD card
    updateSDCard();

    lv_disp_trig_activity(NULL);
    return true;
}

void PluggableView::applyUIConfig()
{
    displaydriver->setBrightness(db.uiConfig.screen_brightness);
    displaydriver->setScreenTimeout(db.uiConfig.screen_timeout);
    if (!screensInitialised)
        return;
    const bool light = db.uiConfig.theme == meshtastic_Theme_LIGHT;
    const bool red = db.uiConfig.theme == meshtastic_Theme_RED;
    lv_display_t *display = lv_display_get_default();
    lv_display_set_theme(display, lv_theme_default_init(display, lv_palette_main(red ? LV_PALETTE_RED : LV_PALETTE_BLUE),
                                                        lv_palette_main(LV_PALETTE_GREEN), !light, LV_FONT_DEFAULT));
    ListRowStyle::setTheme(db.uiConfig.theme);
    const auto background = ListRowStyle::background();
    const auto foreground = ListRowStyle::foreground();
    auto *pageStyle = get_style_page_style_MAIN_DEFAULT();
    lv_style_set_bg_color(pageStyle, background);
    lv_style_set_text_color(pageStyle, foreground);
    lv_obj_report_style_change(pageStyle);
    auto *header = get_style_top_panel_style_MAIN_DEFAULT();
    lv_style_set_bg_color(header, lv_color_hex(red ? 0x542020 : light ? 0x67ea94 : 0x37507e));
    lv_style_set_text_color(header, foreground);
    lv_obj_report_style_change(header);
    for (auto *screen : {objects.menu, objects.home, objects.nodes, objects.groups, objects.chats, objects.map, objects.clock,
                         objects.settings}) {
        if (screen) {
            lv_obj_set_style_bg_color(screen, background, 0);
            lv_obj_set_style_text_color(screen, foreground, 0);
        }
    }
    if (messages)
        messages->setNotificationsEnabled(db.uiConfig.alert_enabled);
    refreshSettingsStatus();
}

void PluggableView::updateUIConfig(const meshtastic_DeviceUIConfig &config)
{
    if (config.version != 1)
        return;
    // Keep all persisted fields, but leave screens, chat drafts and focus intact.
    const bool lockEnabled = !db.uiConfig.screen_lock && config.screen_lock;
    db.uiConfig = config;
    applyUIConfig();
    if (lockEnabled && settings)
        settings->unlockScreen([this]() {
            if (messages)
                messages->setNotificationsSuppressed(false);
        });
}

void PluggableView::screenSaving(bool enabled)
{
    // The PIN overlay owns input while the display still wakes on any key.
    if (enabled && db.uiConfig.screen_lock && settings)
        settings->unlockScreen([this]() {
            if (messages)
                messages->setNotificationsSuppressed(false);
        });
}

/**
 * @brief Initialize all screens and apply customizations
 *
 */
void PluggableView::init_screens(void)
{
    ILOG_DEBUG("init screens...");
    state = MeshtasticView::eInitScreens;
    ui_init();
    ui_events_init();
    screensInitialised = true;
    state = MeshtasticView::eInitDone;
    ILOG_DEBUG("PluggableView init done.");
}

/**
 * Assign input groups per screen/plugin.
 */
void PluggableView::ui_init(void)
{
    ILOG_DEBUG("ui_init...");
#ifdef MUI_SCROLLMENU_PLUGIN
    // create and wire menu plugin
    SETUP_INDEV(menuGroup);
    create_screen_menu();
    menu = new ScrollMenuPlugin();
    menu->init(objects.menu, nullptr, ScrollMenuPlugin::WIDGET_COUNT, menuGroup, indev);
#endif
#ifdef MUI_DASHBOARD_PLUGIN
    // create and wire dashboard plugin home screen
    SETUP_INDEV(homeGroup);
    create_screen_home();
    dashboard = new DashboardPlugin();
    dashboard->init(objects.home, nullptr, DashboardPlugin::WIDGET_COUNT, homeGroup, indev);
#endif
#ifdef MUI_NODES_PLUGIN
    // create and wire nodes plugin
    SETUP_INDEV(nodesGroup);
    create_screen_nodes();
    node = new NodesPlugin();
    node->init(objects.nodes, nullptr, NodesPlugin::WIDGET_COUNT, nodesGroup, indev);
#endif
#ifdef MUI_GROUPS_PLUGIN
    // create and wire groups plugin
    SETUP_INDEV(groupsGroup);
    create_screen_groups();
    groups = new GroupsPlugin();
    groups->init(objects.groups, nullptr, GroupsPlugin::WIDGET_COUNT, groupsGroup, indev);
#endif
#ifdef MUI_MESSAGES_PLUGIN
    // create and wire messages plugin
    SETUP_INDEV(chatsGroup);
    create_screen_chats();
    messages = new MessagesPlugin(*this);
    messages->init(objects.chats, nullptr, MessagesPlugin::WIDGET_COUNT, chatsGroup, indev);
#endif
#ifdef MUI_MAP_PLUGIN
    // create and wire map plugin
    SETUP_INDEV(mapGroup);
    create_screen_map();
    map = new MapPlugin();
    map->init(objects.map, nullptr, MapPlugin::WIDGET_COUNT, mapGroup, indev);
    map->setConfig(&db.uiConfig, [this]() { controller->storeUIConfig(db.uiConfig); });
#endif
#ifdef MUI_CLOCK_PLUGIN
    // create and wire clock plugin
    SETUP_INDEV(clockGroup);
    create_screen_clock();
    clock = new ClockPlugin();
    clock->init(nullptr, nullptr, ClockPlugin::WIDGET_COUNT, clockGroup, indev);
#endif
#ifdef MUI_SETTINGS_PLUGIN
    // create and wire settings plugin
    SETUP_INDEV(settingsGroup);
    create_screen_settings();
    lv_obj_add_flag(objects.settings_panel_1, LV_OBJ_FLAG_HIDDEN);
    lv_group_remove_obj(objects.settings_menu);
    settings = new SettingsPlugin(*controller, *inputdriver, db.uiConfig, db.config, db.module_config);
    settings->configure(objects.settings, objects.settings_panel, settingsGroup, indev);
    settings->setOnApplyUI([this](const meshtastic_DeviceUIConfig &cfg) { updateUIConfig(cfg); });
    settings->setOnChanged([this]() { refreshSettingsStatus(); });
    settings->setOnDoubleSpacePeriod([this](bool enabled) {
        if (messages)
            messages->setDoubleSpacePeriod(enabled);
    });
    if (messages)
        messages->setDoubleSpacePeriod(settings->doubleSpacePeriodEnabled());
#endif
    // finishes creating all plugins, goto home menu screen
    menu->loadScreen();
    lv_group_focus_obj(objects.home_button);
}

void PluggableView::ui_events_init(void)
{
    // main button events
#ifdef MUI_DASHBOARD_PLUGIN
    menu->setOnOpenHome(_("Dashboard"), [this](lv_event_t *e) {
        dashboard->loadScreen();
        lv_indev_set_group(THIS->indev, THIS->homeGroup);
        lv_obj_remove_state(objects.nodes_button, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
        lv_group_focus_obj(objects.top_home_back_button);
    });
#endif
#ifdef MUI_NODES_PLUGIN
    menu->setOnOpenNodes(_("Nodes"), [this](lv_event_t *e) {
        node->loadScreen();
        lv_indev_set_group(THIS->indev, THIS->nodesGroup);
        lv_obj_remove_state(objects.nodes_button, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
        lv_group_focus_obj(objects.top_nodes_back_button);
    });
    node->setOnNodeButton([this](lv_event_t *e) {
        uint32_t nodeId = (unsigned long)lv_event_get_user_data(e);
        messages->loadScreen();
        auto row = nodes.find(nodeId);
        const uint8_t channel = row != nodes.end() ? (uintptr_t)lv_obj_get_user_data(row->second) : 0;
        messages->showMessages(nodeId, channel);
    });
#endif
#ifdef MUI_GROUPS_PLUGIN
    menu->setOnOpenGroups(_("Groups"), [this](lv_event_t *e) {
        groups->loadScreen();
        lv_indev_set_group(THIS->indev, THIS->groupsGroup);
        lv_obj_remove_state(objects.groups_button, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
        lv_group_focus_obj(objects.top_groups_back_button);
    });
    groups->setOnGroupButton([this](lv_event_t *e) {
        uint32_t ch = (unsigned long)lv_event_get_user_data(e);
        messages->loadScreen();
        messages->showMessages(0, ch);
    });
#endif
#ifdef MUI_MESSAGES_PLUGIN
    menu->setOnOpenMessages(_("Messages"), [this](lv_event_t *e) {
        messages->loadScreen();
        if (inputdriver->hasKeyboardDevice())
            lv_indev_set_group(inputdriver->getKeyboard(), chatsGroup);
        else {
            // show virtual keyboard
        }
        lv_obj_remove_state(objects.messages_button, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
    });
    messages->setOnCancel([this](lv_event_t *e) { menu->loadScreen(); });
    messages->setOnSendMessage([this](uint32_t to, uint8_t ch, uint32_t msgTime, const char *msg) -> uint32_t {
        bool pki = true;
        auto callback = [this](const ResponseHandler::Request &req, ResponseHandler::EventType evt, int32_t pass) {
            this->onTextMessageCallback(req, evt, pass);
        };
        uint32_t requestId;
        if (to == UINT32_MAX) {
            requestId = requests.addRequest(ResponseHandler::TextMessageRequest, (void *)(long)ch, callback);
            pki = false;
        } else {
            requestId = requests.addRequest(ResponseHandler::TextMessageRequest, (void *)to, callback);
            auto row = nodes.find(to);
            if (row != nodes.end())
                ch = (uint8_t)(uintptr_t)lv_obj_get_user_data(row->second);
        }
        controller->sendTextMessage(to, ch, db.config.lora.hop_limit, msgTime, requestId, pki, msg);
        return requestId;
    });
#endif

    if (messages) {
        messages->setOwnNode(ownNode);
        messages->setNodeNameResolver([this](uint32_t id) { return nodeName(id); });
        messages->setOnUnreadChanged([this](uint32_t count) {
            unreadMessages = count;
            if (dashboard)
                dashboard->updateUnreadMessages(count);
        });
    }
    if (dashboard) {
        dashboard->setOnOpenMessages([this](lv_event_t *) { messages->loadScreen(); });
        dashboard->setOnOpenNodes([this](lv_event_t *) { node->loadScreen(); });
        dashboard->setOnToggleLoRa([this](lv_event_t *) { settings->open(SettingsPlugin::Editor::Radio); });
        dashboard->setOnToggleSound([this](lv_event_t *) { settings->open(SettingsPlugin::Editor::Popups); });
        dashboard->setOnToggleGPS([this](lv_event_t *) { settings->open(SettingsPlugin::Editor::GPS); });
        dashboard->setOnToggleWLAN([this](lv_event_t *) { settings->open(SettingsPlugin::Editor::WiFi); });
        dashboard->setOnToggleMQTT([this](lv_event_t *) { settings->open(SettingsPlugin::Editor::MQTT); });
        dashboard->setOnToggleTime([this](lv_event_t *) {
            lv_screen_load(objects.clock);
            if (indev)
                lv_indev_set_group(indev, clockGroup);
            if (inputdriver->hasKeyboardDevice())
                lv_indev_set_group(inputdriver->getKeyboard(), clockGroup);
        });
        dashboard->setOnToggleMem([this](lv_event_t *) { updateFreeMem(); });
        // A channel-share editor is not present in the plugin view yet.
        lv_obj_add_flag(objects.home_qr_button, LV_OBJ_FLAG_HIDDEN);
        dashboard->setOnRefreshSDCard([this](lv_event_t *) { updateSDCard(); });
    }
    if (map) {
        map->setOnBack([this]() { menu->loadScreen(); });
        map->setOnOpenWifi([this]() { settings->open(SettingsPlugin::Editor::WiFi); });
        map->setOnOpenNode([this](uint32_t id) {
            messages->loadScreen();
            auto row = nodes.find(id);
            messages->showMessages(id, row != nodes.end() ? (uintptr_t)lv_obj_get_user_data(row->second) : 0);
        });
    }

    // Navigation callbacks for the remaining menu screens.
    lv_obj_add_event_cb(objects.map_button, this->ui_event_MapButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.clock_button, this->ui_event_ClockButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.music_button, this->ui_event_MusicButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.statistics_button, this->ui_event_StatisticsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.tools_button, this->ui_event_ToolsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.apps_button, this->ui_event_AppsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.settings_button, this->ui_event_SettingsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.power_button, this->ui_event_PowerButton, LV_EVENT_ALL, NULL);

    // top back buttons of each plugin
    lv_obj_add_event_cb(objects.top_home_back_button, this->ui_event_TopBackButton, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.top_nodes_back_button, this->ui_event_TopBackButton, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.top_groups_back_button, this->ui_event_TopBackButton, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.top_chat_back_button, this->ui_event_TopBackButton, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.top_clock_back_button, this->ui_event_TopBackButton, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.top_settings_back_button, this->ui_event_TopBackButton, LV_EVENT_SHORT_CLICKED, NULL);
}

/**
 * handle events for virtual keyboard
 */
#if 0
void PluggableView::ui_event_Keyboard(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        lv_obj_t *kb = lv_event_get_target_obj(e);
        uint32_t btn_id = lv_keyboard_get_selected_button(kb);

        switch (btn_id) {
        case 22: { // enter (filtered out by one-liner text input area, so we replace it)
            // lv_obj_t *ta = lv_keyboard_get_textarea(kb);
            // lv_textarea_add_char(ta, ' ');
            // lv_textarea_add_char(ta, CR_REPLACEMENT);
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
            if (THIS->activePanel == objects.messages_panel) {
                THIS->hideKeyboard(objects.messages_panel);
            } else {
                lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
            }
            lv_group_focus_obj(objects.message_input_area);
            break;
        }
        default:
            break;
            // const char *txt = lv_keyboard_get_button_text(kb, btn_id);
        }
    }
}
#endif

void PluggableView::ui_event_TopBackButton(lv_event_t *e)
{
    THIS->menu->loadScreen();
}

// TODO: will be replaced by 'setOnOpenXXX' callbacks, see above
void PluggableView::ui_event_MapButton(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_SHORT_CLICKED && THIS->map) {
        THIS->map->loadScreen();
        THIS->map->showPanel();
    } else if (lv_event_get_code(e) == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Map");
    }
}

void PluggableView::ui_event_ClockButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_SHORT_CLICKED) {
        lv_screen_load_anim(objects.clock, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->clockGroup);
        lv_group_focus_obj(objects.top_clock_back_button);
        lv_obj_remove_state(objects.clock_button, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Clock");
    }
}

void PluggableView::ui_event_MusicButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_SHORT_CLICKED) {
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->musicGroup);
        // lv_group_focus_obj(objects.top_music_back_button);
        lv_obj_remove_state(objects.music_button, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Music");
    }
}

void PluggableView::ui_event_StatisticsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_SHORT_CLICKED) {
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->statisticsGroup);
        // lv_group_focus_obj(objects.top_statistics_back_button);
        lv_obj_remove_state(objects.statistics_button, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Statistics");
    }
}

void PluggableView::ui_event_ToolsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_SHORT_CLICKED) {
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->toolsGroup);
        // lv_group_focus_obj(objects.top_tools_back_button);
        lv_obj_remove_state(objects.tools_button, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Tools");
    }
}

void PluggableView::ui_event_AppsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_SHORT_CLICKED) {
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->homeGroup);
        // lv_group_focus_obj(objects.top_apps_back_button);
        lv_obj_remove_state(objects.apps_button, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Apps");
    }
}

void PluggableView::ui_event_SettingsButton(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_SHORT_CLICKED && THIS->settings)
        THIS->settings->loadScreen();
    else if (lv_event_get_code(e) == LV_EVENT_FOCUSED)
        lv_label_set_text(objects.menu_label, "Settings");
}

void PluggableView::ui_event_PowerButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_SHORT_CLICKED) {
        create_screen_blank();
        lv_screen_load_anim(objects.blank, LV_SCR_LOAD_ANIM_FADE_OUT, 4000, 500, false);
        THIS->controller->requestShutdown(5, THIS->ownNode);
        // if (THIS->controller->isStandalone()) {
        //     lv_timer_create(timer_event_shutdown, 4000, NULL);
        // }
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Power");
    }
}

void PluggableView::notifyConnected(const char *info)
{
    if (state == MeshtasticView::eBooting) {
        // updateBootMessage(info);
    } else {
        if (state == MeshtasticView::eDisconnected) {
            // messageAlert(_("Connected!"), true);
            //  force re-sync with node
            THIS->controller->setConfigRequested(true);
        }
        state = MeshtasticView::eRunning;
    }
}

void PluggableView::notifyDisconnected(const char *info)
{
    if (state == MeshtasticView::eBooting) {
        // updateBootMessage(info);
    } else {
        if (state == MeshtasticView::eRunning) {
            // messageAlert(_("Disconnected!"), true);
        }
        state = MeshtasticView::eDisconnected;
    }
}

void PluggableView::setMyInfo(uint32_t nodeNum)
{
    ownNode = nodeNum;
    if (messages)
        messages->setOwnNode(nodeNum);
    if (map)
        map->setOwnNode(nodeNum);
    if (node && objects.node_button && nodeNum) {
        nodes[nodeNum] = objects.node_button;
        node->bindRow(objects.node_button, nodeNum, 0);
    }
}

std::string PluggableView::nodeName(uint32_t nodeNum) const
{
    auto row = nodes.find(nodeNum);
    if (row == nodes.end() || !row->second)
        return {};
    return lv_label_get_text(lv_obj_get_child(row->second, 1));
}

// home screen
void PluggableView::showLoRaFrequency(const meshtastic_Config_LoRaConfig &cfg)
{
    if (dashboard)
        dashboard->updateLoRaConfig(cfg);
}

void PluggableView::setBellText(bool banner, bool sound)
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
#if 0
    char buf[40];
    lv_snprintf(buf, sizeof(buf), _("Message Alert: %s"),
                db.module_config.external_notification.alert_message_buzzer
                    ? (!sound ? _("silent") : ringtone[db.uiConfig.ring_tone_id].name)
                    : "off");
    lv_label_set_text(objects.basic_settings_alert_label, buf);

    Themes::recolorButton(objects.home_bell_button, banner || sound);
    Themes::recolorText(objects.home_bell_label, banner || sound);
#endif
}

void PluggableView::updateLoRaConfig(const meshtastic_Config_LoRaConfig &cfg)
{
    db.config.lora = cfg;
    db.config.has_lora = true;
    if (dashboard)
        dashboard->updateLoRaConfig(cfg);
}

/**
 * update signal strength text and image for home screen
 */
void PluggableView::updateSignalStrength(int32_t rssi, float snr)
{
    // remember time we last heard a node
    time(&lastHeard);
    if (dashboard)
        dashboard->updateSignalStrength(rssi, snr);
}

/**
 * calculate percentage value from rssi and snr
 * Note: ranges are based on the axis values of the signal scanner
 */
int32_t PluggableView::signalStrength2Percent(int32_t rx_rssi, float rx_snr)
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

void PluggableView::updatePositionConfig(const meshtastic_Config_PositionConfig &cfg)
{
    db.config.position = cfg;
    db.config.has_position = true;
    if (dashboard)
        dashboard->updatePositionConfig(cfg);
}

void PluggableView::updateConnectionStatus(const meshtastic_DeviceConnectionStatus &status)
{
    db.connectionStatus = status;
    if (dashboard)
        dashboard->updateConnectionStatus(status);
    if (map)
        map->updateNetwork(db.config.network.wifi_enabled,
                           status.has_wifi && status.wifi.has_status && status.wifi.status.is_connected,
                           status.has_wifi && status.wifi.has_status);
}

void PluggableView::updateNetworkConfig(const meshtastic_Config_NetworkConfig &cfg)
{
    db.config.network = cfg;
    db.config.has_network = true;
    refreshSettingsStatus();
}

void PluggableView::updateMQTTModule(const meshtastic_ModuleConfig_MQTTConfig &cfg)
{
    db.module_config.mqtt = cfg;
    db.module_config.has_mqtt = true;
    refreshSettingsStatus();
}

void PluggableView::updateExtNotificationModule(const meshtastic_ModuleConfig_ExternalNotificationConfig &cfg)
{
    db.module_config.external_notification = cfg;
    db.module_config.has_external_notification = true;
    refreshSettingsStatus();
}

void PluggableView::refreshSettingsStatus()
{
    if (dashboard) {
        if (db.config.has_lora)
            dashboard->updateLoRaConfig(db.config.lora);
        if (db.config.has_network)
            dashboard->updateNetworkConfig(db.config.network);
        if (db.config.has_position)
            dashboard->updatePositionConfig(db.config.position);
        if (db.module_config.has_mqtt)
            dashboard->updateMQTTConfig(db.module_config.mqtt);
        dashboard->updateNotifications(db.uiConfig.alert_enabled);
        if (db.module_config.has_external_notification)
            dashboard->updateSound(db.module_config.external_notification.enabled &&
                                   db.module_config.external_notification.alert_message_buzzer);
    }
    updateConnectionStatus(db.connectionStatus);
}

void PluggableView::updateDisplayConfig(const meshtastic_Config_DisplayConfig &cfg)
{
    db.config.display = cfg;
    db.config.has_display = true;
    if (cfg.displaymode != meshtastic_Config_DisplayConfig_DisplayMode_COLOR) {
        meshtastic_Config_DisplayConfig &display = db.config.display;
        display.displaymode = meshtastic_Config_DisplayConfig_DisplayMode_COLOR;
        THIS->controller->sendConfig(meshtastic_Config_DisplayConfig{display}, THIS->ownNode);
    }
}

void PluggableView::updatePosition(uint32_t nodeNum, int32_t lat, int32_t lon, int32_t alt, uint32_t sats, uint32_t precision)
{
    if (nodeNum == ownNode) {
        const bool useReceiver = localGPSHasPosition && !db.config.position.fixed_position &&
                                 db.config.position.gps_mode == meshtastic_Config_PositionConfig_GpsMode_ENABLED;
        if (dashboard && !useReceiver)
            dashboard->updatePosition(lat, lon, alt, sats, precision,
                                      db.config.display.units == meshtastic_Config_DisplayConfig_DisplayUnits_METRIC);
        if (!useReceiver && (lat != 0 || lon != 0)) {
            hasPosition = true;
            myLatitude = lat;
            myLongitude = lon;
            if (map)
                map->setGpsPosition(lat, lon);
        }
    } else if (map && (lat != 0 || lon != 0)) {
        map->updateNode(nodeNum, lat, lon, nodeName(nodeNum).c_str());
    }
}

void PluggableView::updateLocalGPSStatus(const LocalGPSStatus &status)
{
    localGPSHasPosition = status.hasPosition;
    const bool useReceiver = status.hasPosition && !db.config.position.fixed_position &&
                             db.config.position.gps_mode == meshtastic_Config_PositionConfig_GpsMode_ENABLED;
    if (dashboard) {
        if (useReceiver)
            dashboard->updatePosition(status.latitude_i, status.longitude_i, status.altitude, status.satellites, 0,
                                      db.config.display.units == meshtastic_Config_DisplayConfig_DisplayUnits_METRIC);
        dashboard->updateLocalGPSStatus(status);
    }
    if (useReceiver) {
        hasPosition = true;
        myLatitude = status.latitude_i;
        myLongitude = status.longitude_i;
        if (map)
            map->setGpsPosition(myLatitude, myLongitude);
    }
}

// TODO: move into NodesPlugin
void PluggableView::addOrUpdateNode(uint32_t nodeNum, uint8_t channel, const meshtastic_NodeInfo &info,
                                    const meshtastic_User &cfg)
{
    auto it = nodes.find(nodeNum);
    if (it == nodes.end()) {
        addNode(nodeNum, channel, cfg.short_name, cfg.long_name, info.last_heard, (MeshtasticView::eRole)cfg.role,
                cfg.public_key.size != 0, info.is_favorite, info.is_ignored, cfg.has_is_unmessagable && cfg.is_unmessagable);
    } else if (node) {
        node->bindRow(it->second, nodeNum, channel);
        node->updateRow(it->second, cfg.short_name, cfg.long_name, info.is_favorite);
    }
}

// TODO: move into NodesPlugin
void PluggableView::addNode(uint32_t nodeNum, uint8_t ch, const char *userShort, const char *userLong, uint32_t lastHeard,
                            eRole role, bool hasKey, bool isFav, bool isIgnored, bool unmessagable)
{
    if (!node)
        return;
    lv_group_t *previous = lv_group_get_default();
    lv_group_set_default(nodesGroup);
    nodes[nodeNum] = node->createRow(objects.nodes_panel, nodeNum, ch, userShort, userLong, isFav);
    nodeCount = nodes.size();
    updateNodesStatus();
    lv_group_set_default(previous);
}

void PluggableView::updateNode(uint32_t nodeNum, uint8_t ch, const meshtastic_User &cfg)
{
    auto row = nodes.find(nodeNum);
    if (node && row != nodes.end()) {
        node->bindRow(row->second, nodeNum, ch);
        lv_label_set_text(lv_obj_get_child(row->second, 1), cfg.short_name);
        lv_label_set_text(lv_obj_get_child(row->second, 2), cfg.long_name);
    }
}

void PluggableView::removeNode(uint32_t nodeNum)
{
    if (map)
        map->removeNode(nodeNum);
    auto row = nodes.find(nodeNum);
    if (row != nodes.end() && nodeNum != ownNode) {
        lv_obj_delete(row->second);
        nodes.erase(row);
    }
    nodeCount = nodes.size();
    updateNodesStatus();
}

void PluggableView::updateChannelConfig(const meshtastic_Channel &ch)
{
    if (ch.index < 0 || ch.index >= c_max_channels)
        return;
    db.channel[ch.index] = ch;

    if (groups)
        groups->updateChannel(ch.index, ch.settings.name, ch.role != meshtastic_Channel_Role_DISABLED);
}

/**
 * update signal strength for direct neighbors
 */
void PluggableView::updateSignalStrength(uint32_t nodeNum, int32_t rssi, float snr)
{
#if 0
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
#endif
}

bool PluggableView::updateSDCard(void)
{
    bool cardDetected = false;
    formatSD = false;
    if (map)
        map->updateStorage(nullptr, false);
    if (sdCard) {
        delete sdCard;
        sdCard = nullptr;
    }
#ifdef HAS_SDCARD
    char buf[64];
#ifdef HAS_SD_MMC
    sdCard = new SDCard;
#else
    sdCard = new SdFsCard;
#endif
    ISdCard::ErrorType err = ISdCard::ErrorType::eNoError;
    if (sdCard->init() && sdCard->cardType() != ISdCard::eNone) {
        ILOG_DEBUG("SdCard init successful, card type: %d", sdCard->cardType());
        ISdCard::CardType cardType = sdCard->cardType();
        ISdCard::FatType fatType = sdCard->fatType();
        uint32_t usedSpace = sdCard->usedBytes() / (1024 * 1024);
        uint32_t totalSpace = sdCard->cardSize() / (1024 * 1024);
        uint32_t totalSpaceGB = (sdCard->cardSize() + 500000000ULL) / (1000ULL * 1000ULL * 1000ULL);

        sprintf(buf, _("%s: %d GB (%s)\nUsed: %0.2f GB (%d%%)"),
                cardType == ISdCard::eMMC    ? "MMC"
                : cardType == ISdCard::eSD   ? "SDSC"
                : cardType == ISdCard::eSDHC ? "SDHC"
                : cardType == ISdCard::eSDXC ? "SDXC"
                                             : "UNKN",
                totalSpaceGB,
                fatType == ISdCard::eExFat   ? "exFAT"
                : fatType == ISdCard::eFat32 ? "FAT32"
                : fatType == ISdCard::eFat16 ? "FAT16"
                                             : "???",
                float(sdCard->usedBytes()) / 1024.0f / 1024.0f / 1024.0f,
                totalSpace ? ((usedSpace * 100) + totalSpace / 2) / totalSpace : 0);
        cardDetected = true;
    } else {
        ILOG_DEBUG("SdFsCard init failed");
        err = sdCard->errorType();
        delete sdCard;
        sdCard = nullptr;
    }

    if (!cardDetected || err != ISdCard::ErrorType::eNoError) {
        switch (err) {
        case ISdCard::ErrorType::eSlotEmpty:
            ILOG_WARN("SD card slot empty");
            lv_snprintf(buf, sizeof(buf), _("SD slot empty"));
            break;
        case ISdCard::ErrorType::eFormatError:
            ILOG_ERROR("SD invalid format");
            lv_snprintf(buf, sizeof(buf), _("SD invalid format"));
            formatSD = true;
            break;
        case ISdCard::ErrorType::eNoMbrError:
            ILOG_ERROR("SD mbr not found");
            lv_snprintf(buf, sizeof(buf), _("SD mbr not found"));
            formatSD = true;
            break;
        case ISdCard::ErrorType::eCardError:
            ILOG_ERROR("SD card error");
            lv_snprintf(buf, sizeof(buf), _("SD card error"));
            break;
        default:
            ILOG_ERROR("SD unknown error");
            lv_snprintf(buf, sizeof(buf), _("SD unknown error"));
            break;
        }
        // allow backup/restore only if there is an SD card detected
        // lv_obj_add_state(objects.basic_settings_backup_restore_button, LV_STATE_DISABLED);
    } else {
        // enable backup/restore
        // lv_obj_clear_state(objects.basic_settings_backup_restore_button, LV_STATE_DISABLED);
    }
    // lv_label_set_text(objects.home_sd_card_label, buf);
#else
    // lv_obj_add_flag(objects.home_sd_card_button, LV_OBJ_FLAG_HIDDEN);
    // lv_obj_add_flag(objects.home_sd_card_label, LV_OBJ_FLAG_HIDDEN);
#if defined(ARCH_PORTDUINO)
    cardDetected = true; // use PortduinoFS instead
    sdCard = new SDCard;
#endif
#endif
    if (!sdCard)
        sdCard = new NoSdCard;
    if (map)
        map->updateStorage(sdCard, cardDetected);
    if (dashboard)
        dashboard->updateSDCard(cardDetected);
    return cardDetected;
}

void PluggableView::updateTime(void)
{
    time_t curr_time;
#ifdef ARCH_PORTDUINO
    time(&curr_time);
#else
    curr_time = actTime;
#endif

    if (dashboard)
        dashboard->updateTime((uint32_t)curr_time, db.config.display.use_12h_clock);

    if (clock)
        clock->updateTime((uint32_t)curr_time, db.config.display.use_12h_clock);
}

void PluggableView::updateTime(uint32_t timeVal)
{
    time_t localtime;
    time(&localtime);

    if (VALID_TIME(localtime)) {
        if (actTime != localtime) {
            ILOG_DEBUG("update (local)time: %d -> %d", actTime, localtime);
            actTime = localtime;
        }
        if (clock)
            clock->updateTimezone(actTime);
    } else {
        if (timeVal > actTime) {
            ILOG_DEBUG("update (act)time: %d -> %d", actTime, timeVal);
            actTime = timeVal;
        }
    }
}

void PluggableView::updateNodesStatus(void)
{
    if (dashboard)
        dashboard->updateNodesStatus(nodesOnline, nodeCount);
    if (node)
        node->updateNodesOnline(nodesOnline, nodeCount);
}

void PluggableView::updateFreeMem(void)
{
    // only update if HomePanel is active (since this seems some critical code that did crash sporadically)
    //    if (activePanel == objects.home_panel && (unsigned long)objects.home_memory_button->user_data) {
    uint32_t freeHeap = 0;
    uint32_t freeHeap_pct = 0;
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
#ifdef ARDUINO_ARCH_ESP32
    freeHeap = ESP.getFreeHeap();
    freeHeap_pct = 100 * freeHeap / ESP.getHeapSize();
#elif defined(ARCH_PORTDUINO)
    static uint32_t totalMem = LinuxHelper::getTotalMem();
    if (totalMem != 0) {
        freeHeap = LinuxHelper::getAvailableMem();
        freeHeap_pct = 100 * freeHeap / totalMem;
    }
#endif
    if (dashboard)
        dashboard->updateFreeMem(freeHeap, mon.free_size);
    //    }
}

void PluggableView::restoreMessage(const LogMessage &msg)
{
    if (messages) {
        messages->restoreMessage(msg.from, msg.to, msg.ch, (const char *)msg.bytes, msg.time, msg.trashFlag);
    }
}

void PluggableView::newMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t &msgtime, bool restore)
{
    if (!messages)
        return;
    if (restore)
        messages->restoreMessage(from, to, ch, msg, msgtime, false);
    else
        messages->newMessage(from, to, ch, msg, msgtime);
}

void PluggableView::packetReceived(const meshtastic_MeshPacket &p)
{
    MeshtasticView::packetReceived(p);

    // try update time from packet
    if (!VALID_TIME(actTime)) {
#ifdef ARCH_PORTDUINO
        updateTime(0); // read from local time
#endif
        if (VALID_TIME(p.rx_time))
            updateTime(p.rx_time);
    }
    if (p.from != ownNode) {
        updateSignalStrength(p.rx_rssi, p.rx_snr);
    }
}

/**
 * handle response from routing
 */
void PluggableView::handleResponse(uint32_t from, const uint32_t id, const meshtastic_Routing &routing,
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
                // handleTraceRouteResponse(routing);
            } else if (req.type == ResponseHandler::TextMessageRequest) {
                messages->handleResponse((unsigned long)req.cookie, id, ack, false);
            } else if (req.type == ResponseHandler::PositionRequest) {
                // handlePositionResponse(from, id, p.rx_rssi, p.rx_snr, p.hop_limit == p.hop_start);
            }
        } else if (routing.error_reason == meshtastic_Routing_Error_MAX_RETRANSMIT) {
            ResponseHandler::Request req = requests.removeRequest(id);
            if (req.type == ResponseHandler::TraceRouteRequest) {
                // handleTraceRouteResponse(routing);
            } else if (req.type == ResponseHandler::TextMessageRequest) {
                messages->handleResponse((unsigned long)req.cookie, id, ack, true);
            }
        } else if (routing.error_reason == meshtastic_Routing_Error_NO_RESPONSE) {
            if (req.type == ResponseHandler::PositionRequest) {
                // handlePositionResponse(from, id, p.rx_rssi, p.rx_snr, p.hop_limit == p.hop_start);
            }
        } else if (routing.error_reason == meshtastic_Routing_Error_NO_CHANNEL ||
                   routing.error_reason == meshtastic_Routing_Error_PKI_UNKNOWN_PUBKEY ||
                   routing.error_reason == meshtastic_Routing_Error_PKI_FAILED ||
                   routing.error_reason == meshtastic_Routing_Error_NO_INTERFACE) {
            if (req.type == ResponseHandler::TextMessageRequest) {
                messages->handleResponse((unsigned long)req.cookie, id, ack, true);
                // TODO: mark node key as wrong or not usable
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
        // handleResponse(from, id, routing.route_reply);
        break;
    }
    default:
        ILOG_ERROR("unhandled meshtastic_Routing tag");
        break;
    }
}

#if 0 // trace route response
void PluggableView::handleResponse(uint32_t from, uint32_t id, const meshtastic_RouteDiscovery &route)
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
#endif

void PluggableView::onTextMessageCallback(const ResponseHandler::Request &req, ResponseHandler::EventType evt, int32_t result)
{
    ILOG_DEBUG("onTextMessageCallback: %d %d", evt, result);
    if (evt == ResponseHandler::found) {
        // handleTextMessageResponse((unsigned long)req.cookie, req.id, false, result);
    } else if (evt == ResponseHandler::removed) {
        // handleTextMessageResponse((unsigned long)req.cookie, req.id, true, result);
    } else {
        ILOG_DEBUG("onTextMessageCallback: timeout!");
        messages->handleResponse((unsigned long)req.cookie, req.id, false, true);
    }
}

void PluggableView::task_handler(void)
{
    if (messages)
        messages->setNotificationsSuppressed(settings && settings->isLocked());
    MeshtasticView::task_handler();
    if (!screensInitialised)
        return;
    pluginRegistry.task_handler(curtime);
    if (curtime - lastrun1 >= 1) {
        lastrun1 = curtime;
        actTime++;
        updateTime();
    }
    if (curtime - lastrun10 >= 10) {
        lastrun10 = curtime;
        updateFreeMem();
        if (db.config.network.wifi_enabled && !displaydriver->isPowersaving())
            controller->requestDeviceConnectionStatus();
    }
}

#endif