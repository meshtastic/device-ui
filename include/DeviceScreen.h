#pragma once

#include "DeviceGUI.h"
#include "DisplayDriverConfig.h"
#include "IClientBase.h"

/**
 * @brief DeviceScreen - sets up the GUI and display drivers
 *
 */
class DeviceScreen
{
  public:
    static DeviceScreen &create(void);
    static DeviceScreen &create(const DisplayDriverConfig *cfg);
    static DeviceScreen &create(DisplayDriverConfig &&cfg);

    void init(IClientBase *client);
    void task_handler(void);

  private:
    DeviceScreen(const DisplayDriverConfig *cfg);
    DeviceScreen(DisplayDriverConfig &&cfg);
    DeviceGUI *gui;
};