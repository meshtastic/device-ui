#pragma once

#include "mesh-pb-constants.h"
#include "stdint.h"
#include <functional>

/**
 * @brief Communication interface to be implemented by the user of the device-ui
 * library. The interface implementation has to be passed when initializing the
 * DeviceScreen.
 * @tparam T
 */

class IClientBase
{
  public:
    virtual void init(void) = 0;
    virtual bool connect(void) = 0;
    virtual bool disconnect(void) = 0;
    virtual bool isConnected(void) = 0;
    virtual bool isStandalone(void) = 0;
    virtual bool sleep(int16_t pin) { return false; }

    virtual bool send(meshtastic_ToRadio &&to) = 0;
    virtual meshtastic_FromRadio receive(void) = 0;
    virtual ~IClientBase(){};

    virtual void task_handler(void){};
    virtual void setNotifyCallback(std::function<void(bool status)> notifyConnectionStatus) = 0;

  protected:
    IClientBase() = default;
};
