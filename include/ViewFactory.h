#pragma once
#include "DisplayDriverConfig.h"

class DeviceGUI;

class ViewFactory
{
  public:
    static DeviceGUI *create(void);
    static DeviceGUI *create(const DisplayDriverConfig &cfg);

  protected:
    ViewFactory(void);
};
