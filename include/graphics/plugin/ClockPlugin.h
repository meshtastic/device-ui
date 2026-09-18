#pragma once

#include "GfxPlugin.h"
#include "meshtastic/clientonly.pb.h"
#include "meshtastic/mesh.pb.h"
#include <cstdint>
#include <string>

/**
 * ClockPlugin - implements business logic for the clock panel
 */
class ClockPlugin : public GfxPlugin
{
  public:
    // widget slots used by dashboard
    enum class Widget : GfxPlugin::WidgetIndex {
        TimezoneLabel = 0,
        TimeLabel,
        SecondsLabel,
        // add more as required but keep below MAX_WIDGETS
        Count
    };

    static constexpr std::size_t WIDGET_COUNT = static_cast<std::size_t>(Widget::Count);

    ClockPlugin();
    virtual ~ClockPlugin();

    // init override: store resolver/parent and optionally auto-register widgets by name
    void init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount = WIDGET_COUNT, lv_group_t *group = nullptr,
              lv_indev_t *indev = nullptr, GfxPlugin::RegisterWidget registerWidget = GfxPlugin::RegisterWidget::All) override;

    // Register clock widgets with default names
    void registerStandardWidgets(void);
    // Register widget indices to compact Action values in the base class.
    void registerStandardWidgetActions(void);

    // Business logic methods: update UI
    void updateTime(uint32_t unixTime, bool use_12h_clock);
    void updateTimezone(uint32_t unixTime);

  protected:
    // handleAction: map compact Action values to plugin callbacks
    void handleAction(Action actionId, WidgetIndex idx, int event_code) /*override*/;

  private:
    lv_obj_t *timeLbl = nullptr;
    lv_obj_t *secondsLbl = nullptr;
};
