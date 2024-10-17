#pragma once

#include "HardwareSerial.h"
#include "SerialClient.h"
#include <time.h>

class UARTClient : public SerialClient
{
  public:
    UARTClient(void);
    void init(void) override;
    bool connect(void) override;
    bool disconnect(void) override;
    bool isConnected(void) override;
    meshtastic_FromRadio receive(void) override;
    virtual ~UARTClient();

    bool isActive(void) const;
    const char* getConnectionInfo(void) const;

  protected:
    // low-level send method to write the buffer to serial
    bool send(const uint8_t *buf, size_t len) override;

    // low-level receive method, periodically being called via thread
    size_t receive(uint8_t *buf, size_t space_left) override;

    HardwareSerial *_serial;
    time_t lastReceived;
};