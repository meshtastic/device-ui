#pragma once

#include "SerialClient.h"
#include <time.h>

class WLANClient : public SerialClient {
  public:
    WLANClient(void);
    void init(void) override;
    bool connect(void) override;
    bool disconnect(void) override;
    bool isConnected(void) override;
    bool send(meshtastic_ToRadio &&to) override;
    meshtastic_FromRadio receive(void) override;
    virtual ~WLANClient();

  protected:
    // low-level send method to write the encoded buffer to WLAN
    bool send(const uint8_t* buf, size_t len);

    // low-level receive method, periodically being called via thread
    size_t receive(uint8_t* buf, size_t space_left);

    time_t lastReceived;
};