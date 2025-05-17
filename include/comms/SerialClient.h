#pragma once

#include "comms/IClientBase.h"
#include "comms/MeshEnvelope.h"
#include "util/SharedQueue.h"

class SerialClient : public IClientBase
{
  public:
    SerialClient(void);
    void init(void) override;
    bool sleep(int16_t pin);
    bool connect(void) override;
    bool disconnect(void) override;
    bool isConnected(void) override;
    bool isStandalone(void) override;
    bool send(meshtastic_ToRadio &&to) override;
    meshtastic_FromRadio receive(void) override;

    void task_handler(void) override;
    void setNotifyCallback(std::function<void(bool status)> notifyConnectionStatus) override;
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

    // status handling, to be called by derived classes
    void setConnectionStatus(bool status);

    // thread handling stuff and data
    static void task_loop(void *);
    static SerialClient *instance;

    // local data
    size_t pb_size;
    size_t bytes_read;
    uint8_t *buffer;

    // status of server connection
    bool connectionStatus;
    volatile bool connected;
    // announce client shutdown
    volatile bool shutdown;
    // callback for connection status
    std::function<void(bool status)> notifyConnectionStatus;

    // receiver and sender queue
    SharedQueue queue;
};