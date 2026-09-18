#ifdef VIEW_160x80

#include "graphics/view/TFT/TFTView_160x80.h"
#include "graphics/common/BatteryLevel.h"
#include "graphics/common/LoRaPresets.h"
#include "graphics/common/ViewController.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverFactory.h"
#include "graphics/map/MapPanel.h"
#include "images.h"
#include "input/InputDriver.h"
#include "lv_i18n.h"
#include "ui.h"
#include "util/ILog.h"
#include <cstdio>
#include <ctime>
#include <functional>
#include <time.h>

// input groups for encoder navigation
lv_group_t *menuGroup = nullptr;
lv_group_t *homeGroup = nullptr;
lv_group_t *nodesGroup = nullptr;
lv_group_t *groupsGroup = nullptr;
lv_group_t *chatsGroup = nullptr;
lv_group_t *mapGroup = nullptr;
lv_group_t *clockGroup = nullptr;
lv_group_t *musicGroup = nullptr;
lv_group_t *statisticsGroup = nullptr;
lv_group_t *toolsGroup = nullptr;
lv_group_t *settingsGroup = nullptr;
lv_group_t *powerGroup = nullptr;

#define THIS TFTView_160x80::instance() // need to use this in all static methods

#define LV_COLOR_HEX(C)                                                                                                          \
    {                                                                                                                            \
        .blue = (C >> 0) & 0xff, .green = (C >> 8) & 0xff, .red = (C >> 16) & 0xff                                               \
    }

#define VALID_TIME(T) (T > 1000000 && T < UINT32_MAX)

TFTView_160x80 *TFTView_160x80::gui = nullptr;

TFTView_160x80 *TFTView_160x80::instance(void)
{
    if (!gui)
        gui = new TFTView_160x80(nullptr, DisplayDriverFactory::create(160, 80));
    return gui;
}

TFTView_160x80 *TFTView_160x80::instance(const DisplayDriverConfig &cfg)
{
    if (!gui)
        gui = new TFTView_160x80(&cfg, DisplayDriverFactory::create(cfg));
    return gui;
}

TFTView_160x80::TFTView_160x80(const DisplayDriverConfig *cfg, DisplayDriver *driver)
    : MeshtasticView(cfg, driver, new ViewController), screensInitialised(false), actTime(0), uptime(0), lastHeard(0),
      hasPosition(false), myLatitude(0), myLongitude(0), db{}
{
}

void TFTView_160x80::init(IClientBase *client)
{
    ILOG_DEBUG("TFTView_160x80 init...");
    MeshtasticView::init(client);

#if defined(INPUTDRIVER_ROTARY_TYPE) || defined(INPUTDRIVER_ENCODER_TYPE)

    indev = InputDriver::instance()->getEncoder();
#elif defined(INPUTDRIVER_BUTTON_TYPE)
    indev = InputDriver::instance()->getButton();
#elif LV_USE_LIBINPUT
    indev = InputDriver::instance()->getPointer();
#endif

    ui_init_boot();

    time(&lastrun60);
    time(&lastrun10);
    time(&lastrun5);
    time(&lastrun1);

    lv_timer_create(timer_event_bootscreen_done, 3000, NULL);
}

void TFTView_160x80::ui_events_init(void)
{
    // main button events
    lv_obj_add_event_cb(objects.home_button, this->ui_event_HomeButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.nodes_button, this->ui_event_NodesButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.groups_button, this->ui_event_GroupsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.messages_button, this->ui_event_MessagesButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.map_button, this->ui_event_MapButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.clock_button, this->ui_event_ClockButton, LV_EVENT_ALL, NULL);
    //    lv_obj_add_event_cb(objects.music_button, this->ui_event_MusicButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.statistics_button, this->ui_event_StatisticsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.tools_button, this->ui_event_ToolsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.settings_button, this->ui_event_SettingsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.power_button, this->ui_event_PowerButton, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(objects.top_home_back_button, this->ui_event_TopBackButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.top_nodes_back_button, this->ui_event_TopBackButton, LV_EVENT_ALL, NULL);
    // lv_obj_add_event_cb(objects.top_groups_back_button, this->ui_event_TopBackButton, LV_EVENT_ALL, NULL);
    // lv_obj_add_event_cb(objects.top_chat_back_button, this->ui_event_TopBackButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.top_map_back_button, this->ui_event_TopBackButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.top_clock_back_button, this->ui_event_TopBackButton, LV_EVENT_ALL, NULL);
    // lv_obj_add_event_cb(objects.top_settings_back_button, this->ui_event_TopBackButton, LV_EVENT_ALL, NULL);

    // lv_obj_add_event_cb(objects.settings_brightness_dropdown, this->ui_event_SettingsBrightnessDropdown, LV_EVENT_ALL, NULL);
}

