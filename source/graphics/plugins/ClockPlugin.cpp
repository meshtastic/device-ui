#include "graphics/plugin/Plugins.h"

#ifdef MUI_CLOCK_PLUGIN

#include "Arduino.h"
#include "graphics/plugin/ClockPlugin.h"
#include "images.h"
#include "lv_i18n.h"
#include "lvgl.h"
#include <cstdio>
#include <ctime>

#ifndef CLOCK_PLUGIN_CUSTOM_WIDGET_NAMES
#include "screens.h"
#endif

static ClockPlugin *p = nullptr;

ClockPlugin::ClockPlugin() : GfxPlugin("Clock") {}

ClockPlugin::~ClockPlugin() = default;

void ClockPlugin::init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount, lv_group_t *group, lv_indev_t *indev,
                       GfxPlugin::RegisterWidget registerWidget)
{
    p = this;
    GfxPlugin::init(parent, resolver, widgetCount, group, indev, registerWidget);
}

void ClockPlugin::registerStandardWidgets(void)
{
#ifndef CLOCK_PLUGIN_CUSTOM_WIDGET_NAMES
    // register widgets that were created by the generated UI
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ClockPlugin::Widget::TimezoneLabel), objects.top_clock_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ClockPlugin::Widget::TimeLabel), objects.clock_time_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(ClockPlugin::Widget::SecondsLabel), objects.clock_seconds_label);
#endif
    // cache lbl
    timeLbl = getWidget(static_cast<WidgetIndex>(Widget::TimeLabel));
    secondsLbl = getWidget(static_cast<WidgetIndex>(Widget::SecondsLabel));
}

void ClockPlugin::registerStandardWidgetActions(void)
{
    // registerWidgetAction(static_cast<WidgetIndex>(Widget::ClockButton), static_cast<GfxPlugin::Action>(Action::OpenSomething));
}

void ClockPlugin::handleAction(Action actionId, WidgetIndex /*idx*/, int /*event_code*/)
{
    switch (actionId) {
    default:
        invokeActionCallback(static_cast<GfxPlugin::Action>(actionId), nullptr);
        break;
    }
}

void ClockPlugin::updateTime(uint32_t unixTime, bool use_12h_clock)
{
    if (timeLbl) {
        time_t t = (time_t)unixTime;
        tm *curr_tm = localtime(&t);
        char buf[80];
        char buf_sec[4];
        if (t > 1000000) {
            if (use_12h_clock) {
                strftime(buf, sizeof(buf), "%I:%M", curr_tm);
            } else {
                strftime(buf, sizeof(buf), "%I:%M", curr_tm);
            }
            strftime(buf_sec, sizeof(buf_sec), "%S", curr_tm);
        } else {
            // show uptime if no valid time
            uint32_t uptime_s = millis() / 1000;
            int hours = uptime_s / 3600;
            uptime_s -= hours * 3600;
            int minutes = uptime_s / 60;
            int seconds = uptime_s - minutes * 60;
            snprintf(buf, sizeof(buf), "%02d:%02d", hours, minutes);
            snprintf(buf_sec, sizeof(buf_sec), "%02d", seconds);
        }
        lv_label_set_text(timeLbl, buf);
        lv_label_set_text(secondsLbl, buf_sec);
    }
}

void ClockPlugin::updateTimezone(uint32_t unixTime)
{
    lv_obj_t *timezone = getWidget(static_cast<WidgetIndex>(Widget::TimezoneLabel));
    if (timezone) {
        time_t t = (time_t)unixTime;
        tm *curr_tm = localtime(&t);
        char buf[30];
        if (t > 1000000) {
            strftime(buf, sizeof(buf), "Timezone: %Z%z", curr_tm);
            lv_label_set_text(timezone, buf);
        } else {
            lv_label_set_text(timezone, "Uptime");
        }
    }
}

#endif