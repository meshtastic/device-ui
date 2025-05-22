#pragma once

#include "Client.h"
#include "comms/SerialClient.h"
#include "util/SharedQueue.h"
#include <stdint.h>

#ifndef SERVER_PORT
#define SERVER_PORT 4403
#endif

class EthClient : public SerialClient
{
  public:
    EthClient(const char *serverName = "localhost", uint16_t port = SERVER_PORT);
    void init(void) override;
    bool connect(void) override;
    bool disconnect(void) override;
    bool isConnected(void) override;
    //    bool send(meshtastic_ToRadio &&to) override;
    meshtastic_FromRadio receive(void) override;
    virtual ~EthClient();

  protected:
    // low-level send method to write the encoded buffer to ethernet
    bool send(const uint8_t *buf, size_t len) override;

    // low-level receive method, periodically being called via thread
    size_t receive(uint8_t *buf, size_t space_left) override;

    Client *client;
    uint8_t mac[6];
    IPAddress localIP;
    IPAddress serverIP;
    const char *server;
    uint16_t serverPort;
};