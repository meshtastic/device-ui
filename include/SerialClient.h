#pragma once

#include "IClientBase.h"
#include "MeshEnvelope.h"
#include "SharedQueue.h"

class SerialClient : public IClientBase
{
  public:
    SerialClient(void);
    void init(void) override;
    bool sleep(int16_t pin);
    bool connect(void) override;
    bool disconnect(void) override;
    bool isConnected(void) override;
    bool send(meshtastic_ToRadio &&to) override;
    meshtastic_FromRadio receive(void) override;
    virtual ~SerialClient();

  protected:
    // low-level send method to write the encoded buffer to serial
    virtual bool send(const uint8_t *buf, size_t len);

    // low-level receive method, periodically being called via thread
    virtual size_t receive(uint8_t *buf, size_t space_left);

    // received a full packet from serial, process it
    virtual void handlePacketReceived(void);

    // received a full packet from serial, process it
    virtual void handleSendPacket(void);

    // thread handling stuff and data
    static void task_loop(void *);
    static SerialClient *instance;

    volatile bool shutdown;
    uint8_t *buffer;

    // local data
    volatile bool connected;
    size_t pb_size;
    size_t bytes_read;

    // receiver and sender queue
    SharedQueue queue;
};