#if defined(VIEW_160x80) || defined(VIEW_320x170) || defined(VIEW_480x222)

#include "graphics/view/TFT/PluggableView.h"
#include "graphics/common/BatteryLevel.h"
#include "graphics/common/LoRaPresets.h"
#include "graphics/common/ViewController.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverFactory.h"
#include "graphics/map/MapPanel.h"
#include "graphics/plugin/ClockPlugin.h"
#include "graphics/plugin/DashboardPlugin.h"
#include "graphics/plugin/GroupsPlugin.h"
#include "graphics/plugin/NodesPlugin.h"
#include "graphics/plugin/ScrollMenuPlugin.h"
#include "images.h"
#include "input/InputDriver.h"
#include "lv_i18n.h"
#include "ui.h"
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
// #include "graphics/map/LinuxFileSystemService.h"
#include "graphics/map/SDCardService.h"
#elif defined(HAS_SD_MMC)
#include "graphics/map/SDCardService.h"
#else
#include "graphics/map/SdFatService.h"
#endif
#include "graphics/common/SdCard.h"

LV_IMAGE_DECLARE(img_no_tile_image);

#define THIS PluggableView::instance() // need to use this in all static methods

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

enum ScrollDirection {
    scrollDownLeft = 1,
    scrollDown = 2,
    scrollDownRight = 3,
    scrollLeft = 4,
    scrollRight = 6,
    scrollUpLeft = 7,
    scrollUp = 8,
    scrollUpRight = 9,
};

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

    ui_init_boot();

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
    if (ownNode && objects.node_button)
        nodes[ownNode] = objects.node_button;

    // check SD card
    updateSDCard();

    lv_disp_trig_activity(NULL);
    return true;
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
 * Overwrite the default generated function ui_init()
 * Assign input groups per screen/plugin.
 */
void PluggableView::ui_init(void)
{
    ILOG_DEBUG("ui_init...");

    // create and wire menu plugin
    SETUP_INDEV(menuGroup);
    create_screen_menu();
    menu = new ScrollMenuPlugin();
    menu->init(objects.menu, nullptr, ScrollMenuPlugin::WIDGET_COUNT, menuGroup, indev);

    // create and wire dashboard plugin home screen
    SETUP_INDEV(homeGroup);
    create_screen_home();
    dashboard = new DashboardPlugin();
    dashboard->init(objects.home, nullptr, DashboardPlugin::WIDGET_COUNT, homeGroup, indev);

    // create and wire nodes plugin
    SETUP_INDEV(nodesGroup);
    create_screen_nodes();
    node = new NodesPlugin();
    node->init(objects.nodes, nullptr, NodesPlugin::WIDGET_COUNT, nodesGroup, indev);

    SETUP_INDEV(groupsGroup);
    create_screen_groups();
    groups = new GroupsPlugin();
    groups->init(objects.groups, nullptr, GroupsPlugin::WIDGET_COUNT, groupsGroup, indev);

    SETUP_INDEV(chatsGroup);
    create_screen_chats();

    SETUP_INDEV(mapGroup);
    create_screen_map();

    // create and wire clock plugin
    SETUP_INDEV(clockGroup);
    create_screen_clock();
    clock = new ClockPlugin();
    clock->init(nullptr, nullptr, ClockPlugin::WIDGET_COUNT, clockGroup, indev);

    SETUP_INDEV(settingsGroup);
    create_screen_settings();

    menu->loadScreen();
    lv_group_focus_obj(objects.home_button);

    setInputGroup();
}

// lv_menu test
/**** !!!!!!!!!!!!  TEMPORARY PLEASE REMOVE BEFORE COMMIT !!!!!!!!!!!!!! */
/**** !!!!!!!!!!!!  TEMPORARY PLEASE REMOVE BEFORE COMMIT !!!!!!!!!!!!!! */
/**** !!!!!!!!!!!!  TEMPORARY PLEASE REMOVE BEFORE COMMIT !!!!!!!!!!!!!! */
/**** !!!!!!!!!!!!  TEMPORARY PLEASE REMOVE BEFORE COMMIT !!!!!!!!!!!!!! */
/**** !!!!!!!!!!!!  TEMPORARY PLEASE REMOVE BEFORE COMMIT !!!!!!!!!!!!!! */
/**** !!!!!!!!!!!!  TEMPORARY PLEASE REMOVE BEFORE COMMIT !!!!!!!!!!!!!! */
typedef enum { LV_MENU_ITEM_BUILDER_VARIANT_1, LV_MENU_ITEM_BUILDER_VARIANT_2 } lv_menu_builder_variant_t;

