#include "graphics/map/URLService.h"
#include "graphics/map/MapTileSettings.h"
#include "graphics/map/TileProvider.h"
#include "lvgl.h"
#include "util/ILog.h"
#include "util/PNGDecoder.h"

#ifdef ARDUINO_ARCH_ESP32

#include "WiFi.h"
#include "esp_wifi.h"

#ifndef MUI_MAX_TLS_TIMEOUT
#define MUI_MAX_TLS_TIMEOUT 2000
#endif

#ifndef MUI_MAX_IDLE_SPINS
#define MUI_MAX_IDLE_SPINS 100
#endif

URLService::URLService(Callback cb) : ITileService("HTTP:"), saveCB(cb)
{
    initPNGDecoder();
}

URLService::~URLService() {}

bool URLService::load(const char *name, void *img)
{
    lv_image_dsc_t *img_dsc = loadRaw(name);
    if (!img_dsc)
        return false;
    lv_obj_t *img_obj = (lv_obj_t *)img;
    lv_image_set_src(img_obj, img_dsc);
    if (lv_image_get_src(img_obj) != img_dsc) {
        ILOG_ERROR("lv_image_set_src failed for tile %s", name);
        if (img_dsc->data && img_dsc->data_size > 0)
            lv_free((void *)img_dsc->data);
        lv_free(img_dsc);
        return false;
    }
    return true;
}

lv_image_dsc_t *URLService::loadRaw(const char *name)
{
    return prepareLoad(name)();
}

ITileService::PreparedLoad URLService::prepareLoad(const char *name)
{
    const std::string filename = name ? name : "";
    const bool cache = MapTileSettings::saveOK();
    const bool color = MapTileSettings::color();
    const uint32_t revision = MapTileSettings::getSourceRevision();
    const uint32_t uniqueId = MapTileSettings::getUniqueId();
    // Resolve on the UI task: a queued filename must never acquire the URL of a
    // different provider selected while it waits for the worker.
    const auto url = !cache && TileProvider::url() == "https://tile.openstreetmap.org/{z}/{x}/{y}.png"
                         ? std::string()
                         : TileProvider::url(name);
    return [this, filename, url, cache, color, revision, uniqueId]() {
        return loadUrl(filename, url, cache, color, revision, uniqueId);
    };
}

lv_image_dsc_t *URLService::loadUrl(const std::string &filename, const std::string &url, bool cache, bool color,
                                  uint32_t sourceRevision, uint32_t uniqueId)
{
    const char *name = filename.c_str();
    if (sourceRevision != MapTileSettings::getSourceRevision())
        return nullptr;
    if (WiFi.status() != WL_CONNECTED) {
        ILOG_DEBUG("URLService::loadRaw skipped (WiFi not connected)");
        return nullptr;
    }

#ifdef MUI_WIFI_PS_MIN_MODEM
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
#elif defined(MUI_WIFI_PS_NONE)
    esp_wifi_set_ps(WIFI_PS_NONE);
#endif

    if (url.empty()) {
        return nullptr;
    }

    size_t len = 0;
    uint8_t *pngImage = nullptr;
    // guard frees pngImage on all exit paths; never call lv_free(pngImage) explicitly
    struct LvFreeGuard {
        uint8_t *&ptr;
        ~LvFreeGuard()
        {
            if (ptr)
                lv_free(ptr);
        }
    } pngGuard{pngImage};
    http.setReuse(true);

    if (!http.begin(url.c_str())) {
        ILOG_ERROR("ERROR begin tile %s", name);
        return nullptr;
    }

    http.addHeader("Accept", "image/png,image/*;q=0.9,*/*;q=0.8");
    http.addHeader("Connection", "keep-alive");
    char userAgentBuf[128];
    snprintf(userAgentBuf, sizeof(userAgentBuf), "meshtastic/2.8 (ESP32; ID-%08X) contact@meshtastic.org",
             uniqueId);

    http.setUserAgent(userAgentBuf);
    http.setTimeout(MUI_MAX_TLS_TIMEOUT);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        ILOG_ERROR("ERROR GET tile %s : %d", name, httpCode);
        http.end();
        return nullptr;
    }

    int contentLen = http.getSize();
    if (contentLen <= 0) {
        ILOG_WARN("GET tile %s : empty", name);
        http.end();
        return nullptr;
    }

    len = (size_t)contentLen;
    pngImage = (uint8_t *)lv_malloc(len);
    if (!pngImage) {
        ILOG_ERROR("lv_malloc failed for tile %s (%u bytes)", name, (unsigned int)len);
        http.end();
        return nullptr;
    }

    WiFiClient *stream = http.getStreamPtr();
    if (!stream) {
        ILOG_ERROR("no WiFiClient stream");
        http.end();
        return nullptr;
    }

    size_t bytesRead = 0;
    uint16_t idleSpins = 0;
    const uint16_t maxIdleSpins = MUI_MAX_IDLE_SPINS;
    while (bytesRead < len) {
        size_t available = stream->available();
        if (available == 0) {
            if (++idleSpins > maxIdleSpins) {
                break;
            }
            delay(5);
            continue;
        }

        idleSpins = 0;
        size_t toRead = available;
        size_t remaining = len - bytesRead;
        if (toRead > remaining) {
            toRead = remaining;
        }

        int got = stream->read(pngImage + bytesRead, toRead);
        if (got <= 0) {
            break;
        }
        bytesRead += (size_t)got;
    }

    if (bytesRead != len) {
        ILOG_ERROR("http read error for tile %s : %u != %u", name, (unsigned int)bytesRead, (unsigned int)len);
        http.end();
        return nullptr;
    }

    ILOG_DEBUG("SUCCESS: GET tile %s (%u bytes)", name, (unsigned int)len);

    lv_image_dsc_t *img_dsc = nullptr;
    bool decoded = color ? decodeImgColor(pngImage, len, &img_dsc) : decodeImgGrey(pngImage, len, &img_dsc);
    http.end();
    if (!decoded) {
        ILOG_ERROR("Failed to decode tile image %s", name);
        return nullptr; // pngGuard frees pngImage
    }

    if (saveCB && cache && sourceRevision == MapTileSettings::getSourceRevision()) {
        bool saveResult = saveCB(name, pngImage, len);
        ILOG_DEBUG("save png to SD -> %s", saveResult ? "OK" : "failed");
    }
    // pngGuard frees pngImage; img_dsc->data (decoded pixels) is a separate allocation
    return img_dsc;
}

#endif
