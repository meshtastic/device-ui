#pragma once

#include "GfxPlugin.h"
#include "meshtastic/clientonly.pb.h"
#include "meshtastic/mesh.pb.h"
#include <cstdint>
#include <functional>
#include <string>

/**
 * GroupsPlugin - plugin that implements business logic for the group channel panel.
 * UI widgets are referenced by small indices (enum Widget) and placed into the GfxPlugin widget array.
 * updateXYZ(...) methods update changes to UI.
 */
class GroupsPlugin : public GfxPlugin
{
  public:
    // widget slots used by dashboard
    enum class Widget : GfxPlugin::WidgetIndex {
        Group0Label = 0,
        Group0Button,
        Group1Label,
        Group1Button,
        Group2Label,
        Group2Button,
        Group3Label,
        Group3Button,
        Group4Label,
        Group4Button,
        Group5Label,
        Group5Button,
        Group6Label,
        Group6Button,
        Group7Label,
        Group7Button,
        // add more as required but keep below MAX_WIDGETS
        Count
    };

    // action identifiers used by plugins and views
    enum class Action : uint8_t {
        None = 0,
        OpenNode,
        // Plugin-specific/custom actions may use values >= CustomBase
        CustomBase = 0x20
    };

    static constexpr std::size_t WIDGET_COUNT = static_cast<std::size_t>(Widget::Count);

    using Callback = std::function<void(lv_event_t *)>;

    GroupsPlugin();
    virtual ~GroupsPlugin();

    // init override: store resolver/parent and optionally auto-register widgets by name
    void init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount = WIDGET_COUNT, lv_group_t *group = nullptr,
              lv_indev_t *indev = nullptr, GfxPlugin::RegisterWidget registerWidget = GfxPlugin::RegisterWidget::All) override;

    // Set view-level callbacks for plugin operations
    void setOnOpenGroups(const Callback &cb) { onOpenNode = cb; }

    // Register menu widgets with default names
    void registerStandardWidgets(void) override;
    // Register widget event standard action
    void registerStandardEventCallbacks(void) override;
    // Register widget indices to compact Action values in the base class.
    void registerStandardWidgetActions(void) override;

    // Business logic methods: update UI
    virtual void updateName(uint8_t index, const char *groupName);

  protected:
    // helper
    lv_obj_t *getGroupNameWidget(uint8_t index);
    lv_obj_t *getGroupButtonWidget(uint8_t index);

    // handleAction: map compact Action values to plugin callbacks
    void handleAction(Action actionId, WidgetIndex idx, int event_code) /*override*/;

  private:
    // lvgl event handlers
    static void ui_event_button(lv_event_t *e);

    Callback onOpenNode;
};