static lv_obj_t *create_text(lv_obj_t *parent, const char *icon, const char *txt, lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t *obj = lv_menu_cont_create(parent);

    lv_obj_t *img = NULL;
    lv_obj_t *label = NULL;

    if (icon) {
        img = lv_image_create(obj);
        lv_image_set_src(img, icon);
    }

    if (txt) {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    if (builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

static lv_obj_t *create_slider(lv_obj_t *parent, const char *icon, const char *txt, int32_t min, int32_t max, int32_t val)
{
    lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);

    if (icon == NULL) {
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    return obj;
}

static lv_obj_t *create_switch(lv_obj_t *parent, const char *icon, const char *txt, bool chk)
{
    lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t *sw = lv_switch_create(obj);
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : LV_STATE_DEFAULT);

    return obj;
}

static lv_obj_t *root_page = nullptr;

static void switch_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *menu = (lv_obj_t *)lv_event_get_user_data(e);
    lv_obj_t *obj = lv_event_get_target_obj(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
            lv_menu_set_page(menu, NULL);
            lv_menu_set_sidebar_page(menu, root_page);
            lv_obj_send_event(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED,
                              NULL);
        } else {
            lv_menu_set_sidebar_page(menu, NULL);
            lv_menu_clear_history(menu); /* Clear history because we will be showing the root page later */
            lv_menu_set_page(menu, root_page);
        }
    }
}

