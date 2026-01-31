#pragma once

#include "graphics/common/MeshtasticView.h"
#include "meshtastic/clientonly.pb.h"

class ScrollMenuPlugin;
class DashboardPlugin;
class NodesPlugin;
class GroupsPlugin;
class ClockPlugin;

// use to facilitate indev groups creation before each screen is initialised
#define SETUP_INDEV(GROUP)                                                                                                       \
    GROUP = lv_group_create();                                                                                                   \
    lv_group_set_default(GROUP);                                                                                                 \
    lv_indev_set_group(THIS->indev, GROUP);

class MapPanel;

/**
 * @brief GUI base view for pluggable components. Can be customized by derived classes/views.
 *        Note: due to static callbacks in lvgl this class is modelled as a
 *        singleton with static callback members
 */
class PluggableView : public MeshtasticView
{
  public:
    void init(IClientBase *client) override;
    bool setupUIConfig(const meshtastic_DeviceUIConfig &uiconfig) override;
    void task_handler(void) override;

    void setMyInfo(uint32_t nodeNum) override;
    void addOrUpdateNode(uint32_t nodeNum, uint8_t channel, const meshtastic_NodeInfo &node, const meshtastic_User &cfg) override;
    void addNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard, eRole role,
                 bool hasKey, bool isFav, bool isIgnored, bool unmessagable) override;
    void updateNode(uint32_t nodeNum, uint8_t channel, const meshtastic_User &cfg) override;
    void updatePosition(uint32_t nodeNum, int32_t lat, int32_t lon, int32_t alt, uint32_t sats, uint32_t precision) override;
    void updateSignalStrength(uint32_t nodeNum, int32_t rssi, float snr) override;
    void updateHopsAway(uint32_t nodeNum, uint8_t hopsAway) override {}
    void updateConnectionStatus(const meshtastic_DeviceConnectionStatus &status) override;

    // methods to update device config
    void updateChannelConfig(const meshtastic_Channel &ch) override {}
    void updateDeviceConfig(const meshtastic_Config_DeviceConfig &cfg) override {}
    void updatePositionConfig(const meshtastic_Config_PositionConfig &cfg) override;
    void updatePowerConfig(const meshtastic_Config_PowerConfig &cfg) override {}
    void updateNetworkConfig(const meshtastic_Config_NetworkConfig &cfg) override {}
    void updateDisplayConfig(const meshtastic_Config_DisplayConfig &cfg) override;
    void updateLoRaConfig(const meshtastic_Config_LoRaConfig &cfg) override;
    void updateBluetoothConfig(const meshtastic_Config_BluetoothConfig &cfg, uint32_t id = 0) override {}
    void updateSecurityConfig(const meshtastic_Config_SecurityConfig &cfg) override {}
    void updateSessionKeyConfig(const meshtastic_Config_SessionkeyConfig &cfg) override {}

    // update internal time
    void updateTime(uint32_t time) override;

    void packetReceived(const meshtastic_MeshPacket &p) override;
    void notifyConnected(const char *info) override;
    void notifyDisconnected(const char *info) override;

  protected:
    PluggableView(const DisplayDriverConfig *cfg, DisplayDriver *driver);
    static PluggableView *instance(void);

    // initialize all ui screens
    virtual void ui_init(void);
    virtual void init_screens(void);
    virtual void addMessage(char *msg) {}
    virtual void newMessage(uint32_t nodeNum, lv_obj_t *container, uint8_t channel, const char *msg) {}

    // update time display on screen
    virtual void updateTime(void);
    // update node counter display
    virtual void updateNodesStatus(void);
    // update SD card slot info
    virtual bool updateSDCard(void);
    // update time display on home screen
    virtual void updateFreeMem(void);
    // show map and load tiles
    virtual void loadMap(void);
    // add objects on map
    virtual void addOrUpdateMap(uint32_t nodeNum, int32_t lat, int32_t lon);
    // remove objects from map
    virtual void removeFromMap(uint32_t nodeNum);

    virtual void ui_events_init(void);

    // lvgl event callbacks
    static void ui_event_TopBackButton(lv_event_t *e);
    static void ui_event_HomeButton(lv_event_t *e);
    static void ui_event_NodesButton(lv_event_t *e);
    static void ui_event_GroupsButton(lv_event_t *e);
    static void ui_event_MessagesButton(lv_event_t *e);
    static void ui_event_MapButton(lv_event_t *e);
    static void ui_event_ClockButton(lv_event_t *e);
    static void ui_event_MusicButton(lv_event_t *e);
    static void ui_event_StatisticsButton(lv_event_t *e);
    static void ui_event_ToolsButton(lv_event_t *e);
    static void ui_event_SettingsButton(lv_event_t *e);
    static void ui_event_PowerButton(lv_event_t *e);

    // lvgl timer callbacks
    static void timer_event_bootscreen_done(lv_timer_t *timer);

    // keyboard
    // static void ui_event_KeyboardButton(lv_event_t *e);
    static void ui_event_Keyboard(lv_event_t *e);
    static void ui_event_message_ready(lv_event_t *e);

    void setGroupFocus(lv_obj_t *panel);
    void setInputGroup(void);

    void showLoRaFrequency(const meshtastic_Config_LoRaConfig &cfg);
    void updateSignalStrength(int32_t rssi, float snr);
    int32_t signalStrength2Percent(int32_t rx_rssi, float rx_snr);
    void setBellText(bool banner, bool sound);
    void updateLocationMap(uint32_t objects);

    // singleton
    static PluggableView *gui;

    // default plugins for all derived views
    ScrollMenuPlugin *menu = nullptr;
    DashboardPlugin *dashboard = nullptr;
    NodesPlugin *node = nullptr;
    GroupsPlugin *groups = nullptr;
    ClockPlugin *clock = nullptr;

    // input device and groups for (encoder/button) navigation
    lv_indev_t *indev = nullptr;
    lv_group_t *menuGroup = nullptr;
    lv_group_t *homeGroup = nullptr;
    lv_group_t *nodesGroup = nullptr;
    lv_group_t *groupsGroup = nullptr;
    lv_group_t *chatsGroup = nullptr;
    lv_group_t *mapGroup = nullptr;
    lv_group_t *clockGroup = nullptr;
    lv_group_t *musicGroup = nullptr;
    lv_group_t *statisticsGroup = nullptr;
    lv_group_t *toolsGroup = nullptr;
    lv_group_t *settingsGroup = nullptr;
    lv_group_t *powerGroup = nullptr;

    time_t lastrun60, lastrun10, lastrun5, lastrun1; // timers for task loop
    time_t actTime, uptime, lastHeard;               // actual time and uptime; time last heard a node
    bool hasPosition;                                // if our position is known
    bool screensInitialised;                         // true if init_screens is completed
    int32_t myLatitude, myLongitude;                 // our current position as reported by firmware

    struct meshtastic_DeviceProfile_ext : meshtastic_DeviceProfile {
        meshtastic_User user;
        meshtastic_Channel channel[c_max_channels]; // storage of channel info
        meshtastic_DeviceUIConfig uiConfig;         // storage of persistent UI data
    };

    // additional local ui data (non-persistent)
    struct meshtastic_DeviceProfile_full : meshtastic_DeviceProfile_ext {
        bool silent;                                        // sound silenced
        meshtastic_DeviceConnectionStatus connectionStatus; // wifi/bluetooth/ethernet
    };

    meshtastic_DeviceProfile_full db{}; // full copy of the node's configuration db (except nodeinfos) plus ui data

    bool formatSD;                                        // offer to format SD card
    MapPanel *map = nullptr;                              // map
    std::unordered_map<uint32_t, lv_obj_t *> nodeObjects; // nodeObjects displayed on map
};