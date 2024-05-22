#pragma once

#include "IClientBase.h"
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
    virtual uint32_t requestDeviceConfig(uint32_t nodeId = 0);
    virtual uint32_t requestPositionConfig(uint32_t nodeId = 0);
    virtual uint32_t requestPowerConfig(uint32_t nodeId = 0);
    virtual uint32_t requestNetworkConfig(uint32_t nodeId = 0);
    virtual uint32_t requestDisplayConfig(uint32_t nodeId = 0);
    virtual uint32_t requestLoRaConfig(uint32_t nodeId = 0);
    virtual uint32_t requestBluetoothConfig(uint32_t nodeId = 0);
    virtual bool sendConfig(const meshtastic_User &user, uint32_t nodeId);
    virtual bool sendConfig(meshtastic_Config_DeviceConfig &&device, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_PositionConfig &&position, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_PowerConfig &&power, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_NetworkConfig &&network, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_DisplayConfig &&display, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_LoRaConfig &&lora, uint32_t nodeId = 0);
    virtual bool sendConfig(meshtastic_Config_BluetoothConfig &&bluetooth, uint32_t nodeId = 0);

    // module config

    virtual void sendText(uint32_t to, uint8_t ch, const char *textmsg);
    virtual void sendHeartbeat(void);

    virtual ~ViewController();

    void setConfigRequested(bool required);

  protected:
    // generic send method to send a (decoded) payload in a meshpacket to radio
    virtual bool send(uint32_t to, meshtastic_PortNum portnum, const meshtastic_Data_payload_t &payload);
    // generic send method to send a decoded byte string in a meshpacket to radio
    virtual bool send(uint32_t to, uint8_t ch, meshtastic_PortNum portnum, const uint8_t *decoded, size_t len);
    // try receive packet from radio via client interface
    virtual bool receive(void);
    // request initial config from radio
    virtual void requestConfig(void);
    // request specific config
    virtual uint32_t requestConfig(meshtastic_AdminMessage_ConfigType type, uint32_t nodeId = 0);
    // request WLAN/BT/MQTT connection status
    virtual void requestDeviceConnectionStatus(void);
    // handle received packet and update view
    bool handleFromRadio(const meshtastic_FromRadio &from);
    // handle meshPacket
    bool packetReceived(const meshtastic_MeshPacket &p);

    MeshtasticView *view;
    IClientBase *client;
    uint32_t sendId;
    time_t lastrun10;
    bool requestConfigRequired;
};
