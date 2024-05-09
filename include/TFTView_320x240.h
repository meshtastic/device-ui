#pragma once

#include "DisplayDriverConfig.h"
#include "MeshtasticView.h"

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
    void task_handler(void) override;

    // methods to update view
    void setMyInfo(uint32_t nodeNum) override;
    void setDeviceMetaData(int hw_model, const char *version, bool has_bluetooth, bool has_wifi, bool has_eth,
                           bool can_shutdown) override;
    void addOrUpdateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                         eRole role) override;
    void addNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                 eRole role) override;
    void updateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                    eRole role) override;
    void updatePosition(uint32_t nodeNum, int32_t lat, int32_t lon, int32_t alt, uint32_t sats, uint32_t precision) override;
    void updateMetrics(uint32_t nodeNum, uint32_t bat_level, float voltage, float chUtil, float airUtil) override;
    void updateSignalStrength(uint32_t nodeNum, int32_t rssi, float snr) override;
    void updateConnectionStatus(const meshtastic_DeviceConnectionStatus &status) override;

    // methods to update device config
    void updateChannelConfig(uint32_t index, const char *name, const uint8_t *psk, uint32_t psk_size, uint8_t role) override;
    void updateDeviceConfig(const meshtastic_Config_DeviceConfig &cfg) override {}
    void updatePositionConfig(const meshtastic_Config_PositionConfig &cfg) override {}
    void updatePowerConfig(const meshtastic_Config_PowerConfig &cfg) override {}
    void updateNetworkConfig(const meshtastic_Config_NetworkConfig &cfg) override {}
    void updateDisplayConfig(const meshtastic_Config_DisplayConfig &cfg) override {}
    void updateLoRaConfig(const meshtastic_Config_LoRaConfig &cfg) override {}
    void updateBluetoothConfig(const meshtastic_Config_BluetoothConfig &cfg) override {}

    // methods to update module config
    void updateMQTTModule(const meshtastic_ModuleConfig_MQTTConfig &cfg) override {}
    void updateSerialModule(const meshtastic_ModuleConfig_SerialConfig &cfg) override {}
    void updateExtNotificationModule(const meshtastic_ModuleConfig_ExternalNotificationConfig &cfg) override {}
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

    void packetReceived(const meshtastic_MeshPacket &p) override;
    void notifyResync(bool show) override;
    void newMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg) override;
    void updateNodesOnline(const char *str) override;
    void removeNode(uint32_t nodeNum) override;

  protected:
    virtual void showMessagePopup(uint32_t from, uint32_t to, uint8_t ch, const char *name);
    // hide new message popup
    virtual void hideMessagePopup(void);
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
    virtual void addMessage(char *msg);
    // add new message to container
    virtual void newMessage(uint32_t nodeNum, lv_obj_t *container, uint8_t channel, const char *msg);
    // create empty message container for node or group channel
    virtual lv_obj_t *newMessageContainer(uint32_t from, uint32_t to, uint8_t ch);
    // set node image based on role
    virtual void setNodeImage(uint32_t nodeNum, eRole role, lv_obj_t *img);
    // set last heard to now, update nodes online
    virtual void updateLastHeard(uint32_t nodeNum);
    // update last heard value on all node panels
    virtual void updateAllLastHeard(void);
    // update image and unread messages on home screen
    virtual void updateUnreadMessages(void);
    // update free memory display on home screen
    virtual void updateFreeMem(void);

  private:
    // view creation only via ViewFactory
    friend class ViewFactory;
    static TFTView_320x240 *instance(void);
    static TFTView_320x240 *instance(const DisplayDriverConfig &cfg);
    TFTView_320x240();
    TFTView_320x240(DisplayDriver *driver);

    void apply_hotfix(void);
    void ui_events_init(void);
    void ui_set_active(lv_obj_t *b, lv_obj_t *p, lv_obj_t *tp);

    // lvgl event callbacks
    // static void ui_event_HomeButton(lv_event_t * e);
    static void ui_event_NodesButton(lv_event_t *e);
    static void ui_event_GroupsButton(lv_event_t *e);
    static void ui_event_MessagesButton(lv_event_t *e);
    static void ui_event_MapButton(lv_event_t *e);
    static void ui_event_SettingsButton(lv_event_t *e);

    static void ui_event_NodeButtonClicked(lv_event_t *e);
    static void ui_event_ChannelButtonClicked(lv_event_t *e);
    static void ui_event_ChatButtonClicked(lv_event_t *e);
    static void ui_event_ChatDelButtonClicked(lv_event_t *e);
    static void ui_event_MsgPopupButton(lv_event_t *e);
    static void ui_event_Keyboard(lv_event_t *e);

    lv_obj_t *activeButton = nullptr;
    lv_obj_t *activePanel = nullptr;
    lv_obj_t *activeTopPanel = nullptr;
    lv_obj_t *activeMsgContainer = nullptr;

    static TFTView_320x240 *gui;
    time_t lastrun60, lastrun5;
};