void PluggableView::ui_events_init(void)
{
    // main button events
    menu->setOnOpenHome(_("Dashboard"), [this](lv_event_t *e) {
        dashboard->loadScreen();
        lv_indev_set_group(THIS->indev, THIS->homeGroup);
        lv_obj_remove_state(objects.nodes_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
        lv_group_focus_obj(objects.top_home_back_button);
    });

    menu->setOnOpenNodes(_("Nodes"), [this](lv_event_t *e) {
        node->loadScreen();
        lv_indev_set_group(THIS->indev, THIS->nodesGroup);
        lv_obj_remove_state(objects.nodes_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
        lv_group_focus_obj(objects.top_nodes_back_button);
    });

    menu->setOnOpenGroups(_("Groups"), [this](lv_event_t *e) {
        groups->loadScreen();
        lv_indev_set_group(THIS->indev, THIS->groupsGroup);
        lv_obj_remove_state(objects.groups_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
        lv_group_focus_obj(objects.top_groups_back_button);
    });

    // lv_obj_add_event_cb(objects.groups_button, this->ui_event_GroupsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.messages_button, this->ui_event_MessagesButton, LV_EVENT_ALL, NULL);
    // message text area
    lv_obj_add_event_cb(objects.message_input_area, ui_event_message_ready, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(objects.map_button, this->ui_event_MapButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.clock_button, this->ui_event_ClockButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.music_button, this->ui_event_MusicButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.statistics_button, this->ui_event_StatisticsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.tools_button, this->ui_event_ToolsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.settings_button, this->ui_event_SettingsButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(objects.power_button, this->ui_event_PowerButton, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(objects.top_home_back_button, this->ui_event_TopBackButton, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.top_nodes_back_button, this->ui_event_TopBackButton, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.top_groups_back_button, this->ui_event_TopBackButton, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.top_chat_back_button, this->ui_event_TopBackButton, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.top_map_back_button, this->ui_event_TopBackButton, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.top_clock_back_button, this->ui_event_TopBackButton, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.top_settings_back_button, this->ui_event_TopBackButton, LV_EVENT_PRESSED, NULL);

    // lv_menu test
    // lv_menu test

    lv_obj_t *sub_mechanics_page = lv_menu_page_create(objects.settings_menu, "TITLE");
    lv_obj_set_style_pad_hor(sub_mechanics_page,
                             lv_obj_get_style_pad_left(lv_menu_get_main_header(objects.settings_menu), LV_PART_MAIN), 0);
    lv_menu_separator_create(sub_mechanics_page);
    lv_obj_t *section = lv_menu_section_create(sub_mechanics_page);
    create_slider(section, LV_SYMBOL_SETTINGS, "Velocity", 0, 150, 120);
    create_slider(section, LV_SYMBOL_SETTINGS, "Acceleration", 0, 150, 50);
    create_slider(section, LV_SYMBOL_SETTINGS, "Weight limit", 0, 150, 80);

    lv_obj_t *sub_sound_page = lv_menu_page_create(objects.settings_menu, NULL);
    lv_obj_set_style_pad_hor(sub_sound_page,
                             lv_obj_get_style_pad_left(lv_menu_get_main_header(objects.settings_menu), LV_PART_MAIN), 0);
    lv_menu_separator_create(sub_sound_page);
    section = lv_menu_section_create(sub_sound_page);
    create_switch(section, LV_SYMBOL_AUDIO, "Sound", false);

    lv_obj_t *sub_display_page = lv_menu_page_create(objects.settings_menu, NULL);
    lv_obj_set_style_pad_hor(sub_display_page,
                             lv_obj_get_style_pad_left(lv_menu_get_main_header(objects.settings_menu), LV_PART_MAIN), 0);
    lv_menu_separator_create(sub_display_page);
    section = lv_menu_section_create(sub_display_page);
    create_slider(section, LV_SYMBOL_SETTINGS, "Brightness", 0, 150, 100);

    lv_obj_t *sub_software_info_page = lv_menu_page_create(objects.settings_menu, NULL);
    lv_obj_set_style_pad_hor(sub_software_info_page,
                             lv_obj_get_style_pad_left(lv_menu_get_main_header(objects.settings_menu), LV_PART_MAIN), 0);
    section = lv_menu_section_create(sub_software_info_page);
    create_text(section, NULL, "Version 1.0", LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t *sub_legal_info_page = lv_menu_page_create(objects.settings_menu, NULL);
    lv_obj_set_style_pad_hor(sub_legal_info_page,
                             lv_obj_get_style_pad_left(lv_menu_get_main_header(objects.settings_menu), LV_PART_MAIN), 0);
    section = lv_menu_section_create(sub_legal_info_page);
    for (uint32_t i = 0; i < 15; i++) {
        create_text(section, NULL,
                    "This is a long long long long long long long long long text, if it is long enough it may scroll.",
                    LV_MENU_ITEM_BUILDER_VARIANT_1);
    }

    lv_obj_t *sub_about_page = lv_menu_page_create(objects.settings_menu, NULL);
    lv_obj_set_style_pad_hor(sub_about_page,
                             lv_obj_get_style_pad_left(lv_menu_get_main_header(objects.settings_menu), LV_PART_MAIN), 0);
    lv_menu_separator_create(sub_about_page);
    section = lv_menu_section_create(sub_about_page);
    lv_obj_t *cont = create_text(section, NULL, "Software information", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(objects.settings_menu, cont, sub_software_info_page);
    cont = create_text(section, NULL, "Legal information", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(objects.settings_menu, cont, sub_legal_info_page);

    lv_obj_t *sub_menu_mode_page = lv_menu_page_create(objects.settings_menu, NULL);
    lv_obj_set_style_pad_hor(sub_menu_mode_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(objects.settings_menu), 0), 0);
    lv_menu_separator_create(sub_menu_mode_page);
    section = lv_menu_section_create(sub_menu_mode_page);
    cont = create_switch(section, LV_SYMBOL_AUDIO, "Sidebar enable", true);
    lv_obj_add_event_cb(lv_obj_get_child(cont, 2), switch_handler, LV_EVENT_VALUE_CHANGED, objects.settings_menu);

    /*Create a root page*/
    root_page = lv_menu_page_create(objects.settings_menu, "Settings");
    lv_obj_set_style_pad_hor(root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(objects.settings_menu), 0), 0);
    section = lv_menu_section_create(root_page);
    cont = create_text(section, LV_SYMBOL_SETTINGS, "Mechanics", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(objects.settings_menu, cont, sub_mechanics_page);
    cont = create_text(section, LV_SYMBOL_AUDIO, "Sound", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(objects.settings_menu, cont, sub_sound_page);
    cont = create_text(section, LV_SYMBOL_SETTINGS, "Display", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(objects.settings_menu, cont, sub_display_page);

    create_text(root_page, NULL, "Others", LV_MENU_ITEM_BUILDER_VARIANT_1);
    section = lv_menu_section_create(root_page);
    cont = create_text(section, NULL, "About", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(objects.settings_menu, cont, sub_about_page);
    cont = create_text(section, LV_SYMBOL_SETTINGS, "Menu mode", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(objects.settings_menu, cont, sub_menu_mode_page);

    lv_menu_set_sidebar_page(objects.settings_menu, NULL);
    lv_menu_clear_history(objects.settings_menu); /* Clear history because we will be showing the root page later */
    lv_menu_set_page(objects.settings_menu, root_page);

    //    lv_menu_set_sidebar_page(objects.settings_menu, root_page);
}

/**
 * Set focus to first button of a panel
 */
#if 0
 void PluggableView::setGroupFocus(lv_obj_t *panel)
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
#endif

/**
 * input group used by keyboard and/or pointer for dynamic assignment
 */
void PluggableView::setInputGroup(void)
{
    lv_group_t *group = chatsGroup;

    if (group && inputdriver->hasKeyboardDevice())
        lv_indev_set_group(inputdriver->getKeyboard(), group);

    if (group && inputdriver->hasPointerDevice())
        lv_indev_set_group(inputdriver->getPointer(), group);
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

#if 0
void PluggableView::ui_event_HomeButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->homeGroup);
        lv_group_focus_obj(objects.top_home_back_button);
        lv_obj_remove_state(objects.home_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    }
    else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Home");
    }
}
#endif

#if 0
void PluggableView::ui_event_NodesButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.nodes, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->nodesGroup);
        lv_group_focus_obj(objects.top_nodes_back_button);
        lv_obj_remove_state(objects.nodes_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    }
    else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Nodes");
    }
}
#endif
void PluggableView::ui_event_GroupsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.groups, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->groupsGroup);
        lv_group_focus_obj(objects.top_groups_back_button);
        lv_obj_remove_state(objects.groups_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Group Channels");
    }
}

void PluggableView::ui_event_MessagesButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.chats, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->chatsGroup);
        lv_group_focus_obj(objects.top_chat_back_button);
        lv_obj_remove_state(objects.messages_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Messages");
    }
}

void PluggableView::ui_event_message_ready(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_READY) {
        ILOG_DEBUG("ui_event_message_ready -> LV_EVENT_READY");
        char *txt = (char *)lv_textarea_get_text(objects.message_input_area);
        uint32_t len = strlen(txt);
        if (len) {
            ILOG_INFO("");
            // lv_textarea_set_text(objects.message_input_area, "");
            //     if (!lv_obj_has_flag(objects.keyboard, LV_OBJ_FLAG_HIDDEN)) {
            //         THIS->hideKeyboard(objects.messages_panel);
            //     }
            lv_group_focus_obj(objects.message_input_area);
        }
    } else if (event_code == LV_EVENT_CANCEL) {
        ILOG_DEBUG("ui_event_message_ready -> LV_EVENT_CANCEL");

    } else if (event_code == LV_EVENT_VALUE_CHANGED) {
        ILOG_DEBUG("ui_event_message_ready -> LV_EVENT_VALUE_CHANGED");
    }
}

void PluggableView::ui_event_MapButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.map, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->mapGroup);
        THIS->loadMap();
        lv_group_focus_obj(objects.top_map_back_button);
        lv_obj_remove_state(objects.map_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Map");
    }
}