void TFTView_160x80::timer_event_bootscreen_done(lv_timer_t *timer)
{
    if (THIS->state == eBooting)
        THIS->state = MeshtasticView::eBootScreenDone;
    lv_timer_delete(timer);
}

/**
 * @brief initialize UI with persistent data
 */
bool TFTView_160x80::setupUIConfig(const meshtastic_DeviceUIConfig &uiconfig)
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

#if 0
    // initialize own node panel
    if (ownNode && objects.node_button)
        nodes[ownNode] = objects.node_button;
#endif
    // check SD card
    // updateSDCard();

    lv_disp_trig_activity(NULL);
    return true;
}

/**
 * @brief Initialize all screens and apply customizations
 *
 */
void TFTView_160x80::init_screens(void)
{
    ILOG_DEBUG("init screens...");
    state = MeshtasticView::eInitScreens;
    ui_init();
    ui_events_init();
    screensInitialised = true;
    state = MeshtasticView::eInitDone;
    ILOG_DEBUG("TFTView_160x80 init done.");
}

/**
 * Overwrite the default generated function ui_init()
 * Assign input groups per screen.
 */
void TFTView_160x80::ui_init(void)
{
    ILOG_DEBUG("ui_init...");
    menuGroup = lv_group_create();
    lv_group_set_default(menuGroup);
    lv_indev_set_group(THIS->indev, menuGroup);
    create_screen_menu();
    ILOG_DEBUG("menu group count: %d", lv_group_get_obj_count(menuGroup));

    homeGroup = lv_group_create();
    lv_group_set_default(homeGroup);
    lv_indev_set_group(THIS->indev, menuGroup);
    create_screen_home();
    ILOG_DEBUG("home group count: %d", lv_group_get_obj_count(homeGroup));

    // create and wire dashboard plugin
    if (!dashboard) {
        dashboard = new DashboardPlugin();
        dashboard->init(nullptr, nullptr, DashboardPlugin::WIDGET_COUNT, homeGroup, indev);

        // register widgets that were created by the generated UI (adjust names if your generated UI differs)
        dashboard->setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::TimeLabel), objects.home_time_label);
        dashboard->setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::LoRaLabel), objects.home_lora_label);
        dashboard->setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::SignalButton),
                             objects.home_signal_button);
        dashboard->setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::SignalLabel),
                             objects.home_signal_label);
        dashboard->setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::SignalPctLabel),
                             objects.home_signal_pct_label);
        dashboard->setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::SdLabel), objects.home_sd_card_label);
        dashboard->setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::WlanButton), objects.home_wlan_label);
        dashboard->setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::MemoryLabel),
                             objects.home_memory_label);
        dashboard->setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::NodesLabel), objects.home_nodes_label);
        dashboard->setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::MapButton), objects.map_button);
        dashboard->setWidget(static_cast<GfxPlugin::WidgetIndex>(DashboardPlugin::Widget::NodesButton), objects.nodes_button);
        dashboard->registerStandardWidgetActions();
