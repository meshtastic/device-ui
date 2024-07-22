#pragma once

#include <stdint.h>

#ifndef CHARGING_VOLTAGE
#define CHARGING_VOLTAGE 4.30
#endif

class BatteryLevel
{
  public:
    enum Status { Plugged, Charging, Full, Mid, Low, Empty, Warn };

    BatteryLevel(void);
    Status calcStatus(uint32_t percentage, float voltage);

  private:
    struct {
        uint32_t percentage;
        float voltage;
    } levels[6];
};