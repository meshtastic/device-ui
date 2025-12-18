#include "graphics/plugin/GfxPlugin.h"
#include "graphics/plugin/PluginRegistry.h"
#include "lvgl.h"

PluginRegistry registry;

GfxPlugin::GfxPlugin(const std::string &name)
{
    registry.registerPlugin(name, this);
}

GfxPlugin::~GfxPlugin()
{
    registry.deregisterPlugin(this);
}

void GfxPlugin::init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount, lv_group_t *group, lv_indev_t *indev,
                     GfxPlugin::RegisterWidget registerWidget)
{
    this->parent = parent;
    this->resolver = std::move(resolver);
    this->group = group;
    this->indev = indev;
    this->widgetCount = (widgetCount <= MAX_WIDGETS) ? widgetCount : MAX_WIDGETS;
    widgets.fill(nullptr);
    widgetActions.fill(Action::None);

    if (int8_t(registerWidget) & int8_t(GfxPlugin::RegisterWidget::Standard))
        this->registerStandardWidgets();
    if (int8_t(registerWidget) & int8_t(GfxPlugin::RegisterWidget::Events))
        this->registerStandardEventCallbacks();
    if (int8_t(registerWidget) & int8_t(GfxPlugin::RegisterWidget::Actions))
        this->registerStandardWidgetActions();
}

void GfxPlugin::deinit()
{
    widgets.fill(nullptr);
    widgetActions.fill(Action::None);
    resolver = nullptr;
    parent = nullptr;
    group = nullptr;
    indev = nullptr;
    widgetCount = 0;
    actionCallback = nullptr;
    lvEventCb = nullptr;
}

void GfxPlugin::loadScreen(lv_screen_load_anim_t anim, uint32_t time)
{
    if (parent)
        lv_screen_load_anim(parent, anim, time, 0, false);
    if (indev && group)
        lv_indev_set_group(indev, group);
}

void GfxPlugin::setWidget(WidgetIndex idx, lv_obj_t *obj, bool onEvent)
{
    if (idx >= widgetCount)
        return;
    widgets[idx] = obj;
    if (!obj)
        return;
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    if (onEvent) {
        // attach an event callback that forwards to plugin->onEvent()
        lv_obj_add_event_cb(
            obj,
            [](lv_event_t *e) {
                GfxPlugin *p = reinterpret_cast<GfxPlugin *>(lv_event_get_user_data(e));
                p->onEvent(lv_event_get_target_obj(e), lv_event_get_code(e));
            },
            LV_EVENT_ALL, this);
    }
}
