#pragma once
#include "graphics/driver/DisplayDriver.h"

/**
 * @brief For simulation on pc/raspberry
 * This class provides an SDL GUI on the local desktop; dimensions are defined
 * in lv_drv_conf.h Usage: define USE_SDL=1 for the rasbian/portduino target and
 * link with -lSDL
 */
class SDLDriver : public DisplayDriver
{
  public:
    static SDLDriver &create(uint16_t width, uint16_t height);
    void init(DeviceGUI *gui) override;
    void task_handler(void) override;
    virtual ~SDLDriver() {}

  private:
    SDLDriver(uint16_t width, uint16_t height);

    static SDLDriver *SDLdriver;
};