#include "graphics/plugin/Plugins.h"

#ifdef MUI_GROUPS_PLUGIN

#include "Arduino.h"
#include "graphics/plugin/GroupsPlugin.h"
#include "images.h"
#include "lv_i18n.h"
#include "lvgl.h"
#include "util/ILog.h"
#include <cstdio>
#include <ctime>

#ifndef GROUPS_PLUGIN_CUSTOM_WIDGET_NAMES
#include "screens.h"
#endif

static GroupsPlugin *p = nullptr;

GroupsPlugin::GroupsPlugin() : GfxPlugin("Groups") {}

GroupsPlugin::~GroupsPlugin() = default;

void GroupsPlugin::init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount, lv_group_t *group, lv_indev_t *indev,
                        GfxPlugin::RegisterWidget registerWidget)
{
    p = this;
    GfxPlugin::init(parent, resolver, widgetCount, group, indev, registerWidget);
}

void GroupsPlugin::registerStandardWidgets(void)
{
#ifndef GROUPS_PLUGIN_CUSTOM_WIDGET_NAMES
    // register widgets that were created by the generated UI
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group0Label), objects.group_name_0);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group0Button), objects.group_button_0);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group1Label), objects.group_name_1);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group1Button), objects.group_button_1);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group2Label), objects.group_name_2);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group2Button), objects.group_button_2);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group3Label), objects.group_name_3);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group3Button), objects.group_button_3);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group4Label), objects.group_name_4);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group4Button), objects.group_button_4);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group5Label), objects.group_name_5);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group5Button), objects.group_button_5);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group6Label), objects.group_name_6);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group6Button), objects.group_button_6);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group7Label), objects.group_name_7);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(GroupsPlugin::Widget::Group7Button), objects.group_button_7);
#endif
}

void GroupsPlugin::registerStandardWidgetActions(void)
{
    // registerWidgetAction(static_cast<WidgetIndex>(Widget::Group0Button),
    // static_cast<GfxPlugin::Action>(Action::OpenSomething));
}

void GroupsPlugin::registerStandardEventCallbacks(void)
{
    lv_obj_t *group_button = p->getWidget(static_cast<WidgetIndex>(Widget::Group0Button));
    // if (group_button)
    //     lv_obj_add_event_cb(group_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onOpenGroup);
}

void GroupsPlugin::handleAction(Action actionId, WidgetIndex /*idx*/, int /*event_code*/)
{
    switch (actionId) {
    default:
        invokeActionCallback(static_cast<GfxPlugin::Action>(actionId), nullptr);
        break;
    }
}

void GroupsPlugin::updateName(uint8_t index, const char *groupName)
{
    lv_obj_t *nameWidget = getGroupNameWidget(index);
    if (nameWidget) {
        lv_label_set_text(nameWidget, groupName);
    }
}

lv_obj_t *GroupsPlugin::getGroupNameWidget(uint8_t index)
{
    switch (index) {
    case 0:
        return getWidget(static_cast<WidgetIndex>(Widget::Group0Label));
    case 1:
        return getWidget(static_cast<WidgetIndex>(Widget::Group1Label));
    case 2:
        return getWidget(static_cast<WidgetIndex>(Widget::Group2Label));
    case 3:
        return getWidget(static_cast<WidgetIndex>(Widget::Group3Label));
    case 4:
        return getWidget(static_cast<WidgetIndex>(Widget::Group4Label));
    case 5:
        return getWidget(static_cast<WidgetIndex>(Widget::Group5Label));
    case 6:
        return getWidget(static_cast<WidgetIndex>(Widget::Group6Label));
    case 7:
        return getWidget(static_cast<WidgetIndex>(Widget::Group7Label));
    default:
        ILOG_ERROR("getGroupNameWidget: invalid index %d", index);
        return nullptr;
    }
}

lv_obj_t *GroupsPlugin::getGroupButtonWidget(uint8_t index)
{
    switch (index) {
    case 0:
        return getWidget(static_cast<WidgetIndex>(Widget::Group0Button));
    case 1:
        return getWidget(static_cast<WidgetIndex>(Widget::Group1Button));
    case 2:
        return getWidget(static_cast<WidgetIndex>(Widget::Group2Button));
    case 3:
        return getWidget(static_cast<WidgetIndex>(Widget::Group3Button));
    case 4:
        return getWidget(static_cast<WidgetIndex>(Widget::Group4Button));
    case 5:
        return getWidget(static_cast<WidgetIndex>(Widget::Group5Button));
    case 6:
        return getWidget(static_cast<WidgetIndex>(Widget::Group6Button));
    case 7:
        return getWidget(static_cast<WidgetIndex>(Widget::Group7Button));
    default:
        ILOG_ERROR("getGroupButtonWidget: invalid index %d", index);
        return nullptr;
    }
}

#endif