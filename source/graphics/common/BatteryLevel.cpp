#include "graphics/common/BatteryLevel.h"

BatteryLevel::BatteryLevel(void)
    : levels{
          {100, 0.0f},                    // Plugged
          {100, (float)CHARGING_VOLTAGE}, // Charging
          {80, 4.00f},                    // Full
          {35, 3.50f},                    // Mid
          {10, 3.30f},                    // Low
          {0, 3.12f},                     // Empty
          {0, 3.10f}                      // Warn
      }
{
}

BatteryLevel::Status BatteryLevel::calcStatus(uint32_t percentage, float voltage)
{
    if (voltage == levels[Plugged].voltage) {
        return Plugged;
    }
    if (percentage >= levels[Charging].percentage && voltage > levels[Charging].voltage) {
        return Charging;
    } else if (percentage >= levels[Full].percentage && voltage > levels[Full].voltage) {
        return Full;
    } else if (percentage >= levels[Mid].percentage && voltage > levels[Mid].voltage) {
        return Mid;
    } else if (percentage >= levels[Low].percentage && voltage > levels[Low].voltage) {
        return Low;
    } else if (percentage > levels[Empty].percentage) {
        return Empty;
    } else {
        return Warn;
    }
}
