#pragma once

#include "MeshtasticView.h"
#include "meshtastic/clientonly.pb.h"

/**
 * @brief GUI view for e.g. T-Deck
 * Handles creation of display driver and controller.
 * Note: due to static callbacks in lvgl this class is modelled as
 *       a singleton with static callback members
 */
class TFTView_320x240 : public MeshtasticView
{
  public:
    void init(IClientBase *client) override;
    void setupUIConfig(const meshtastic_DeviceUIConfig& uiconfig) override;
    void task_handler(void) override;

    // methods to update view
    void setMyInfo(uint32_t nodeNum) override;
    void setDeviceMetaData(int hw_model, const char *version, bool has_bluetooth, bool has_wifi, bool has_eth,
                           bool can_shutdown) override;
    void addOrUpdateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                         eRole role, bool hasKey, bool viaMqtt) override;
    void addNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard, eRole role,
                 bool hasKey, bool viaMqtt) override;
    void updateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                    eRole role, bool hasKey, bool viaMqtt) override;
    void updatePosition(uint32_t nodeNum, int32_t lat, int32_t lon, int32_t alt, uint32_t sats, uint32_t precision) override;
    void updateMetrics(uint32_t nodeNum, uint32_t bat_level, float voltage, float chUtil, float airUtil) override;
    void updateEnvironmentMetrics(uint32_t nodeNum, const meshtastic_EnvironmentMetrics &metrics) override;
    void updateAirQualityMetrics(uint32_t nodeNum, const meshtastic_AirQualityMetrics &metrics) override;
    void updatePowerMetrics(uint32_t nodeNum, const meshtastic_PowerMetrics &metrics) override;
    void updateSignalStrength(uint32_t nodeNum, int32_t rssi, float snr) override;
    void updateHopsAway(uint32_t nodeNum, uint8_t hopsAway) override;
    void updateConnectionStatus(const meshtastic_DeviceConnectionStatus &status) override;

    // methods to update device config
    void updateChannelConfig(const meshtastic_Channel &ch) override;
    void updateDeviceConfig(const meshtastic_Config_DeviceConfig &cfg) override;
    void updatePositionConfig(const meshtastic_Config_PositionConfig &cfg) override;
    void updatePowerConfig(const meshtastic_Config_PowerConfig &cfg) override;
    void updateNetworkConfig(const meshtastic_Config_NetworkConfig &cfg) override;
    void updateDisplayConfig(const meshtastic_Config_DisplayConfig &cfg) override;
    void updateLoRaConfig(const meshtastic_Config_LoRaConfig &cfg) override;
    void updateBluetoothConfig(const meshtastic_Config_BluetoothConfig &cfg) override;
    void updateSecurityConfig(const meshtastic_Config_SecurityConfig &cfg) override;
    void updateSessionKeyConfig(const meshtastic_Config_SessionkeyConfig &cfg) override;

    // methods to update module config
    void updateMQTTModule(const meshtastic_ModuleConfig_MQTTConfig &cfg) override;
    void updateSerialModule(const meshtastic_ModuleConfig_SerialConfig &cfg) override {}
    void updateExtNotificationModule(const meshtastic_ModuleConfig_ExternalNotificationConfig &cfg) override;
    void updateStoreForwardModule(const meshtastic_ModuleConfig_StoreForwardConfig &cfg) override {}
    void updateRangeTestModule(const meshtastic_ModuleConfig_RangeTestConfig &cfg) override {}
    void updateTelemetryModule(const meshtastic_ModuleConfig_TelemetryConfig &cfg) override {}
    void updateCannedMessageModule(const meshtastic_ModuleConfig_CannedMessageConfig &) override {}
    void updateAudioModule(const meshtastic_ModuleConfig_AudioConfig &cfg) override {}
    void updateRemoteHardwareModule(const meshtastic_ModuleConfig_RemoteHardwareConfig &cfg) override {}
    void updateNeighborInfoModule(const meshtastic_ModuleConfig_NeighborInfoConfig &cfg) override {}
    void updateAmbientLightingModule(const meshtastic_ModuleConfig_AmbientLightingConfig &cfg) override {}
    void updateDetectionSensorModule(const meshtastic_ModuleConfig_DetectionSensorConfig &cfg) override {}
    void updatePaxCounterModule(const meshtastic_ModuleConfig_PaxcounterConfig &cfg) override {}
    void updateFileinfo(const meshtastic_FileInfo &fileinfo) override {}
    void updateRingtone(const char rtttl[231]) override;

    // update internal time
    void updateTime(uint32_t time) override;

    void packetReceived(const meshtastic_MeshPacket &p) override;
    void handleResponse(uint32_t from, uint32_t id, const meshtastic_Routing &routing, const meshtastic_MeshPacket &p) override;
    void handleResponse(uint32_t from, uint32_t id, const meshtastic_RouteDiscovery &route) override;
    void handlePositionResponse(uint32_t from, uint32_t request_id, int32_t rx_rssi, float rx_snr, bool isNeighbor) override;
    void notifyResync(bool show) override;
    void notifyReboot(bool show) override;
    void notifyShutdown(void) override;
    void blankScreen(bool enable) override;
    void screenSaving(bool enabled) override;
    bool isScreenLocked(void) override;
    void newMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg) override;
    void removeNode(uint32_t nodeNum) override;

    enum BasicSettings {
        eNone,
        eUsername,
        eDeviceRole,
        eRegion,
        eModemPreset,
        eChannel,
        eWifi,
        eLanguage,
        eScreenTimeout,
        eScreenLock,
        eScreenBrightness,
        eTheme,
        eInputControl,
        eAlertBuzzer,
        eReset,
        eReboot,
        eModifyChannel
    };

  protected:
    struct NodeFilter {
        bool unknown;  // filter out unknown nodes
        bool mqtt;     // filter out via mqtt nodes
        bool offline;  // filter out offline nodes (>15min lastheard)
        bool position; // filter out nodes without position
        char *name;    // filter by name
        bool active;   // flag for active filter
    };

    struct NodeHighlight {
        bool chat;      // highlight nodes with active chats
        bool position;  // highlight nodes with position
        bool telemetry; // highlight nodes with telemetry
        bool iaq;       // highlight nodes with IAQ
        char *name;     // hightlight by name
        bool active;    // flag for active highlight;
    };

    typedef void (*UserWidgetFunc)(lv_obj_t *, void *, int);

    // initialize all ui screens
    virtual void init_screens(void);
    // update custom display string on boot screen 
    virtual void updateBootMessage(void);
    // update node counter display (online and filtered)
    virtual void updateNodesStatus(void);
    // display message popup
    virtual void showMessagePopup(uint32_t from, uint32_t to, uint8_t ch, const char *name);
    // hide new message popup
    virtual void hideMessagePopup(void);
    // display user widget (dynamically created)
    void showUserWidget(UserWidgetFunc createWidget);
    // display messages of a group channel
    virtual void addChat(uint32_t from, uint32_t to, uint8_t ch);
    // mark chat border to indicate a new message
    virtual void highlightChat(uint32_t from, uint32_t to, uint8_t ch);
    // display number of active chats
    virtual void updateActiveChats(void);
    // display new message popup
    virtual void showMessages(uint8_t channel);
    // display messages of a node
    virtual void showMessages(uint32_t nodeNum);
    // own chat message
    virtual void handleAddMessage(char *msg);
    // add own message to current chat
    virtual void addMessage(uint32_t requestId, char *msg);
    // add new message to container
    virtual void newMessage(uint32_t nodeNum, lv_obj_t *container, uint8_t channel, const char *msg);
    // create empty message container for node or group channel
    virtual lv_obj_t *newMessageContainer(uint32_t from, uint32_t to, uint8_t ch);
    // filter or highlight node
    virtual bool applyNodesFilter(uint32_t nodeNum, bool reset = false);
    // display message alert popup
    virtual void messageAlert(const char *alert, bool show);
    // mark sent message as received
    virtual void handleTextMessageResponse(uint32_t channelOrNode, uint32_t id, bool ack);
    // set node image based on role
    virtual void setNodeImage(uint32_t nodeNum, eRole role, bool viaMqtt, lv_obj_t *img);
    // apply filter and count number of filtered nodes
    virtual void updateNodesFiltered(bool reset);
    // set last heard to now, update nodes online
    virtual void updateLastHeard(uint32_t nodeNum);
    // update last heard value on all node panels
    virtual void updateAllLastHeard(void);
    // update image and unread messages on home screen
    virtual void updateUnreadMessages(void);
    // update time display on home screen
    virtual void updateTime(void);
    // update free memory display on home screen
    virtual void updateFreeMem(void);
    // update distance to other node
    virtual void updateDistance(uint32_t nodeNum, int32_t lat, int32_t lon);

    NodeFilter filter;
    NodeHighlight highlight;

  private:
    // view creation only via ViewFactory
    friend class ViewFactory;
    static TFTView_320x240 *instance(void);
    static TFTView_320x240 *instance(const DisplayDriverConfig &cfg);
    TFTView_320x240();
    TFTView_320x240(const DisplayDriverConfig *cfg, DisplayDriver *driver);

    void apply_hotfix(void);
    void updateTheme(void);
    void ui_events_init(void);
    void ui_set_active(lv_obj_t *b, lv_obj_t *p, lv_obj_t *tp);
    void showKeyboard(lv_obj_t *textArea);

    void enablePanel(lv_obj_t *panel);
    void disablePanel(lv_obj_t *panel);
    void setGroupFocus(lv_obj_t *panel);
    void setInputGroup(void);
    void setInputButtonLabel(void);

    void scanSignal(uint32_t scanNo);
    void handleTraceRouteResponse(const meshtastic_Routing &routing);
    void addNodeToTraceRoute(uint32_t nodeNum, lv_obj_t *panel);
    void removeSpinner(void);
    void packetDetected(const meshtastic_MeshPacket &p);
    void writePacketLog(const meshtastic_MeshPacket &p);
    void updateStatistics(const meshtastic_MeshPacket &p);

    uint32_t language2val(meshtastic_Language lang);
    meshtastic_Language val2language(uint32_t val);
    void setLocale(meshtastic_Language lang);
    void setLanguage(meshtastic_Language lang);
    void setTimeout(uint32_t timeout);
    void setBrightness(uint32_t brightness);
    void setTheme(uint32_t theme);
    void storeNodeOptions(void);
    void showLoRaFrequency(const meshtastic_Config_LoRaConfig &cfg);
    void setBellText(bool banner, bool sound);

    // lvgl event callbacks
    // static void ui_event_HomeButton(lv_event_t * e);
    static void ui_event_NodesButton(lv_event_t *e);
    static void ui_event_GroupsButton(lv_event_t *e);
    static void ui_event_MessagesButton(lv_event_t *e);
    static void ui_event_MapButton(lv_event_t *e);
    static void ui_event_SettingsButton(lv_event_t *e);

    static void ui_event_NodeButton(lv_event_t *e);
    static void ui_event_ChannelButton(lv_event_t *e);
    static void ui_event_ChatButton(lv_event_t *e);
    static void ui_event_ChatDelButton(lv_event_t *e);
    static void ui_event_MsgPopupButton(lv_event_t *e);

    // Home screen
    static void ui_event_EnvelopeButton(lv_event_t *e);
    static void ui_event_OnlineNodesButton(lv_event_t *e);
    static void ui_event_TimeButton(lv_event_t *e);
    static void ui_event_LoRaButton(lv_event_t *e);
    static void ui_event_BellButton(lv_event_t *e);
    static void ui_event_LocationButton(lv_event_t *e);
    static void ui_event_WLANButton(lv_event_t *e);
    static void ui_event_MQTTButton(lv_event_t *e);
    static void ui_event_MemoryButton(lv_event_t *e);

    static void ui_event_KeyboardButton(lv_event_t *e);
    static void ui_event_Keyboard(lv_event_t *e);

    static void ui_event_message_ready(lv_event_t *e);

    static void ui_event_user_button(lv_event_t *e);
    static void ui_event_role_button(lv_event_t *e);
    static void ui_event_region_button(lv_event_t *e);
    static void ui_event_preset_button(lv_event_t *e);
    static void ui_event_wifi_button(lv_event_t *e);
    static void ui_event_language_button(lv_event_t *e);
    static void ui_event_channel_button(lv_event_t *e);
    static void ui_event_brightness_button(lv_event_t *e);
    static void ui_event_theme_button(lv_event_t *e);
    static void ui_event_calibration_button(lv_event_t *e);
    static void ui_event_timeout_button(lv_event_t *e);
    static void ui_event_screen_lock_button(lv_event_t *e);
    static void ui_event_input_button(lv_event_t *e);
    static void ui_event_alert_button(lv_event_t *e);
    static void ui_event_reset_button(lv_event_t *e);
    static void ui_event_reboot_button(lv_event_t *e);
    static void ui_event_device_reboot_button(lv_event_t *e);
    static void ui_event_device_shutdown_button(lv_event_t *e);
    static void ui_event_device_cancel_button(lv_event_t *e);
    static void ui_event_shutdown_button(lv_event_t *e);
    static void ui_event_modify_channel(lv_event_t *e);
    static void ui_event_delete_channel(lv_event_t *e);

    static void ui_event_screen_timeout_slider(lv_event_t *e);
    static void ui_event_brightness_slider(lv_event_t *e);
    static void ui_event_frequency_slot_slider(lv_event_t *e);
    static void ui_event_modem_preset_dropdown(lv_event_t *e);
    static void ui_event_device_role_dropdown(lv_event_t *e);

    static void ui_event_calibration_screen_loaded(lv_event_t *e);

    static void ui_event_mesh_detector(lv_event_t *e);
    static void ui_event_mesh_detector_start(lv_event_t *e);
    static void ui_event_signal_scanner(lv_event_t *e);
    static void ui_event_signal_scanner_node(lv_event_t *e);
    static void ui_event_signal_scanner_start(lv_event_t *e);
    static void ui_event_trace_route(lv_event_t *e);
    static void ui_event_trace_route_to(lv_event_t *e);
    static void ui_event_trace_route_start(lv_event_t *e);
    static void ui_event_trace_route_node(lv_event_t *e);
    static void ui_event_neighbors(lv_event_t *e);
    static void ui_event_statistics(lv_event_t *e);
    static void ui_event_packet_log(lv_event_t *e);

    static void ui_event_pin_screen_button(lv_event_t *e);
    static void ui_event_statistics_table(lv_event_t *e);

    static void ui_event_ok(lv_event_t *e);
    static void ui_event_cancel(lv_event_t *e);

    // animations
    static void ui_anim_node_panel_cb(void *var, int32_t v);
    static void ui_anim_radar_cb(void *var, int32_t r);

    lv_obj_t *activeButton = nullptr;
    lv_obj_t *activePanel = nullptr;
    lv_obj_t *activeTopPanel = nullptr;
    lv_obj_t *activeMsgContainer = nullptr;
    lv_obj_t *activeWidget = nullptr;
    lv_obj_t *activeTextInput = nullptr;
    lv_group_t *input_group = nullptr;

    enum BasicSettings activeSettings = eNone; // active settings menu (used to disable other button presses)

    static TFTView_320x240 *gui;                     // singleton pattern
    bool screensInitialised;                         // true if init_screens is completed
    uint32_t nodesFiltered;                          // no. hidden nodes in node list
    bool processingFilter;                           // indicates that filtering is ongoing
    bool packetLogEnabled;                           // display received packets
    bool detectorRunning;                            // meshDetector is active
    uint16_t statisticTableRows;                     // number of rows in statistics table
    uint16_t packetCounter;                          // number of packets in packet log
    time_t lastrun60, lastrun10, lastrun5, lastrun1; // timers for task loop
    time_t actTime, uptime;                          // actual time and uptime;
    bool hasPosition;                                // if our position is known
    int32_t myLatitude, myLongitude;                 // our current position as reported by firmware
    void *topNodeLL;                                 // pointer to topmost button in group ll
    uint32_t scans;                                  // scanner counter
    lv_anim_t radar;                                 // radar animation
    static uint32_t currentNode;                     // current selected node
    static lv_obj_t *currentPanel;                   // current selected node panel
    static lv_obj_t *spinnerButton;                  // start button animation
    static time_t startTime;                         // time when start button was pressed
    static uint32_t pinKeys;                         // number of keys pressed (lock screen)
    static bool screenLocked;                        // screen lock active
    uint32_t selectedHops;                           // remember selected choice
    bool chooseNodeSignalScanner;                    // chose a target node for signal scanner
    bool chooseNodeTraceRoute;                       // chose a target node for trace route
    char old_val1_scratch[64], old_val2_scratch[64]; // temporary scratch buffers for settings strings
    std::array<lv_obj_t *, c_max_channels> ch_label; // indexable label list for settings
    meshtastic_Channel *channel_scratch;             // temporary scratch copy of channel db

    // extended default device profile struct with additional required data
    struct meshtastic_DeviceProfile_ext : meshtastic_DeviceProfile {
        meshtastic_Channel channel[c_max_channels]; // storage of channel info
        meshtastic_DeviceUIConfig uiConfig;         // storage of persistent UI data
    };

    // additional local ui data
    struct meshtastic_DeviceProfile_full : meshtastic_DeviceProfile_ext {
        uint16_t ringtoneId;         // index into ringtone preset
        bool silent;                 // sound silenced
    };

    meshtastic_DeviceProfile_full db; // full copy of the node's configuration db (except nodeinfos) plus ui data
};