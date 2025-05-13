#pragma once

#include "Client.h"
#include "comms/IClientBase.h"
#include "util/SharedQueue.h"
#include <stdint.h>

#ifndef SERVER_PORT
#define SERVER_PORT 4403
#endif

class EthClient : public IClientBase
{
  public:
    EthClient(const char *serverName = "localhost", uint16_t port = SERVER_PORT);
    EthClient(IPAddress server, uint16_t port = SERVER_PORT);
    EthClient(uint8_t *mac, IPAddress ip, IPAddress server = IPAddress(127, 0, 0, 1), uint16_t port = SERVER_PORT);
    // EthClient(uint8_t *mac, IPAddress ip, IPAddress server, IPAddress gateway, IPAddress subnet);
    void init(void) override;
    bool sleep(int16_t pin);
    bool connect(void) override;
    bool disconnect(void) override;
    bool isConnected(void) override;
    bool send(meshtastic_ToRadio &&to) override;
    meshtastic_FromRadio receive(void) override;
    virtual ~EthClient();

  protected:
    // low-level send method to write the encoded buffer to ethernet
    virtual bool send(const uint8_t *buf, size_t len);

    // low-level receive method, periodically being called via thread
    virtual size_t receive(uint8_t *buf, size_t space_left);

    // received a full packet from ethernet, process it
    virtual void handlePacketReceived(void);

    // send packet via ethernet
    virtual void handleSendPacket(void);

    // thread handling stuff and data
    static void task_loop(void *);
    static EthClient *instance;

    volatile bool shutdown;
    uint8_t *buffer;

    // local data
    volatile bool connected;
    size_t pb_size;
    size_t bytes_read;

    Client *client;
    uint8_t mac[6];
    IPAddress localIP;
    IPAddress serverIP;
    const char *server;
    uint16_t serverPort;

    // receiver and sender queue
    SharedQueue queue;
};