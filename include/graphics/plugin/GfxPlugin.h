#pragma once

#include "Plugins.h"
#include "lvgl.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

#ifdef ARCH_PORTDUINO
#define EVENT_CLICKED LV_EVENT_CLICKED
#else
#define EVENT_CLICKED LV_EVENT_PRESSED
#endif

#if 0
// forward-declare LVGL types
struct _lv_obj_t;
struct _lv_group_t;
struct _lv_indev_t;
using lv_obj_t = _lv_obj_t;
using lv_group_t = _lv_group_t;
using lv_indev_t = _lv_indev_t;
#endif

/*
 GfxPlugin - minimal, display-agnostic plugin base.
 - Generated UIs expose global named LVGL widgets. Plugins operate on those widgets.
 - During init the view can use widget setter function to populate the widget array.
*/
class GfxPlugin
{
  public:
    // compact action identifiers used by plugins and views
    enum class RegisterWidget : uint8_t {
        Nothing = 0,
        Standard = 1,
        Events = 2,
        Actions = 4,
        All = 255,
    };

    // compact action identifiers used by plugins and views
    enum class Action : uint8_t {
        None = 0,
        OpenMap,
        OpenNodes,
        OpenMessages,
        Power,
        // Plugin-specific/custom actions may use values >= CustomBase
        CustomBase = 0x20
    };

    // view decides (touch if touchscreen, encoder otherwise)
    enum class InputMode : uint8_t { Auto = 0, Touch, Encoder, Buttons };

    using WidgetIndex = uint8_t;
    static constexpr std::size_t MAX_WIDGETS = 32; // adjust if you need more widgets

    // resolver: return the LVGL object pointer for a generated global widget name (or nullptr)
    using WidgetResolver = std::function<lv_obj_t *(const char *name)>;

    // action callback signature using Action enum
    using ActionCb = std::function<void(Action action, void *data)>;

    // generic lv event callback (forwarding hook)
    using LvEventCallback = std::function<void(lv_obj_t *obj, int event_code)>;

    GfxPlugin(const std::string &name);
    virtual ~GfxPlugin();

    // Initialize plugin: provide parent container, resolver and optional groups/indev.
    // widgetCount tells the plugin how many indices are meaningful (<= MAX_WIDGETS).
    virtual void init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount = 0, lv_group_t *group = nullptr,
                      lv_indev_t *indev = nullptr, GfxPlugin::RegisterWidget registerWidget = GfxPlugin::RegisterWidget::All);

    // Tear down plugin state
    virtual void deinit();

    // show screen (only use in case parent is screen widget)
    virtual void loadScreen(lv_screen_load_anim_t anim = LV_SCR_LOAD_ANIM_MOVE_TOP, uint32_t time = 500);

    // show panel (only use in case parent is panel widget)
    virtual void showPanel(void) {}

    // periodic task handler called by view; millis is ms since boot (view supplies)
    virtual void task_handler(uint32_t millis) { (void)millis; }

    virtual void onShow() {}
    virtual void onHide() {}
    virtual void setInputMode(InputMode m)
    {
        inputMode = m;
        (void)m;
    }

    // Register menu widgets with default names
    virtual void registerStandardWidgets(void) {}
    // Register widget event standard action
    virtual void registerStandardEventCallbacks(void) {}
    // Register widget indices to compact Action values in the base class.
    virtual void registerStandardWidgetActions(void) {}

    // set action callback
    void setActionCallback(ActionCb cb) { actionCallback = std::move(cb); }

    // set LV event forwarding hook
    void setLvEventCallback(LvEventCallback cb) { lvEventCb = std::move(cb); }

    // direct widget access by index
    lv_obj_t *getWidget(WidgetIndex idx) const
    {
        if (idx >= widgetCount)
            return nullptr;
        return widgets[idx];
    }

    // register an lv_obj_t* for a given index (called by view during initialisation or via resolver)
    void setWidget(WidgetIndex idx, lv_obj_t *obj, bool onEvent = false);

    // helper: use resolver to populate one widget entry
    bool registerWidgetByName(WidgetIndex idx, const char *name)
    {
        if (!resolver || idx >= widgetCount || !name)
            return false;
        lv_obj_t *w = resolver(name);
        if (!w)
            return false;
        widgets[idx] = w;
        return true;
    }

    // register widget -> action mapping using compact Action enum
    void registerWidgetAction(WidgetIndex idx, Action action)
    {
        if (idx >= widgetCount)
            return;
        widgetActions[idx] = action;
    }

    // entrypoint for LVGL-generated callbacks: view or generated callbacks should call this
    virtual void onEvent(lv_obj_t *obj, int event_code)
    {
        // forward raw event if requested
        if (lvEventCb)
            lvEventCb(obj, event_code);
        // TODO handleAction(widgetActions[idx], idx, event_code);
    }

  protected:
    // override to handle compact Action for a widget index
    virtual void handleAction(Action actionId, WidgetIndex idx, int event_code)
    {
        (void)idx;
        (void)event_code;
        if (actionCallback)
            actionCallback(actionId, nullptr);
    }

    // fallback event handling for objects not registered in array
    virtual void handleEvent(lv_obj_t *obj, int event_code)
    {
        (void)obj;
        (void)event_code;
    }

    // helper to invoke action callback directly
    void invokeActionCallback(Action action, void *data = nullptr)
    {
        if (actionCallback)
            actionCallback(action, data);
    }

    lv_obj_t *parent = nullptr;
    lv_group_t *group = nullptr;
    lv_indev_t *indev = nullptr;
    InputMode inputMode = InputMode::Auto;

    std::size_t widgetCount = 0;
    std::array<lv_obj_t *, MAX_WIDGETS> widgets{};
    std::array<Action, MAX_WIDGETS> widgetActions{};
    WidgetResolver resolver;

    ActionCb actionCallback;
    LvEventCallback lvEventCb;
};