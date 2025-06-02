#pragma once

#include "graphics/driver/DisplayDriver.h"

template <class EINK> class EINKDriver : public DisplayDriver
{
  public:
    EINKDriver(EINK *eink, uint16_t width, uint16_t height);
    void init(DeviceGUI *gui) override;

  protected:
    EINK *einkDriver;
};

template <class EINK>
EINKDriver<EINK>::EINKDriver(EINK *eink, uint16_t width, uint16_t height) : DisplayDriver(width, height), einkDriver(eink)
{
}

template <class EINK> void EINKDriver<EINK>::init(DeviceGUI *gui)
{
    DisplayDriver::init(gui);
}
