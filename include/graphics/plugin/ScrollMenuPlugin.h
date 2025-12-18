#pragma once

#include "GfxPlugin.h"
#include <cstdint>
#include <functional>
#include <string>

struct _lv_event_t;
using lv_event_t = _lv_event_t;

class ScrollMenuPlugin : public GfxPlugin
{
  public:
    // widget slots used by menu
    enum class Widget : GfxPlugin::WidgetIndex {
        MenuLabel = 0,
        HomeButton,
        NodesButton,
        GroupsButton,
        MessagesButton,
        MapButton,
        ClockButton,
        MusicButton,
        StatisticsButton,
        ToolsButton,
        SettingsButton,
        PowerButton,
        // add more as required but keep below MAX_WIDGETS
        Count
    };

    enum class Action : uint8_t {
        None = 0,
        OpenHome,
        OpenNodes,
        OpenGroups,
        OpenMessages,
        OpenMap,
        Power,
        // Plugin-specific/custom actions may use values >= CustomBase
        CustomBase = 0x20
    };

    static constexpr std::size_t WIDGET_COUNT = static_cast<std::size_t>(Widget::Count);

    using Callback = struct {
        const char *name;
        std::function<void(lv_event_t *)> cb;
    };

    ScrollMenuPlugin();
    virtual ~ScrollMenuPlugin();

    // init override: store resolver/parent and optionally auto-register widgets by name
    void init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount = WIDGET_COUNT, lv_group_t *group = nullptr,
              lv_indev_t *indev = nullptr, GfxPlugin::RegisterWidget registerWidget = GfxPlugin::RegisterWidget::All) override;

    // load screen (parent)
    virtual void loadScreen(lv_screen_load_anim_t anim = LV_SCR_LOAD_ANIM_MOVE_TOP, uint32_t time = 500);

    // Set view-level callbacks for well-known operations; first parameter is display label when focused
    void setOnOpenHome(const char *menu, const std::function<void(lv_event_t *)> &cb) { onOpenHome = {menu, cb}; }
    void setOnOpenNodes(const char *menu, const std::function<void(lv_event_t *)> &cb) { onOpenNodes = {menu, cb}; }
    void setOnOpenGroups(const char *menu, const std::function<void(lv_event_t *)> &cb) { onOpenGroups = {menu, cb}; }
    void setOnOpenMessages(const char *menu, const std::function<void(lv_event_t *)> &cb) { onOpenMessages = {menu, cb}; }
    void setOnOpenMap(const char *menu, const std::function<void(lv_event_t *)> &cb) { onOpenMap = {menu, cb}; }
    void setOnOpenClock(const char *menu, const std::function<void(lv_event_t *)> &cb) { onOpenClock = {menu, cb}; }
    void setOnOpenMusic(const char *menu, const std::function<void(lv_event_t *)> &cb) { onOpenMusic = {menu, cb}; }
    void setOnOpenStatistics(const char *menu, const std::function<void(lv_event_t *)> &cb) { onOpenStatistics = {menu, cb}; }
    void setOnOpenTools(const char *menu, const std::function<void(lv_event_t *)> &cb) { onOpenTools = {menu, cb}; }
    void setOnOpenSettings(const char *menu, const std::function<void(lv_event_t *)> &cb) { onOpenSettings = {menu, cb}; }
    void setOnPower(const char *menu, const std::function<void(lv_event_t *)> &cb) { onPower = {menu, cb}; }

    // Register menu widgets with default names
    void registerStandardWidgets(void) override;
    // Register widget event standard action
    void registerStandardEventCallbacks(void) override;
    // Register widget indices to compact Action values in the base class.
    void registerStandardWidgetActions(void) override;

  protected:
    // handleAction: map compact Action values to plugin callbacks
    void handleAction(Action actionId, WidgetIndex idx, int event_code) /*override*/;

  private:
    // lvgl event handlers
    static void ui_event_MenuButton(lv_event_t *e);

    Callback onOpenHome;
    Callback onOpenNodes;
    Callback onOpenGroups;
    Callback onOpenMessages;
    Callback onOpenMap;
    Callback onOpenClock;
    Callback onOpenMusic;
    Callback onOpenStatistics;
    Callback onOpenTools;
    Callback onOpenSettings;
    Callback onPower;
};
