#include "graphics/plugin/Plugins.h"

#ifdef MUI_SCROLLMENU_PLUGIN

#include "Arduino.h"
#include "graphics/plugin/ScrollMenuPlugin.h"
#include "images.h"
#include "lv_i18n.h"
#include "lvgl.h"

#ifndef MENU_PLUGIN_CUSTOM_WIDGET_NAMES
#include "screens.h"
#endif

static ScrollMenuPlugin *p = nullptr;

ScrollMenuPlugin::ScrollMenuPlugin() : GfxPlugin("ScrollMenu") {}

ScrollMenuPlugin::~ScrollMenuPlugin() = default;

void ScrollMenuPlugin::init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount, lv_group_t *group,
                            lv_indev_t *indev, GfxPlugin::RegisterWidget registerWidget)
{
    p = this;
    GfxPlugin::init(parent, resolver, widgetCount, group, indev, registerWidget);
}

void ScrollMenuPlugin::loadScreen(lv_screen_load_anim_t anim, uint32_t time)
{
    GfxPlugin::loadScreen(anim, time);
    lv_obj_t *home_button = p->getWidget(static_cast<WidgetIndex>(Widget::HomeButton));
    if (home_button)
        lv_obj_remove_state(home_button, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
}

void ScrollMenuPlugin::registerStandardWidgets(void)
{
#ifndef MENU_PLUGIN_CUSTOM_WIDGET_NAMES
    // register default widgets that were created by the generated UI
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::MenuLabel), objects.menu_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::HomeButton), objects.home_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::NodesButton), objects.nodes_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::GroupsButton), objects.groups_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::MessagesButton), objects.messages_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::MapButton), objects.map_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::ClockButton), objects.clock_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::MusicButton), objects.music_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::StatisticsButton), objects.statistics_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::ToolsButton), objects.tools_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::SettingsButton), objects.settings_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ScrollMenuPlugin::Widget::PowerButton), objects.power_button);
#endif
}

// not used
void ScrollMenuPlugin::registerStandardWidgetActions(void)
{
    registerWidgetAction(static_cast<WidgetIndex>(Widget::HomeButton), static_cast<GfxPlugin::Action>(Action::OpenHome));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::NodesButton), static_cast<GfxPlugin::Action>(Action::OpenNodes));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::GroupsButton), static_cast<GfxPlugin::Action>(Action::OpenGroups));
    registerWidgetAction(static_cast<WidgetIndex>(Widget::MessagesButton), static_cast<GfxPlugin::Action>(Action::OpenMessages));
}

void ScrollMenuPlugin::registerStandardEventCallbacks(void)
{
    lv_obj_t *home_button = p->getWidget(static_cast<WidgetIndex>(Widget::HomeButton));
    if (home_button)
        lv_obj_add_event_cb(home_button, this->ui_event_MenuButton, LV_EVENT_ALL, (void *)&onOpenHome);

    lv_obj_t *nodes_button = p->getWidget(static_cast<WidgetIndex>(Widget::NodesButton));
    if (nodes_button)
        lv_obj_add_event_cb(nodes_button, this->ui_event_MenuButton, LV_EVENT_ALL, (void *)&onOpenNodes);

    lv_obj_t *groups_button = p->getWidget(static_cast<WidgetIndex>(Widget::GroupsButton));
    if (groups_button)
        lv_obj_add_event_cb(groups_button, this->ui_event_MenuButton, LV_EVENT_ALL, (void *)&onOpenGroups);

    lv_obj_t *messages_button = p->getWidget(static_cast<WidgetIndex>(Widget::MessagesButton));
    if (messages_button)
        lv_obj_add_event_cb(messages_button, this->ui_event_MenuButton, LV_EVENT_ALL, (void *)&onOpenMessages);

    lv_obj_t *map_button = p->getWidget(static_cast<WidgetIndex>(Widget::MapButton));
    if (map_button)
        lv_obj_add_event_cb(map_button, this->ui_event_MenuButton, LV_EVENT_ALL, (void *)&onOpenMap);

    lv_obj_t *clock_button = p->getWidget(static_cast<WidgetIndex>(Widget::ClockButton));
    if (clock_button)
        lv_obj_add_event_cb(clock_button, this->ui_event_MenuButton, LV_EVENT_ALL, (void *)&onOpenClock);

    lv_obj_t *music_button = p->getWidget(static_cast<WidgetIndex>(Widget::MusicButton));
    if (music_button)
        lv_obj_add_event_cb(music_button, this->ui_event_MenuButton, LV_EVENT_ALL, (void *)&onOpenMusic);

    lv_obj_t *statistics_button = p->getWidget(static_cast<WidgetIndex>(Widget::StatisticsButton));
    if (statistics_button)
        lv_obj_add_event_cb(statistics_button, this->ui_event_MenuButton, LV_EVENT_ALL, (void *)&onOpenStatistics);

    lv_obj_t *tools_button = p->getWidget(static_cast<WidgetIndex>(Widget::ToolsButton));
    if (tools_button)
        lv_obj_add_event_cb(tools_button, this->ui_event_MenuButton, LV_EVENT_ALL, (void *)&onOpenTools);

    lv_obj_t *settings_button = p->getWidget(static_cast<WidgetIndex>(Widget::SettingsButton));
    if (settings_button)
        lv_obj_add_event_cb(settings_button, this->ui_event_MenuButton, LV_EVENT_ALL, (void *)&onOpenSettings);

    lv_obj_t *power_button = p->getWidget(static_cast<WidgetIndex>(Widget::PowerButton));
    if (power_button)
        lv_obj_add_event_cb(power_button, this->ui_event_MenuButton, LV_EVENT_ALL, (void *)&onPower);
}

void ScrollMenuPlugin::ui_event_MenuButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == EVENT_CLICKED) {
        ScrollMenuPlugin::Callback *onPress = (ScrollMenuPlugin::Callback *)(lv_event_get_user_data(e));
        if (onPress && onPress->cb)
            onPress->cb(e);
    } else if (event_code == LV_EVENT_FOCUSED || event_code == LV_EVENT_PRESSED) {
        lv_obj_t *menuLbl = p->getWidget(static_cast<WidgetIndex>(Widget::MenuLabel));
        if (menuLbl) {
            ScrollMenuPlugin::Callback *cbLabel = (ScrollMenuPlugin::Callback *)(lv_event_get_user_data(e));
            lv_label_set_text(menuLbl, cbLabel ? cbLabel->name : _("<defunct>"));
        }
    }
}

#endif