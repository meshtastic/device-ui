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
    virtual void sendText(uint32_t to, uint8_t ch, const char *textmsg);
    virtual void sendConfig(void); // TODO
    virtual ~ViewController();

    void setConfigRequested(bool required);

  protected:
    // generic send method to send a decoded byte string in a meshpacket to radio
    virtual bool send(uint32_t to, uint8_t ch, meshtastic_PortNum portnum, const uint8_t *decoded, size_t len);
    // try receive packet from radio via client interface
    virtual bool receive(void);
    // request initial config from radio
    virtual void requestConfig(void);
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