#if 0
        // wire dashboard callbacks into existing view actions
        dashboard->setOnOpenMap([this]() {
            lv_screen_load_anim(objects.map, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
            lv_indev_set_group(this->indev, mapGroup);
            this->loadMap();
            lv_group_focus_obj(objects.top_map_back_button);
            lv_obj_remove_state(objects.map_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
        });
        dashboard->setOnOpenNodes([this]() {
            lv_screen_load_anim(objects.nodes, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
            lv_indev_set_group(this->indev, nodesGroup);
            lv_group_focus_obj(objects.top_nodes_back_button);
            lv_obj_remove_state(objects.nodes_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
        });
        dashboard->setOnPower([this]() {
            create_screen_blank();
            lv_screen_load_anim(objects.blank, LV_SCR_LOAD_ANIM_FADE_OUT, 4000, 500, false);
            this->controller->requestShutdown(5, this->ownNode);
        });
#endif
    }

    nodesGroup = lv_group_create();
    lv_group_set_default(nodesGroup);
    lv_indev_set_group(THIS->indev, nodesGroup);
    create_screen_nodes();
    ILOG_DEBUG("nodes group count: %d", lv_group_get_obj_count(nodesGroup));
#if 0
    groupsGroup = lv_group_create();
    lv_group_set_default(groupsGroup);
    lv_indev_set_group(THIS->indev, groupsGroup);
    create_screen_groups();
    ILOG_DEBUG("groups group count: %d", lv_group_get_obj_count(groupsGroup));
#endif
#if 0
    chatsGroup = lv_group_create();
    lv_group_set_default(chatsGroup);
    lv_indev_set_group(THIS->indev, chatsGroup);
    create_screen_chats();
    ILOG_DEBUG("chats group count: %d", lv_group_get_obj_count(chatsGroup));
#endif
    mapGroup = lv_group_create();
    lv_group_set_default(mapGroup);
    lv_indev_set_group(THIS->indev, mapGroup);
    create_screen_map();
    ILOG_DEBUG("map group count: %d", lv_group_get_obj_count(mapGroup));

    clockGroup = lv_group_create();
    lv_group_set_default(clockGroup);
    lv_indev_set_group(THIS->indev, clockGroup);
    create_screen_clock();
    ILOG_DEBUG("clock group count: %d", lv_group_get_obj_count(clockGroup));
#if 0
    settingsGroup = lv_group_create();
    lv_group_set_default(settingsGroup);
    lv_indev_set_group(THIS->indev, settingsGroup);
    create_screen_settings();
    ILOG_DEBUG("settings group count: %d", lv_group_get_obj_count(settingsGroup));
#endif
    ILOG_DEBUG("total group count: %d", lv_group_get_count());

    lv_screen_load_anim(objects.menu, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, true);
    lv_group_set_default(menuGroup);
    lv_indev_set_group(THIS->indev, menuGroup);
    lv_group_focus_obj(objects.home_button);
}

void TFTView_160x80::ui_event_TopBackButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.menu, LV_SCR_LOAD_ANIM_OUT_BOTTOM, 500, 0, false);
        lv_indev_set_group(THIS->indev, menuGroup);
        lv_group_focus_obj(objects.home_button);
    }
}

void TFTView_160x80::ui_event_HomeButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, homeGroup);
        lv_group_focus_obj(objects.top_home_back_button);
        lv_obj_remove_state(objects.home_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Home");
    }
}

void TFTView_160x80::ui_event_NodesButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.nodes, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, nodesGroup);
        lv_group_focus_obj(objects.top_nodes_back_button);
        lv_obj_remove_state(objects.nodes_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Nodes");
    }
}

void TFTView_160x80::ui_event_GroupsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
#if 0
        lv_screen_load_anim(objects.groups, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, groupsGroup);
        lv_group_focus_obj(objects.top_groups_back_button);
        lv_obj_remove_state(objects.groups_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
#endif
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Group Channels");
    }
}

void TFTView_160x80::ui_event_MessagesButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
#if 0
        lv_screen_load_anim(objects.chats, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, chatsGroup);
        lv_group_focus_obj(objects.top_chat_back_button);
        lv_obj_remove_state(objects.chat_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
#endif
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Messages");
    }
}

void TFTView_160x80::ui_event_MapButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.map, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, mapGroup);
        //        THIS->loadMap();
        lv_group_focus_obj(objects.top_map_back_button);
        lv_obj_remove_state(objects.map_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Map");
    }
}

void TFTView_160x80::ui_event_ClockButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.clock, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, clockGroup);
        lv_group_focus_obj(objects.top_clock_back_button);
        lv_obj_remove_state(objects.clock_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Clock");
    }
}

#if 0
void TFTView_160x80::ui_event_MusicButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, musicGroup);
        //lv_group_focus_obj(objects.top_clock_back_button);
        lv_obj_remove_state(objects.music_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    }
    else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Music");
    }
}
#endif

void TFTView_160x80::ui_event_StatisticsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
#if 0
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, statisticsGroup);
        //lv_group_focus_obj(objects.top_clock_back_button);
        lv_obj_remove_state(objects.statistics_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
#endif
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Statistics");
    }
}

void TFTView_160x80::ui_event_ToolsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
#if 0
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, toolsGroup);
        //lv_group_focus_obj(objects.top_clock_back_button);
        lv_obj_remove_state(objects.tools_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
#endif
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Tools");
    }
}

void TFTView_160x80::ui_event_SettingsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
#if 0
        lv_screen_load_anim(objects.settings, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, settingsGroup);
        lv_group_focus_obj(objects.top_settings_back_button);
        lv_obj_remove_state(objects.settings_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
#endif
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Settings");
    }
}

