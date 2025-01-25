#pragma once

#include "stdint.h"

/**
 * Base class for all implemented display device drivers
 */
class DisplayDeviceDriver
{
  public:
    DisplayDeviceDriver(){};
    virtual void init(void) {}
    virtual bool hasTouch(void) { return false; }
    virtual bool hasButton(void) { return false; }
    virtual bool hasLight(void) { return false; }
    virtual bool isPowersaving(void) { return false; }
    // virtual void task_handler(void) {};

    virtual uint8_t getBrightness(void) const { return 128; }
    virtual void setBrightness(uint8_t brightness) {}

    virtual uint16_t getScreenTimeout() const { return 0; }
    virtual void setScreenTimeout(uint16_t timeout) {}

    virtual ~DisplayDeviceDriver() = default;

  private:
    DisplayDeviceDriver(const DisplayDeviceDriver &) = delete;
    DisplayDeviceDriver &operator=(const DisplayDeviceDriver &) = delete;
};