#pragma once

#include "GfxPlugin.h"
#include "graphics/common/SdCard.h"
#include "graphics/map/MapPanel.h"
#include "meshtastic/clientonly.pb.h"
#include <memory>
#include <unordered_map>
#include <vector>

// Owns map navigation and source selection; the view supplies device state and
// navigation callbacks without exposing its controller or generated settings UI.
class MapPlugin : public GfxPlugin
{
  public:
    enum class Widget : WidgetIndex { Canvas, Title, Back, Count };
    static constexpr std::size_t WIDGET_COUNT = static_cast<std::size_t>(Widget::Count);

    MapPlugin();
    ~MapPlugin() override;
    void init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount = WIDGET_COUNT, lv_group_t *group = nullptr,
              lv_indev_t *indev = nullptr, RegisterWidget registerWidget = RegisterWidget::All) override;
    void registerStandardWidgets() override;
    void setConfig(meshtastic_DeviceUIConfig *config, std::function<void()> save);
    void setOwnNode(uint32_t id);
    void updateStorage(ISdCard *card, bool mounted);
    void updateNetwork(bool enabled, bool connected, bool known = true);
    void setOnBack(std::function<void()> cb) { onBack = std::move(cb); }
    void setOnOpenWifi(std::function<void()> cb) { onOpenWifi = std::move(cb); }
    void setOnOpenNode(std::function<void(uint32_t)> cb) { onOpenNode = std::move(cb); }
    void updateNode(uint32_t id, int32_t latitude, int32_t longitude, const char *shortName);
    void removeNode(uint32_t id);
    void setGpsPosition(int32_t latitude, int32_t longitude);
    void showPanel() override;
    void onShow() override;
    void onHide() override;
    void task_handler(time_t millis) override;
    MapPanel *getMap() const { return map.get(); }

  private:
    struct Node {
        int32_t latitude = 0;
        int32_t longitude = 0;
        std::string name;
        lv_obj_t *marker = nullptr;
    };
    enum class NoticeAction { None, Source, Wifi };
    void createControls();
    void createMap();
    void addNode(uint32_t id, Node &node);
    void discoverSources();
    void selectSource(std::size_t index, bool persist);
    void updateSourceControls();
    void showSources();
    void closeSources();
    void applyUrl();
    void updateStatus();
    void updateZoom();
    void focusZoom();
    void saveHome();
    void setFollowGps(bool follow, bool persist);
    lv_obj_t *button(lv_obj_t *container, const char *text);
    void handleMapEvent(lv_event_t *event);
    static void event(lv_event_t *event);

    std::unique_ptr<MapPanel> map;
    meshtastic_DeviceUIConfig *config = nullptr;
    ISdCard *card = nullptr;
    std::function<void()> saveConfig, onBack, onOpenWifi;
    std::function<void(uint32_t)> onOpenNode;
    std::unordered_map<uint32_t, Node> nodes;
    std::vector<std::string> sources;
    std::string statusText;
    uint32_t ownNode = 0;
    int32_t latitude = 0, longitude = 0;
    bool hasGps = false, mounted = false, storageDirty = true, visible = false;
    bool wifiEnabled = false, wifiConnected = false, networkKnown = false;
    bool hasSdSource = false, following = false, deferZoomFocus = false;
    bool sourcesOpen = false;
    NoticeAction noticeAction = NoticeAction::None;
    uint32_t lastStatus = 0;
    lv_obj_t *canvas = nullptr, *toolbar = nullptr, *zoom = nullptr, *zoomLabel = nullptr;
    lv_obj_t *panLeft = nullptr, *panRight = nullptr, *panUp = nullptr, *panDown = nullptr;
    lv_obj_t *home = nullptr, *follow = nullptr, *sourceButton = nullptr;
    lv_obj_t *homeMarker = nullptr, *gpsMarker = nullptr;
    lv_obj_t *notice = nullptr, *noticeLabel = nullptr, *attribution = nullptr;
    lv_obj_t *sourcePanel = nullptr, *sourceDropdown = nullptr, *urlInput = nullptr;
    lv_obj_t *sourceMessage = nullptr, *applyButton = nullptr, *closeButton = nullptr;
};
