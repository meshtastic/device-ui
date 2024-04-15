#pragma once
#include "DisplayDriverConfig.h"

class DisplayDriver;

class DisplayDriverFactory
{
  public:
    static DisplayDriver *create(uint16_t width, uint16_t height);
    static DisplayDriver *create(const DisplayDriverConfig &config);

  private:
    DisplayDriverFactory(void);
    // TODO: DisplayDriverFactory(uint32_t width, uint32_t height);
    // TODO: DisplayDriverFactory(const DisplayDriverConfig& config);
};