void TFTView_160x80::ui_event_PowerButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
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

#if 0
void TFTView_160x80::ui_event_SettingsBrightnessDropdown(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        uint32_t selected = lv_dropdown_get_selected(objects.settings_brightness_dropdown);
        ILOG_DEBUG("pressed: brightness %d", selected * 50);
        THIS->displaydriver->setBrightness((uint8_t)selected * 50);
    }
    else if (event_code == LV_EVENT_FOCUSED) {
        uint32_t selected = lv_dropdown_get_selected(objects.settings_brightness_dropdown);
        ILOG_DEBUG("focused: brightness %d", selected * 50);
        //THIS->displaydriver->setBrightness((uint8_t)selected * 50);
    }
}
#endif

void TFTView_160x80::updateTime(void)
{
    // char buf1[80], buf2[80];
    time_t curr_time;
#ifdef ARCH_PORTDUINO
    time(&curr_time);
#else
    curr_time = actTime;
#endif

    if (dashboard)
        dashboard->updateTime((uint32_t)curr_time, db.config.display.use_12h_clock);

#if 0 // TODO: has some code for ClockPlugin for reuse
    int len1 = 0, len2 = 0;
    if (VALID_TIME(curr_time) /* && (unsigned long)objects.home_time_button->user_data == 0 */) {
        if (db.config.display.use_12h_clock) {
            len1 = strftime(buf1, 40, "%I:%M:%S %p\n%a %d-%b-%g", curr_tm);
            len2 = strftime(buf2, 40, "%I:%M", curr_tm);
        } else {
            len1 = strftime(buf1, 40, "%T %Z\n%a %d-%b-%g", curr_tm);
            len2 = strftime(buf2, 40, "%H:%M", curr_tm);
        }
    } else {
        uint32_t uptime = millis() / 1000;
        int hours = uptime / 3600;
        uptime -= hours * 3600;
        int minutes = uptime / 60;
        int seconds = uptime - minutes * 60;

        sprintf(&buf1[len1], _("uptime: %02d:%02d:%02d"), hours, minutes, seconds);
        sprintf(&buf2[len2], _("%02d:%02d"), hours, minutes);
    }
    lv_label_set_text(objects.home_time_label, buf1);
    lv_label_set_text(objects.clock_time_label, buf2);
#endif
}

void TFTView_160x80::updateTime(uint32_t timeVal)
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

void TFTView_160x80::notifyConnected(const char *info)
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

void TFTView_160x80::notifyDisconnected(const char *info)
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

void TFTView_160x80::setMyInfo(uint32_t nodeNum)
{
    ownNode = nodeNum;
}

void TFTView_160x80::packetReceived(const meshtastic_MeshPacket &p)
{
    MeshtasticView::packetReceived(p);

    // try update time from packet
    if (!VALID_TIME(actTime) && VALID_TIME(p.rx_time))
        updateTime(p.rx_time);

#if 0
    if (p.from != ownNode) {
        updateSignalStrength(p.rx_rssi, p.rx_snr);
    }
#endif
}

void TFTView_160x80::task_handler(void)
{
    MeshtasticView::task_handler();

    if (screensInitialised) {
        if (curtime - lastrun1 >= 1) { // call every 1s
                                       //            if (map) {
                                       //                lv_group_t *current = lv_group_get_default();
                                       //                lv_group_set_default(mapGroup);
                                       //                updateLocationMap(THIS->map->getObjectsOnMap());
                                       //                lv_group_set_default(current);
                                       //            }

            lastrun1 = curtime;
            actTime++;
            updateTime();
        }
    }

    if (curtime - lastrun10 >= 10) { // call every 10s
        lastrun10 = curtime;
        // TODO updateFreeMem();

        if ((db.config.network.wifi_enabled || db.module_config.mqtt.enabled) && !displaydriver->isPowersaving()) {
            controller->requestDeviceConnectionStatus();
        }
    }

    if (curtime - lastrun60 >= 60) { // call every 60s
        lastrun60 = curtime;
        // updateAllLastHeard();

        // if (detectorRunning) {
        //     controller->sendPing();
        // }
#if 0
        // if we didn't hear any node for 1h assume we have no signal
        if (curtime - lastHeard > secs_until_offline) {
            lv_obj_set_style_bg_image_src(objects.home_signal_button, &img_home_no_signal_icon,
                                          LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(objects.home_signal_label, _("no signal"));
            lv_label_set_text(objects.home_signal_pct_label, "");
        }
#endif
    }
}

#endif