#pragma once

#include "IClientBase.h"
#include "LogRotate.h"
#include <time.h>

class MeshtasticView;

class ViewController
{
  public:
    ViewController();
    virtual void init(MeshtasticView *gui, IClientBase *_client);
    virtual void runOnce(void);
    virtual bool sleep(int16_t pin);
    virtual void processEvent(void);

    // device config
    virtual uint32_t requestDeviceUIConfig(void);
    virtual uint32_t requestDeviceConfig(uint32_t nodeId = 0);
    virtual uint32_t requestPositionConfig(uint32_t nodeId = 0);
    virtual uint32_t requestPowerConfig(uint32_t nodeId = 0);
    virtual uint32_t requestNetworkConfig(uint32_t nodeId = 0);
    virtual uint32_t requestDisplayConfig(uint32_t nodeId = 0);
    virtual uint32_t requestLoRaConfig(uint32_t nodeId = 0);
    virtual uint32_t requestBluetoothConfig(uint32_t nodeId = 0);
    virtual uint32_t requestRingtone(uint32_t nodeId = 0);
    // request WLAN/BT/MQTT connection status
    virtual bool requestDeviceConnectionStatus(void);

    virtual bool requestReboot(int32_t seconds, uint32_t nodeId = 0);
    virtual bool requestRebootOTA(int32_t seconds, uint32_t nodeId = 0);
    virtual bool requestShutdown(int32_t seconds, uint32_t nodeId = 0);
    virtual bool requestReset(bool factoryReset, uint32_t nodeId = 0);

    virtual bool storeUIConfig(const meshtastic_DeviceUIConfig &config);
    virtual bool sendConfig(const meshtastic_User &user, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_DeviceConfig &&device, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_PositionConfig &&position, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_PowerConfig &&power, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_NetworkConfig &&network, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_DisplayConfig &&display, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_LoRaConfig &&lora, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_BluetoothConfig &&bluetooth, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_SecurityConfig &&security, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Channel &channel, uint32_t nodeId = 0);

    // module config
    virtual bool sendConfig(meshtastic_ModuleConfig_MQTTConfig &&mqtt, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_SerialConfig &&serial, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_ExternalNotificationConfig &&extNotif, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_StoreForwardConfig &&storeForward, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_RangeTestConfig &&rangeTest, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_TelemetryConfig &&telemetry, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_CannedMessageConfig &&cannedMessage, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_AudioConfig &&audio, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_RemoteHardwareConfig &&remoteHW, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_NeighborInfoConfig &&neighborInfo, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_AmbientLightingConfig &&ambientLighting, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_DetectionSensorConfig &&detectionSensor, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_ModuleConfig_PaxcounterConfig &&paxCounter, uint32_t nodeId = 0);

    virtual bool sendConfig(const char ringtone[231], uint32_t nodeId = 0);
    virtual void sendTextMessage(uint32_t to, uint8_t ch, uint8_t hopLimit, uint32_t msgTime, uint32_t requestId, bool usePkc, const char *textmsg);
    virtual void removeTextMessages(uint32_t from, uint32_t to, uint8_t ch);
    virtual bool requestPosition(uint32_t to, uint8_t ch, uint32_t requestId);
    virtual void traceRoute(uint32_t to, uint8_t ch, uint8_t hopLimit, uint32_t requestId);

    // helpers
    virtual bool sendAdminMessage(meshtastic_AdminMessage &config, uint32_t nodeId);
    virtual bool sendAdminMessage(meshtastic_AdminMessage &&config, uint32_t nodeId);
    virtual void sendHeartbeat(void);
    virtual void sendPing(void);

    virtual ~ViewController();

    void setConfigRequested(bool required);

  protected:
    // generic send method to send a (decoded) payload in a meshpacket to radio
    virtual bool send(uint32_t to, meshtastic_PortNum portnum, const meshtastic_Data_payload_t &payload, bool wantRsp = false);
    // generic send method to send a decoded byte string in a meshpacket to radio
    virtual bool send(uint32_t to, uint8_t ch, uint8_t hopLimit, uint32_t requestId, meshtastic_PortNum portnum, bool wantRsp,
                      bool usePkc, const unsigned char bytes[meshtastic_Constants_DATA_PAYLOAD_LEN], size_t len);
    virtual bool receive(void);
    // request initial config from radio
    virtual void requestConfig(void);
    // request additional startup config from radio
    virtual void requestAdditionalConfig(void);
    // request specific config
    virtual uint32_t requestConfig(meshtastic_AdminMessage_ConfigType type, uint32_t nodeId = 0);
    // begin loading persistent messages
    virtual void beginRestoreTextMessages(void);
    // incrementally load persistent messages
    virtual void restoreTextMessages(void);
    // handle received packet and update view
    bool handleFromRadio(const meshtastic_FromRadio &from);
    // handle meshPacket
    bool packetReceived(const meshtastic_MeshPacket &p);

    MeshtasticView *view;
    LogRotate log;
    IClientBase *client;
    uint32_t sendId;
    uint32_t myNodeNum;
    time_t lastrun10;
    time_t lastSetup;
    time_t restoreTimer;
    bool setupDone;             // true if ui config has been loaded and screens are setup in the view
    bool configCompleted;       // true if all data from node has been received
    bool messagesRestored;      // true if log messages have been restored
    bool requestConfigRequired; // true if config needs to be reloaded from the node
};
