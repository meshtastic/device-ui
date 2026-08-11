#pragma once

#include "graphics/map/TileService.h"
#include <functional>

#ifdef ARDUINO_ARCH_ESP32
#include "HTTPClient.h"

class URLService : public ITileService
{
  public:
    using Callback = std::function<bool(const char *name, void *img, size_t len)>;

    URLService(Callback cb = nullptr);
    bool load(const char *name, void *img) override;
    virtual ~URLService();

  private:
    Callback saveCB = nullptr;
    HTTPClient http;
};

#endif