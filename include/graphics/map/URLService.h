#pragma once

#include "graphics/map/TileService.h"
#include <functional>

#if defined(ARDUINO_ARCH_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32P4)
#include "HTTPClient.h"

class URLService : public ITileService
{
  public:
    using Callback = std::function<bool(const char *name, void *img, size_t len)>;

    URLService(Callback cb = nullptr);
    bool load(const char *name, void *img) override;
    bool isAsync() const override { return true; }
    lv_image_dsc_t *loadRaw(const char *name) override;
    virtual ~URLService();

  private:
    Callback saveCB = nullptr;
    HTTPClient http;
};

#endif