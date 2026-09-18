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
    bool isAsync() const override { return true; }
    lv_image_dsc_t *loadRaw(const char *name) override;
    PreparedLoad prepareLoad(const char *name) override;
    virtual ~URLService();

  private:
    lv_image_dsc_t *loadUrl(const std::string &filename, const std::string &url, bool cache, bool color,
                           uint32_t sourceRevision, uint32_t uniqueId);
    Callback saveCB = nullptr;
    HTTPClient http;
};

#endif
