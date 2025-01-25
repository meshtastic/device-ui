#pragma once

#ifdef LVGL_DRIVER

#include "drivers/display/st7789/lv_st7789.h"
#include "graphics/driver/DisplayDeviceDriver.h"
#include "graphics/driver/DisplayDriverConfig.h"

class LVGL_ST7789_Driver : public DisplayDeviceDriver
{
  public:
    LVGL_ST7789_Driver() {}
    LVGL_ST7789_Driver(const DisplayDriverConfig &) : DisplayDeviceDriver() {}

  protected:
  private:
};

#endif