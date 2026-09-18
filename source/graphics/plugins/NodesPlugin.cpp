#include "graphics/plugin/Plugins.h"

#ifdef MUI_NODES_PLUGIN

#include "Arduino.h"
#include "graphics/plugin/ListRowStyle.h"
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
    auto *row = getWidget(static_cast<WidgetIndex>(Widget::NodeButton));
    if (row) {
        ListRowStyle::container(lv_obj_get_parent(row));
        const std::string shortName = lv_label_get_text(getWidget(static_cast<WidgetIndex>(Widget::ShortNameLabel)));
        const std::string longName = lv_label_get_text(getWidget(static_cast<WidgetIndex>(Widget::LongNameLabel)));
        updateRow(row, shortName.c_str(), longName.c_str(), false);
    }
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
    // registerWidgetAction(static_cast<WidgetIndex>(Widget::NodeButton),
    // static_cast<GfxPlugin::Action>(Action::OpenSomething));
}

void NodesPlugin::registerStandardEventCallbacks(void)
{
    // bindRow attaches the callback once the view knows this row's node ID.
}

void NodesPlugin::bindRow(lv_obj_t *row, uint32_t nodeId, uint8_t channel)
{
    if (!row)
        return;
    lv_obj_set_user_data(row, reinterpret_cast<void *>(static_cast<uintptr_t>(channel)));
    lv_obj_remove_event_cb(row, ui_event_button);
    lv_obj_add_event_cb(row, ui_event_button, LV_EVENT_ALL, reinterpret_cast<void *>(static_cast<uintptr_t>(nodeId)));
    if (group && lv_obj_get_group(row) != group)
        lv_group_add_obj(group, row);
}

lv_obj_t *NodesPlugin::createRow(lv_obj_t *parent, uint32_t nodeId, uint8_t channel, const char *shortName, const char *longName,
                                 bool favorite)
{
    auto *row = lv_button_create(parent);
    // Keep the public row child order used by chat headings: image, short, long.
    lv_image_create(row);
    lv_label_create(row);
    lv_label_create(row);
    updateRow(row, shortName, longName, favorite);
    bindRow(row, nodeId, channel);
    return row;
}

void NodesPlugin::updateRow(lv_obj_t *row, const char *shortName, const char *longName, bool favorite)
{
    if (!row || lv_obj_get_child_count(row) < 3)
        return;
    ListRowStyle::row(row);
    lv_obj_set_height(row, 53);
    auto *icon = lv_obj_get_child(row, 0);
    auto *shortLabel = lv_obj_get_child(row, 1);
    auto *longLabel = lv_obj_get_child(row, 2);
    lv_image_set_src(icon, favorite ? &img_heart_image : &img_home_nodes_icon);
    lv_obj_set_size(icon, 32, 32);
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);
    ListRowStyle::text(icon);
    lv_obj_align(shortLabel, LV_ALIGN_TOP_LEFT, 0, 1);
    lv_obj_set_size(shortLabel, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_left(shortLabel, 42, 0);
    lv_label_set_long_mode(shortLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(shortLabel, shortName ? shortName : "");
    ListRowStyle::text(shortLabel);
    lv_obj_align(longLabel, LV_ALIGN_TOP_LEFT, 0, 22);
    lv_obj_set_size(longLabel, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_left(longLabel, 42, 0);
    lv_label_set_long_mode(longLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text(longLabel, longName ? longName : "");
    ListRowStyle::text(longLabel);
}

void NodesPlugin::ui_event_button(lv_event_t *e)
{
    const auto code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED && p->onNodeButton)
        p->onNodeButton(e);
    else if (code == LV_EVENT_LONG_PRESSED && p->onOpenNode)
        p->onOpenNode(e);
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
        lv_label_set_text_fmt(nodesOnlineLbl, _("%d of %d nodes online"), online, nodeCount);
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