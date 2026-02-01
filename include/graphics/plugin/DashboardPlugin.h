#pragma once

#include "GfxPlugin.h"
#include "meshtastic/clientonly.pb.h"
#include "meshtastic/connection_status.pb.h"
#include "meshtastic/mesh.pb.h"
#include <cstdint>
#include <functional>
#include <string>

/**
 * DashboardPlugin - concrete dashboard plugin that implements business logic for the home/dashboard panel.
 * Plugin stores the updated dashboard state
 * UI widgets are referenced by small indices (enum Widget) and placed into the GfxPlugin widget array.
 * updateXYZ(...) methods update internal state and push changes to UI.
 */
class DashboardPlugin : public GfxPlugin
{
  public:
    // widget slots used by dashboard
    enum class Widget : GfxPlugin::WidgetIndex {
        MailButton = 0,
        MailLabel,
        NodesButton,
        NodesLabel,
        TimeButton,
        TimeLabel,
        LoRaButton,
        LoRaLabel,
        SignalButton,
        SignalLabel,
        SignalPctLabel,
        BellButton,
        BellLabel,
        LocationButton,
        LocationLabel,
        WlanButton,
        WlanLabel,
        MqttButton,
        MqttLabel,
        SdButton,
        SdLabel,
        MemoryButton,
        MemoryLabel,
        QrButton,
        QrLabel,
        // add more as required but keep below MAX_WIDGETS
        Count
    };

    // action identifiers used by plugins and views
    enum class Action : uint8_t {
        None = 0,
        OpenMessages,
        OpenNodes,
        ToggleTime,
        ToggleLoRa,
        ToggleNotifications,
        ToggleGPS,
        ToggleWLAN,
        ToggleMQTT,
        ToggleMem,
        ToggleQR,
        RefreshSD,
        // Plugin-specific/custom actions may use values >= CustomBase
        CustomBase = 0x20
    };

    static constexpr std::size_t WIDGET_COUNT = static_cast<std::size_t>(Widget::Count);

    using Callback = std::function<void(lv_event_t *)>;

    DashboardPlugin();
    virtual ~DashboardPlugin();

    // init override: store resolver/parent and optionally auto-register widgets by name
    void init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount = WIDGET_COUNT, lv_group_t *group = nullptr,
              lv_indev_t *indev = nullptr, GfxPlugin::RegisterWidget registerWidget = GfxPlugin::RegisterWidget::All) override;

    // load screen (parent)
    virtual void loadScreen(lv_screen_load_anim_t anim = LV_SCR_LOAD_ANIM_MOVE_TOP, uint32_t time = 500);

    // Set view-level callbacks for plugin operations
    void setOnOpenMessages(const Callback &cb) { onOpenMessages = cb; }
    void setOnOpenNodes(const Callback &cb) { onOpenNodes = cb; }
    void setOnToggleTime(const Callback &cb) { onToggleTime = cb; }
    void setOnToggleLoRa(const Callback &cb) { onToggleLoRa = cb; }
    void setOnToggleSound(const Callback &cb) { onToggleSound = cb; }
    void setOnToggleGPS(const Callback &cb) { onToggleGPS = cb; }
    void setOnToggleWLAN(const Callback &cb) { onToggleWLAN = cb; }
    void setOnToggleMQTT(const Callback &cb) { onToggleMQTT = cb; }
    void setOnRefreshSDCard(const Callback &cb) { onRefreshSDCard = cb; }
    void setOnToggleMem(const Callback &cb) { onToggleMem = cb; }
    void setOnToggleQR(const Callback &cb) { onToggleQR = cb; }

    // Register menu widgets with default names
    void registerStandardWidgets(void) override;
    // Register widget event standard action
    void registerStandardEventCallbacks(void) override;
    // Register widget indices to compact Action values in the base class.
    void registerStandardWidgetActions(void) override;

    // Business logic methods: update UI
    void updateTime(uint32_t unixTime, bool use_12h_clock);
    void updateLoRaConfig(const meshtastic_Config_LoRaConfig &cfg);
    void updateSignalStrength(int32_t rssi, float snr);
    void updatePosition(int32_t lat, int32_t lon, int32_t alt, uint32_t sats, uint32_t precision, bool metric);
    void updateSDCard(bool cardDetected, const char *info = nullptr);
    void updateConnectionStatus(const meshtastic_DeviceConnectionStatus &status);
    void updateFreeMem(uint32_t freeHeapBytes, uint32_t lvglFreeBytes);
    void updateNodesStatus(uint32_t online, uint32_t total);

  protected:
    // helper
    int32_t signalStrength2Percent(int32_t rx_rssi, float rx_snr);

    // handleAction: map compact Action values to plugin callbacks
    void handleAction(Action actionId, WidgetIndex idx, int event_code) /*override*/;

  private:
    // lvgl event handlers
    static void ui_event_button(lv_event_t *e);

    Callback onOpenMessages = nullptr;
    Callback onOpenNodes = nullptr;
    Callback onToggleTime = nullptr;
    Callback onToggleLoRa = nullptr;
    Callback onToggleSound = nullptr;
    Callback onToggleGPS = nullptr;
    Callback onToggleWLAN = nullptr;
    Callback onToggleMQTT = nullptr;
    Callback onRefreshSDCard = nullptr;
    Callback onToggleMem = nullptr;
    Callback onToggleQR = nullptr;
};