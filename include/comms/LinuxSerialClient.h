#pragma once

#include "comms/UARTClient.h"

class LinuxSerialClient : public UARTClient
{
  public:
    LinuxSerialClient(const char *tty);
    void init(void) override;
    virtual ~LinuxSerialClient() {}

  protected:
    const char* tty;
};