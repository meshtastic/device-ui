#pragma once

#include "DeviceGUI.h"
#include "DisplayDriverConfig.h"
#include "ResponseHandler.h"
#include "lvgl.h"
#include "mesh-pb-constants.h"
#include <array>
#include <stdint.h>
#include <string>
#include <time.h>
#include <unordered_map>

#define LV_OBJ_IDX(x) spec_attr->children[x]

constexpr uint8_t c_max_channels = 8;
constexpr uint32_t c_request_timeout = 60 * 1000;

class ViewController;

class MeshtasticView : public DeviceGUI
{
  public:
    const uint32_t secs_until_offline = 60 * 60 + 60;

    MeshtasticView(const DisplayDriverConfig *cfg, DisplayDriver *driver, ViewController *_controller);
    virtual void init(IClientBase *client);
    virtual void task_handler(void);
    virtual void triggerHeartbeat(void);
    virtual bool sleep(int16_t pin);

    uint32_t getMyNodeNum(void) { return ownNode; }

    enum eRole {
        client,
        client_mute,
        router,
        router_client,
        repeater,
        tracker,
        sensor,
        tak,
        client_hidden,
        lost_and_found,
        tak_tracker,
        unknown
    };

    // methods to update view
    virtual void setupUIConfig(const meshtastic_DeviceUIConfig& uiconfig) {}
    virtual void setMyInfo(uint32_t nodeNum);
    virtual void setDeviceMetaData(int hw_model, const char *version, bool has_bluetooth, bool has_wifi, bool has_eth,
                                   bool can_shutdown);
    virtual void addOrUpdateNode(uint32_t nodeNum, uint8_t channel, uint32_t lastHeard, eRole role, bool hasKey, bool viaMqtt);
    virtual void addOrUpdateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong,
                                 uint32_t lastHeard, eRole role, bool hasKey, bool viaMqtt);
    virtual void addNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                         eRole role, bool hasKey, bool viaMqtt);
    virtual void updateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                            eRole role,  bool hasKey, bool viaMqtt);
    virtual void updatePosition(uint32_t nodeNum, int32_t lat, int32_t lon, int32_t alt, uint32_t sats, uint32_t precision);
    virtual void updateMetrics(uint32_t nodeNum, uint32_t bat_level, float voltage, float chUtil, float airUtil);
    virtual void updateEnvironmentMetrics(uint32_t nodeNum, const meshtastic_EnvironmentMetrics &metrics) {}
    virtual void updateAirQualityMetrics(uint32_t nodeNum, const meshtastic_AirQualityMetrics &metrics) {}
    virtual void updatePowerMetrics(uint32_t nodeNum, const meshtastic_PowerMetrics &metrics) {}
    virtual void updateSignalStrength(uint32_t nodeNum, int32_t rssi, float snr);
    virtual void updateHopsAway(uint32_t nodeNum, uint8_t hopsAway) {}
    virtual void updateConnectionStatus(const meshtastic_DeviceConnectionStatus &status) {}

    // methods to update device config
    virtual void updateChannelConfig(const meshtastic_Channel &ch) {}
    virtual void updateDeviceConfig(const meshtastic_Config_DeviceConfig &cfg) {}
    virtual void updatePositionConfig(const meshtastic_Config_PositionConfig &cfg) {}
    virtual void updatePowerConfig(const meshtastic_Config_PowerConfig &cfg) {}
    virtual void updateNetworkConfig(const meshtastic_Config_NetworkConfig &cfg) {}
    virtual void updateDisplayConfig(const meshtastic_Config_DisplayConfig &cfg) {}
    virtual void updateLoRaConfig(const meshtastic_Config_LoRaConfig &cfg) {}
    virtual void updateBluetoothConfig(const meshtastic_Config_BluetoothConfig &cfg) {}
    virtual void updateSecurityConfig(const meshtastic_Config_SecurityConfig &cfg) {}
    virtual void updateSessionKeyConfig(const meshtastic_Config_SessionkeyConfig &cfg) {}

    // methods to update module config
    virtual void updateMQTTModule(const meshtastic_ModuleConfig_MQTTConfig &cfg) {}
    virtual void updateSerialModule(const meshtastic_ModuleConfig_SerialConfig &cfg) {}
    virtual void updateExtNotificationModule(const meshtastic_ModuleConfig_ExternalNotificationConfig &cfg) {}
    virtual void updateStoreForwardModule(const meshtastic_ModuleConfig_StoreForwardConfig &cfg) {}
    virtual void updateRangeTestModule(const meshtastic_ModuleConfig_RangeTestConfig &cfg) {}
    virtual void updateTelemetryModule(const meshtastic_ModuleConfig_TelemetryConfig &cfg) {}
    virtual void updateCannedMessageModule(const meshtastic_ModuleConfig_CannedMessageConfig &) {}
    virtual void updateAudioModule(const meshtastic_ModuleConfig_AudioConfig &cfg) {}
    virtual void updateRemoteHardwareModule(const meshtastic_ModuleConfig_RemoteHardwareConfig &cfg) {}
    virtual void updateNeighborInfoModule(const meshtastic_ModuleConfig_NeighborInfoConfig &cfg) {}
    virtual void updateAmbientLightingModule(const meshtastic_ModuleConfig_AmbientLightingConfig &cfg) {}
    virtual void updateDetectionSensorModule(const meshtastic_ModuleConfig_DetectionSensorConfig &cfg) {}
    virtual void updatePaxCounterModule(const meshtastic_ModuleConfig_PaxcounterConfig &cfg) {}
    virtual void updateFileinfo(const meshtastic_FileInfo &fileinfo) {}
    virtual void updateRingtone(const char rtttl[231]) {}

    virtual void configCompleted(void) { configComplete = true; }

    virtual void handleResponse(uint32_t from, uint32_t id, const meshtastic_Routing &routing, const meshtastic_MeshPacket &p) {}
    virtual void handleResponse(uint32_t from, uint32_t id, const meshtastic_RouteDiscovery &route) {}
    virtual void handlePositionResponse(uint32_t from, uint32_t request_id, int32_t rx_rssi, float rx_snr, bool isNeighbor) {}
    virtual void packetReceived(const meshtastic_MeshPacket &p);
    virtual void newMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg);
    virtual void notifyResync(bool show);
    virtual void notifyReboot(bool show);
    virtual void notifyShutdown(void);
    virtual void showMessagePopup(const char *from);

    virtual void removeNode(uint32_t nodeNum);

    // local update methods
    virtual void updateLastHeard(uint32_t nodeNum);
    virtual void updateTime(uint32_t time) {}

  protected:
    // helpers
    std::tuple<uint32_t, uint32_t> nodeColor(uint32_t nodeNum);
    bool lastHeardToString(uint32_t lastHeard, char *buf);
    const char *deviceRoleToString(enum eRole role);
    std::string pskToBase64(const meshtastic_ChannelSettings_psk_t &psk);
    bool base64ToPsk(const std::string &base64, meshtastic_ChannelSettings_psk_t &psk);

    ViewController *controller;
    ResponseHandler requests;
    std::unordered_map<uint32_t, lv_obj_t *> nodes;       // node panels
    std::unordered_map<uint32_t, lv_obj_t *> messages;    // message containers (within ui_MessagesPanel)
    std::unordered_map<uint32_t, lv_obj_t *> chats;       // active chats (within ui_ChatPanel)
    std::array<lv_obj_t *, c_max_channels> channel;       // TODO channel name and info
    std::array<lv_obj_t *, c_max_channels> channelGroup;  // message containers for channel group
    uint32_t nodeCount = 1, nodesOnline = 1, ownNode = 0; // node info
    uint32_t unreadMessages = 0;                          // messages
    bool configComplete = false;                          // config request finishe
    time_t lastrun20 = 0;                                 // 20s task
};
