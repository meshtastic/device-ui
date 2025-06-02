#pragma once

#include "lvgl.h"
#include "stdint.h"

/**
 * Base class for all implemented display device drivers
 */
class DisplayDeviceDriver
{
  public:
    DisplayDeviceDriver(uint16_t width, uint16_t height) : screenWidth(width), screenHeight(height){};

    virtual void init(void) {}
    virtual lv_display_t *createDisplay(uint32_t hor_res, uint32_t ver_res) = 0;

    virtual uint8_t getBrightness(void) const { return 128; }
    virtual void setBrightness(uint8_t brightness) {}

    virtual uint16_t getScreenTimeout() const { return 0; }
    virtual void setScreenTimeout(uint16_t timeout) {}

    virtual bool hasButton(void) const { return false; }
    virtual bool hasTouch(void) const { return false; }
    virtual bool light(void) { return false; }

    virtual ~DisplayDeviceDriver(){};

  protected:
    const uint16_t screenWidth;
    const uint16_t screenHeight;

  private:
    DisplayDeviceDriver(const DisplayDeviceDriver &) = delete;
    DisplayDeviceDriver &operator=(const DisplayDeviceDriver &) = delete;
};