void PluggableView::ui_event_ClockButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.clock, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->clockGroup);
        lv_group_focus_obj(objects.top_clock_back_button);
        lv_obj_remove_state(objects.clock_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Clock");
    }
}

void PluggableView::ui_event_MusicButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->musicGroup);
        // lv_group_focus_obj(objects.top_music_back_button);
        lv_obj_remove_state(objects.music_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Music");
    }
}

void PluggableView::ui_event_StatisticsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->statisticsGroup);
        // lv_group_focus_obj(objects.top_statistics_back_button);
        lv_obj_remove_state(objects.statistics_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Statistics");
    }
}

void PluggableView::ui_event_ToolsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.home, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->toolsGroup);
        // lv_group_focus_obj(objects.top_tools_back_button);
        lv_obj_remove_state(objects.tools_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Tools");
    }
}

void PluggableView::ui_event_SettingsButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        lv_screen_load_anim(objects.settings, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, false);
        lv_indev_set_group(THIS->indev, THIS->settingsGroup);
        // lv_group_focus_obj(objects.top_settings_back_button);
        lv_obj_remove_state(objects.settings_button, LV_STATE_CHECKED | LV_STATE_PRESSED);
    } else if (event_code == LV_EVENT_FOCUSED) {
        lv_label_set_text(objects.menu_label, "Settings");
    }
}

