#include "graphics/plugin/Plugins.h"

#ifdef MUI_NODES_PLUGIN

#include "Arduino.h"
#include "graphics/plugin/NodesPlugin.h"
#include "images.h"
#include "lv_i18n.h"
#include "lvgl.h"
#include <cstdio>
#include <ctime>

#ifndef NODES_PLUGIN_CUSTOM_WIDGET_NAMES
#include "screens.h"
#endif

static NodesPlugin *p = nullptr;

NodesPlugin::NodesPlugin() : GfxPlugin("Nodes") {}

NodesPlugin::~NodesPlugin() = default;

void NodesPlugin::init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount, lv_group_t *group, lv_indev_t *indev,
                       GfxPlugin::RegisterWidget registerWidget)
{
    p = this;
    GfxPlugin::init(parent, resolver, widgetCount, group, indev, registerWidget);
}

void NodesPlugin::registerStandardWidgets(void)
{
#ifndef NODES_PLUGIN_CUSTOM_WIDGET_NAMES
    // register widgets that were created by the generated UI
    setWidget(static_cast<GfxPlugin::WidgetIndex>(NodesPlugin::Widget::NodesOnlineLabel), objects.top_nodes_online_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(NodesPlugin::Widget::NodeButton), objects.node_button);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(NodesPlugin::Widget::ShortNameLabel), objects.node_short_name);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(NodesPlugin::Widget::LongNameLabel), objects.node_long_name);
#endif
}

void NodesPlugin::registerStandardWidgetActions(void)
{
    // registerWidgetAction(static_cast<WidgetIndex>(Widget::NodeButton), static_cast<GfxPlugin::Action>(Action::OpenSomething));
}

void NodesPlugin::registerStandardEventCallbacks(void)
{
    lv_obj_t *node_button = p->getWidget(static_cast<WidgetIndex>(Widget::NodeButton));
    // if (node_button)
    //     lv_obj_add_event_cb(node_button, this->ui_event_button, LV_EVENT_ALL, (void *)&onOpenNode);
}

void NodesPlugin::handleAction(Action actionId, WidgetIndex /*idx*/, int /*event_code*/)
{
    switch (actionId) {
    default:
        invokeActionCallback(static_cast<GfxPlugin::Action>(actionId), nullptr);
        break;
    }
}

void NodesPlugin::updateNodesOnline(uint32_t online, uint32_t nodeCount)
{
    lv_obj_t *nodesOnlineLbl = getWidget(static_cast<WidgetIndex>(Widget::NodesOnlineLabel));
    if (nodesOnlineLbl) {
        lv_label_set_text_fmt(objects.top_home_label, "%d of %d nodes online", online, nodeCount);
    }
}

void NodesPlugin::updateName(const char *shortName, const char *longName)
{
    lv_obj_t *shortNameLbl = getWidget(static_cast<WidgetIndex>(Widget::ShortNameLabel));
    if (shortNameLbl) {
        lv_label_set_text(shortNameLbl, shortName);
    }
    lv_obj_t *longNameLbl = getWidget(static_cast<WidgetIndex>(Widget::LongNameLabel));
    if (longNameLbl) {
        lv_label_set_text(longNameLbl, longName);
    }
}

#endif