#pragma once

#include "DisplayDriver.h"

template <class OLED> class OLEDDriver : public DisplayDriver {
public:
  OLEDDriver(OLED *oled, uint16_t width, uint16_t height);
  virtual void init(void);

protected:
  OLED *oled;
};

template <class OLED>
OLEDDriver<OLED>::OLEDDriver(OLED *oled, uint16_t width, uint16_t height)
    : DisplayDriver(width, height), OLED(oled) {}

template <class OLED> void OLEDDriver<OLED>::init(void) {
  DisplayDriver::init();
}