void PluggableView::ui_event_PowerButton(lv_event_t *e)
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

    // This must be run before displaying LoRa frequency as channel of 0 ("calculate from hash") leads to an integer underflow
    if (!db.config.lora.channel_num) {
        db.config.lora.channel_num = LoRaPresets::getDefaultSlot(db.config.lora.region, THIS->db.config.lora.modem_preset,
                                                                 THIS->db.channel[0].settings.name);
    }

    if (dashboard)
        dashboard->updateLoRaConfig(db.config.lora);

#if 0
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
#endif
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
    if (cfg.gps_mode != meshtastic_Config_PositionConfig_GpsMode_NOT_PRESENT) {
        if (cfg.fixed_position && db.uiConfig.map_data.has_home) {
            updatePosition(ownNode, db.uiConfig.map_data.home.latitude, db.uiConfig.map_data.home.longitude, 0, 0, 0);
        }
        // grey out text to indicate it's a fixed position vs. actual GPS position
        // Themes::recolorText(objects.home_location_label, !cfg.fixed_position);
    }
    // Themes::recolorButton(objects.home_location_button, cfg.gps_mode == meshtastic_Config_PositionConfig_GpsMode_ENABLED);
}

void PluggableView::updateConnectionStatus(const meshtastic_DeviceConnectionStatus &status)
{
    db.connectionStatus = status;
    if (status.has_wifi) {
        if (db.config.network.wifi_enabled || db.config.network.eth_enabled) {
            if (status.wifi.has_status) {
                char buf[20];
                uint32_t ip = status.wifi.status.ip_address;
                sprintf(buf, "%d.%d.%d.%d", ip & 0xff, (ip & 0xff00) >> 8, (ip & 0xff0000) >> 16, (ip & 0xff000000) >> 24);
                lv_label_set_text(objects.home_wlan_label, buf);
                // Themes::recolorButton(objects.home_wlan_button, true);
                // Themes::recolorText(objects.home_wlan_label, true);
                if (status.wifi.status.is_connected) {
                    lv_obj_set_style_bg_img_src(objects.home_wlan_button, &img_home_wlan_icon, LV_PART_MAIN | LV_STATE_DEFAULT);
                } else {
                    lv_obj_set_style_bg_img_src(objects.home_wlan_button, &img_home_wlan_off_icon,
                                                LV_PART_MAIN | LV_STATE_DEFAULT);
                }

                if (status.wifi.status.is_mqtt_connected) {
                    // Themes::recolorButton(objects.home_mqtt_button, true, 255);
                    // Themes::recolorText(objects.home_mqtt_label, true);
                } else {
                    // Themes::recolorButton(objects.home_mqtt_button, db.module_config.mqtt.enabled);
                    // Themes::recolorText(objects.home_mqtt_label, false);
                }
            }
        } else {
            // Themes::recolorButton(objects.home_wlan_button, false);
            // Themes::recolorText(objects.home_wlan_label, false);
            if (status.wifi.status.is_mqtt_connected) {
                // Themes::recolorButton(objects.home_mqtt_button, true, 255);
                // Themes::recolorText(objects.home_mqtt_label, true);
            } else {
                // Themes::recolorButton(objects.home_mqtt_button, db.module_config.mqtt.enabled, 100);
                // Themes::recolorText(objects.home_mqtt_label, false);
            }
            lv_obj_set_style_bg_img_src(objects.home_wlan_button, &img_home_wlan_off_icon, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    } else {
        lv_obj_add_flag(objects.home_wlan_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(objects.home_wlan_button, LV_OBJ_FLAG_HIDDEN);
    }
#if 0
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
#endif
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
        if (dashboard)
            dashboard->updatePosition(lat, lon, alt, sats, precision,
                                      db.config.display.units == meshtastic_Config_DisplayConfig_DisplayUnits_METRIC);

        if (lat != 0 && lon != 0) {
            hasPosition = true;
            myLatitude = lat;
            myLongitude = lon;
#if 0
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
#endif
            // update own location on map
            if (map)
                map->setGpsPosition(lat * 1e-7, lon * 1e-7);
        }
    } else {
        if (lat != 0 && lon != 0) {
            if (hasPosition) {
                // updateDistance(nodeNum, lat, lon);
            }
            addOrUpdateMap(nodeNum, lat, lon);
        }
    }
#if 0
    if (lat != 0 && lon != 0) {
        int32_t altU = abs(alt) < 10000 ? alt : 0;
        char units[3] = {};
        if (db.config.display.units == meshtastic_Config_DisplayConfig_DisplayUnits_METRIC) {
            units[0] = 'm';
        } else {
            units[0] = 'f';
            units[1] = 't';
            altU = int32_t(float(altU) * 3.28084);
        }
        char buf[32];
        sprintf(buf, "%.5f %.5f", lat * 1e-7, lon * 1e-7);
        lv_obj_t *panel = nodes[nodeNum];
        lv_label_set_text(panel->LV_OBJ_IDX(node_pos1_idx), buf);
        if (sats)
            sprintf(buf, "%d%s MSL  %u sats", altU, units, sats);
        sprintf(buf, "%d%s MSL", altU, units);
        lv_label_set_text(panel->LV_OBJ_IDX(node_pos2_idx), buf);
        // store lat/lon in user_data, because we need these values later to calculate the distance to us
        panel->LV_OBJ_IDX(node_pos1_idx)->user_data = (void *)lat;
        panel->LV_OBJ_IDX(node_pos2_idx)->user_data = (void *)lon;
    }

    applyNodesFilter(nodeNum);
#endif
}

// nodes screen

void PluggableView::addOrUpdateNode(uint32_t nodeNum, uint8_t channel, const meshtastic_NodeInfo &node,
                                    const meshtastic_User &cfg)
{
    if (nodes.find(nodeNum) == nodes.end()) {
        addNode(nodeNum, channel, cfg.short_name, cfg.long_name, node.last_heard, (MeshtasticView::eRole)cfg.role,
                cfg.public_key.size != 0, node.is_favorite, node.is_ignored, cfg.has_is_unmessagable && cfg.is_unmessagable);
    } else {
        updateNode(nodeNum, channel, cfg);
    }
}

void PluggableView::addNode(uint32_t nodeNum, uint8_t ch, const char *userShort, const char *userLong, uint32_t lastHeard,
                            eRole role, bool hasKey, bool isFav, bool isIgnored, bool unmessagable)
{
    ILOG_DEBUG("addNode(%d): num=0x%08x, lastseen=%d, name=%s(%s), role=%d", nodeCount, nodeNum, lastHeard, userLong, userShort,
               role);

    lv_group_set_default(nodesGroup);

    // NodeButton
    lv_obj_t *obj = lv_button_create(objects.nodes_panel);
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, LV_PCT(100), 20);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff444444), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xfff0f0f0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff707070), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xff24fb00), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_PRESSED);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, -14, 0);
            lv_obj_set_size(obj, 20, 20);
            if (isFav)
                lv_image_set_src(obj, &img_heart_image);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // NodeShortName
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.node_short_name = obj;
            lv_obj_set_pos(obj, 10, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, userShort);
        }
        {
            // NodeLongName
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.node_long_name = obj;
            lv_obj_set_pos(obj, 70, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, userLong);
        }
    }

    nodes[nodeNum] = obj;
    nodeCount++;
    updateNodesStatus();
}

