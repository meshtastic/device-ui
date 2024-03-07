#pragma once

#include "mesh-pb-constants.h"

/**
 * @brief Communication interface to be implemented by the user of the device-ui
 * library. The interface implementation has to be passed when initializing the
 * DeviceScreen.
 * @tparam T
 */

class IClientBase {
public:
  virtual void init(void) = 0;
  virtual bool connect(void) = 0;
  virtual bool disconnect(void) = 0;
  virtual bool isConnected(void) = 0;
  virtual bool send(meshtastic_ToRadio &&to) = 0;
  virtual meshtastic_FromRadio receive(void) = 0;
  virtual ~IClientBase(){};

protected:
  IClientBase() = default;
};
