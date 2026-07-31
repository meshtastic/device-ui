#pragma once

#include "graphics/map/TileService.h"
#include <functional>

#ifdef ARCH_PORTDUINO

#include <curl/curl.h>

#ifdef INADDR_NONE
// Winsock defines INADDR_NONE as a macro; ArduinoCore-API declares it as an IPAddress symbol.
#undef INADDR_NONE
#endif

/**
 * CURLService is a TileService implementation that uses libcurl to fetch map tiles over HTTP.
 * It supports saving tiles to a cache via a user-provided callback.
 */
class CURLService : public ITileService
{
  public:
    using Callback = std::function<bool(const char *name, void *img, size_t len)>;

    CURLService(Callback cb = nullptr);
    bool load(const char *name, void *img) override;
    virtual ~CURLService();

  private:
    Callback saveCB = nullptr;
    CURL *curlHandle = nullptr;
    uint64_t offlineUntilMs = 0;
};

#endif
