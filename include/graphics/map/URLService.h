#pragma once

#include "graphics/map/TileService.h"
#include "lvgl.h"

#ifdef ARDUINO_ARCH_ESP32
#include "HTTPClient.h" // not available on Linux/Portduino

class URLService : public ITileService
{
  public:
    URLService();
    bool load(const char *name, void *img) override;
    virtual ~URLService();

  private:
    HTTPClient http;
};

#endif