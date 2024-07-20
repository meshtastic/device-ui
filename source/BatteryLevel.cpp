#include "BatteryLevel.h"

BatteryLevel::BatteryLevel(void)
    : levels{
          {100, CHARGING_VOLTAGE}, // Charging
          {80, 3.80},              // Full
          {35, 3.50},              // Mid
          {10, 3.30},              // Low
          {0, 3.12},               // Empty
          {0, 3.10}                // Warn
      }
{
}

BatteryLevel::Status BatteryLevel::calcStatus(uint32_t percentage, float voltage)
{
    if (percentage >= levels[Charging].percentage && voltage > levels[Charging].voltage) {
        return Charging;
    } else if (percentage >= levels[Full].percentage && voltage > levels[Full].voltage) {
        return Full;
    } else if (percentage >= levels[Mid].percentage && voltage > levels[Mid].voltage) {
        return Mid;
    } else if (percentage >= levels[Low].percentage && voltage > levels[Low].voltage) {
        return Low;
    } else if (percentage >= levels[Empty].percentage && voltage > levels[Empty].voltage) {
        return Empty;
    } else if (percentage >= levels[Warn].percentage && voltage > levels[Warn].voltage) {
        return Warn;
    } else {
        return Slash;
    }
}
