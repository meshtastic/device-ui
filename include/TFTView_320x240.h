#pragma once

#include "MeshtasticView.h"

/**
 * @brief GUI view for e.g. T-Deck
 * Handles creation of display driver and controller.
 * Note: due to static callbacks in lvgl this class is modelled as
 *       a singleton with static callback members
 */
class TFTView_320x240 : public MeshtasticView {
public:
  static TFTView_320x240 *instance(void);
  virtual void init(IClientBase *client);
  virtual void task_handler(void);

  // methods to update view
  virtual void setMyInfo(uint32_t nodeNum);
  virtual void setDeviceMetaData(int hw_model, const char *version,
                                 bool has_bluetooth, bool has_wifi,
                                 bool has_eth, bool can_shutdown);
  virtual void addOrUpdateNode(uint32_t nodeNum, uint8_t channel,
                               const char *userShort, const char *userLong,
                               uint32_t lastHeard, eRole role);
  virtual void addNode(uint32_t nodeNum, uint8_t channel, const char *userShort,
                       const char *userLong, uint32_t lastHeard, eRole role);
  virtual void updateNode(uint32_t nodeNum, uint8_t channel,
                          const char *userShort, const char *userLong,
                          uint32_t lastHeard, eRole role);
  virtual void updatePosition(uint32_t nodeNum, int32_t lat, int32_t lon,
                              int32_t alt, uint32_t precision);
  virtual void updateMetrics(uint32_t nodeNum, uint32_t bat_level,
                             float voltage, float chUtil, float airUtil);
  virtual void updateSignalStrength(uint32_t nodeNum, int32_t rssi, float snr);
  virtual void packetReceived(uint32_t from, const meshtastic_MeshPacket &p);

  // methods to update device config
  virtual void updateChannelConfig(uint32_t index, const char *name,
                                   const uint8_t *psk, uint32_t psk_size,
                                   uint8_t role);
  virtual void updateDeviceConfig(const meshtastic_Config_DeviceConfig &cfg) {}
  virtual void
  updatePositionConfig(const meshtastic_Config_PositionConfig &cfg) {}
  virtual void updatePowerConfig(const meshtastic_Config_PowerConfig &cfg) {}
  virtual void updateNetworkConfig(const meshtastic_Config_NetworkConfig &cfg) {
  }
  virtual void updateDisplayConfig(const meshtastic_Config_DisplayConfig &cfg) {
  }
  virtual void updateLoRaConfig(const meshtastic_Config_LoRaConfig &cfg) {}
  virtual void
  updateBluetoothConfig(const meshtastic_Config_BluetoothConfig &cfg) {}

  // methods to update module config
  virtual void updateMQTTModule(const meshtastic_ModuleConfig_MQTTConfig &cfg) {
  }
  virtual void
  updateSerialModule(const meshtastic_ModuleConfig_SerialConfig &cfg) {}
  virtual void updateExtNotificationModule(
      const meshtastic_ModuleConfig_ExternalNotificationConfig &cfg) {}
  virtual void updateStoreForwardModule(
      const meshtastic_ModuleConfig_StoreForwardConfig &cfg) {}
  virtual void
  updateRangeTestModule(const meshtastic_ModuleConfig_RangeTestConfig &cfg) {}
  virtual void
  updateTelemetryModule(const meshtastic_ModuleConfig_TelemetryConfig &cfg) {}
  virtual void updateCannedMessageModule(
      const meshtastic_ModuleConfig_CannedMessageConfig &) {}
  virtual void
  updateAudioModule(const meshtastic_ModuleConfig_AudioConfig &cfg) {}
  virtual void updateRemoteHardwareModule(
      const meshtastic_ModuleConfig_RemoteHardwareConfig &cfg) {}
  virtual void updateNeighborInfoModule(
      const meshtastic_ModuleConfig_NeighborInfoConfig &cfg) {}
  virtual void updateAmbientLightingModule(
      const meshtastic_ModuleConfig_AmbientLightingConfig &cfg) {}
  virtual void updateDetectionSensorModule(
      const meshtastic_ModuleConfig_DetectionSensorConfig &cfg) {}
  virtual void
  updatePaxCounterModule(const meshtastic_ModuleConfig_PaxcounterConfig &cfg) {}

  virtual void removeNode(uint32_t nodeNum);
  virtual void newMessage(const char *msg){};

protected:
  // add own message to current chat
  virtual void addMessage(char *msg);
  // set node image based on role
  virtual void setNodeImage(uint32_t nodeNum, eRole role, lv_obj_t *img);
  // set last heard to now, update nodes online
  virtual void updateLastHeard(uint32_t nodeNum);
  // update last heard value on all node panels
  virtual void updateAllLastHeard(void);

private:
  TFTView_320x240();

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
  static void ui_event_Keyboard(lv_event_t *e);

  lv_obj_t *activeButton = nullptr;
  lv_obj_t *activePanel = nullptr;
  lv_obj_t *activeTopPanel = nullptr;

  static TFTView_320x240 *gui;
};