void PluggableView::updateNode(uint32_t nodeNum, uint8_t ch, const meshtastic_User &cfg)
{
    auto it = nodes.find(nodeNum);
    if (it != nodes.end() && it->second) {
        if (it->first == ownNode) {
            if (node)
                node->updateName(cfg.short_name, cfg.long_name);
        }
    }
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
    return cardDetected;
}

void PluggableView::loadMap(void)
{
    if (!map) {
        lv_group_set_default(mapGroup);
#if LV_USE_FS_ARDUINO_SD
        map = new MapPanel(objects.map_panel);
#elif defined(HAS_SD_MMC)
        map = new MapPanel(objects.map_panel, new SDCardService());
#elif defined(HAS_SDCARD)
        map = new MapPanel(objects.map_panel, new SdFatService());
#elif defined(ARCH_PORTDUINO)
        map = new MapPanel(objects.map_panel, new SDCardService()); // TODO: LinuxFileSystemService
#else
        map = new MapPanel(objects.map_panel);
#endif
        MapTileSettings::setDebug(true);
#if 0
        if (!nodeObjects.empty()) {
            for (auto it : nodeObjects) {
                lv_obj_t *p = nodes[it.first];
                float lat = 1e-7 * (long)p->LV_OBJ_IDX(node_pos1_idx)->user_data;
                float lon = 1e-7 * (long)p->LV_OBJ_IDX(node_pos2_idx)->user_data;
                map->add(it.first, lat, lon, drawObjectCB);
                lv_obj_add_flag(it.second, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_event_cb(it.second, ui_event_mapNodeButton, LV_EVENT_CLICKED, (void *)it.first);
            }
        }
#endif
    }

    if (sdCard) {
        if (!sdCard->isUpdated()) {
            map->setNoTileImage(&img_no_tile_image);
            std::set<std::string> mapStyles = sdCard->loadMapStyles(MapTileSettings::getPrefix());
            if (mapStyles.find("/map") != mapStyles.end()) {
                // no styles found, but the /map directory, so use it
                MapTileSettings::setPrefix("/map");
                MapTileSettings::setTileStyle("");
                // lv_obj_add_flag(objects.map_style_dropdown, LV_OBJ_FLAG_HIDDEN);
            }
#if 0
            else if (!mapStyles.empty()) {
                // populate dropdown
                uint16_t pos = 0;
                bool savedStyleOK = false;
                lv_dropdown_set_options(objects.map_style_dropdown, "");
                for (auto it : mapStyles) {
                    lv_dropdown_add_option(objects.map_style_dropdown, it.c_str(), pos);
                    if (it == db.uiConfig.map_data.style) {
                        lv_dropdown_set_selected(objects.map_style_dropdown, pos);
                        MapTileSettings::setTileStyle(db.uiConfig.map_data.style);
                        savedStyleOK = true;
                    }
                    pos++;
                }
                if (!savedStyleOK) {
                    // no such style on SD, pick first one we found
                    char style[20];
                    lv_dropdown_set_selected(objects.map_style_dropdown, 0);
                    lv_dropdown_get_selected_str(objects.map_style_dropdown, style, sizeof(style));
                    MapTileSettings::setTileStyle(style);
                }
                MapTileSettings::setPrefix("/maps");
            }
#endif
            else {
                // messageAlert(_("No map tiles found on SDCard!"), true);
                map->setNoTileImage(&img_no_tile_image);
            }
            map->forceRedraw();
        }
    } else {
        // lv_dropdown_set_options(objects.map_style_dropdown, "");
    }

    lv_obj_clear_flag(objects.map_panel, LV_OBJ_FLAG_HIDDEN);
}

void PluggableView::updateLocationMap(uint32_t num)
{
    lv_label_set_text_fmt(objects.top_map_label, _("Locations Map (%d/%d)"), num, nodeCount);
}

/**
 * add node location image for display on map
 */
void PluggableView::addOrUpdateMap(uint32_t nodeNum, int32_t lat, int32_t lon)
{
    auto it = nodeObjects.find(nodeNum);
#if 0
    if (it == nodeObjects.end()) {
        uint32_t bgColor, fgColor;
        std::tie(bgColor, fgColor) = nodeColor(nodeNum);
        lv_obj_t *img = lv_image_create(objects.raw_map_panel);
        lv_obj_set_size(img, 40, 35);
        lv_img_set_src(img, &img_circle_image);
        lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TOP_MID);
        lv_obj_set_style_opa(img, 180, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_image_recolor(img, lv_color_hex(bgColor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_image_recolor_opa(img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(img, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(img, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_left(img, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(img, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *lbl = lv_label_create(img);
        lv_obj_set_pos(lbl, 0, 0);
        lv_obj_set_size(lbl, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_text_color(lbl, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_opa(img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_image_recolor_opa(img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_align(lbl, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_align(lbl, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *p = nodes[nodeNum];
        lv_label_set_text_fmt(lbl, "%s", lv_label_get_text(p->LV_OBJ_IDX(node_lbs_idx)));

        // position label callback
        lv_obj_add_flag(p->LV_OBJ_IDX(node_pos1_idx), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(p->LV_OBJ_IDX(node_pos1_idx), ui_event_positionButton, LV_EVENT_CLICKED, (void *)p);

        nodeObjects[nodeNum] = img;
        if (map) {
            map->add(nodeNum, lat * 1e-7, lon * 1e-7, drawObjectCB);
            lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(img, ui_event_mapNodeButton, LV_EVENT_CLICKED, (void *)nodeNum);
            updateLocationMap(map->getObjectsOnMap());
        }
    } else
#endif
    {
        if (map) {
            map->update(it->first, lat * 1e-7, lon * 1e-7);
        }
    }
}

void PluggableView::removeFromMap(uint32_t nodeNum)
{
    auto it = nodeObjects.find(nodeNum);
    if (it == nodeObjects.end())
        return;
#if 0
    lv_obj_t *img = it->second;
    if (map) {
        map->remove(it->first);
        updateLocationMap(map->getObjectsOnMap());
    }
    nodeObjects.erase(nodeNum);
    lv_obj_remove_event_cb(img, ui_event_mapNodeButton);
    lv_obj_delete(img);
#endif
}

#if 0
void PluggableView::ui_screen_event_cb(lv_event_t *e)
{
    if (THIS->activePanel == objects.map_panel) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch (dir) {
        case LV_DIR_LEFT:
            e->user_data = (void *)scrollRight;
            break;
        case LV_DIR_RIGHT:
            e->user_data = (void *)scrollLeft;
            break;
        case LV_DIR_TOP:
            e->user_data = (void *)scrollDown;
            break;
        case LV_DIR_BOTTOM:
            e->user_data = (void *)scrollUp;
            break;
        default:
            break;
        }
        ILOG_DEBUG("gesture: %d", (uint16_t)dir);
        THIS->ui_event_arrow(e);
    }
}

void PluggableView::ui_event_arrow(lv_event_t *e)
{
    if (THIS->map && THIS->map->redrawComplete()) {
        uint16_t deltaX = 0;
        uint16_t deltaY = 0;
        ScrollDirection direction = (ScrollDirection)(unsigned long)e->user_data;
        switch (direction) {
        case scrollDownLeft:
            deltaX = 1;
            deltaY = -1;
            break;
        case scrollDown:
            deltaX = 0;
            deltaY = -1;
            break;
        case scrollDownRight:
            deltaX = -1;
            deltaY = -1;
            break;
        case scrollLeft:
            deltaX = 1;
            deltaY = 0;
            break;
        case scrollRight:
            deltaX = -1;
            deltaY = 0;
            break;
        case scrollUpLeft:
            deltaX = 1;
            deltaY = 1;
            break;
        case scrollUp:
            deltaX = 0;
            deltaY = 1;
            break;
        case scrollUpRight:
            deltaX = -1;
            deltaY = 1;
            break;
        default:
            break;
        };
        if (!THIS->map->scroll(deltaX, deltaY))
            THIS->map->forceRedraw();
    }
    THIS->updateLocationMap(THIS->map->getObjectsOnMap());
}
#endif

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

void PluggableView::task_handler(void)
{
    MeshtasticView::task_handler();

    if (screensInitialised) {
        if (map) {
            lv_group_t *current = lv_group_get_default();
            lv_group_set_default(mapGroup); // TODO: add MapPanel::setGroup() interface
            map->task_handler();
            lv_group_set_default(current);
        }

        if (curtime - lastrun1 >= 1) { // call every 1s
            if (map) {
                lv_group_t *current = lv_group_get_default();
                lv_group_set_default(mapGroup);
                updateLocationMap(THIS->map->getObjectsOnMap());
                lv_group_set_default(current);
            }

            lastrun1 = curtime;
            actTime++;
            updateTime();
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
        // updateAllLastHeard();

        // if (detectorRunning) {
        //     controller->sendPing();
        // }

        // if we didn't hear any node for 1h assume we have no signal
        if (curtime - lastHeard > secs_until_offline) {
            lv_obj_set_style_bg_image_src(objects.home_signal_button, &img_home_no_signal_icon, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(objects.home_signal_label, _("no signal"));
            lv_label_set_text(objects.home_signal_pct_label, "");
        }
    }
}

#endif