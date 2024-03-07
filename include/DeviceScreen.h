#pragma once

#include "DeviceGUI.h"
#include "IClientBase.h"

/**
 * @brief DeviceScreen - sets up the GUI and display drivers
 *
 */
class DeviceScreen {
public:
  static DeviceScreen &create(void);

  void init(IClientBase *client);
  void task_handler(void);

private:
  DeviceScreen();
  DeviceGUI *gui;
};