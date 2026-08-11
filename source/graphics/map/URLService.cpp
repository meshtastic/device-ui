#include "graphics/map/URLService.h"
#include "graphics/map/MapTileSettings.h"
#include "graphics/map/TileProvider.h"
#include "lvgl.h"
#include "util/ILog.h"
#include "util/PNGDecoder.h"

#ifdef ARDUINO_ARCH_ESP32

#include "HTTPClient.h" // not available on Linux/Portduino
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
    if (WiFi.status() != WL_CONNECTED) {
        ILOG_DEBUG("URLService::load skipped (WiFi not connected)");
        return false;
    }

#ifdef MUI_WIFI_PS_MIN_MODEM
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
#elif defined(MUI_WIFI_PS_NONE)
    esp_wifi_set_ps(WIFI_PS_NONE);
#endif

    std::string url = TileProvider::url(name);
    if (url.empty()) {
        return false;
    }

    size_t len = 0;
    uint8_t *pngImage = nullptr;
    struct LvFreeGuard {
        uint8_t *&ptr;
        ~LvFreeGuard()
        {
            if (ptr)
                lv_free(ptr);
        }
    } pngGuard{pngImage};

    {
        HTTPClient http;
        http.setReuse(false);

        if (!http.begin(url.c_str())) {
            ILOG_ERROR("ERROR begin %s", url.c_str());
            return false;
        }

        http.addHeader("Accept", "image/png,image/*;q=0.9,*/*;q=0.8");
        http.addHeader("Connection", "close");
        // generate a unique, policy-compliant User-Agent
        char userAgentBuf[128];
        snprintf(userAgentBuf, sizeof(userAgentBuf),
                 "meshtastic/2.8 (ESP32; ID-%08X) contact@meshtastic.org", MapTileSettings::getUniqueId());

        http.setUserAgent(userAgentBuf);
        http.setTimeout(MUI_MAX_TLS_TIMEOUT);

        int httpCode = http.GET();
        if (httpCode != HTTP_CODE_OK) {
            ILOG_ERROR("ERROR GET %s : %d", url.c_str(), httpCode);
            http.end();
            return false;
        }

        int contentLen = http.getSize();
        if (contentLen <= 0) {
            ILOG_WARN("GET %s : empty", url.c_str());
            http.end();
            return false;
        }

        len = (size_t)contentLen;
        pngImage = (uint8_t *)lv_malloc(len);
        if (!pngImage) {
            ILOG_ERROR("lv_malloc failed for %s (%u bytes)", url.c_str(), (unsigned int)len);
            http.end();
            return false;
        }

        WiFiClient *stream = http.getStreamPtr();
        if (!stream) {
            ILOG_ERROR("no WiFiClient stream");
            http.end();
            return false;
        }

        // read .png file in chunks to increase reliability (avoid readBytes())
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
            ILOG_ERROR("http read error %s : %u != %u", url.c_str(), (unsigned int)bytesRead, (unsigned int)len);
            http.end();
            return false;
        }

        http.end();

        ILOG_DEBUG("SUCCESS: GET %s (%u bytes)", url.c_str(), (unsigned int)len);
    }

    // Decode png
    lv_img_dsc_t *img_dsc = nullptr;
    bool decoded = MapTileSettings::color() ? decodeImgColor(pngImage, len, &img_dsc) : decodeImgGrey(pngImage, len, &img_dsc);
    if (decoded) {
        lv_obj_t *img_obj = (lv_obj_t *)img;
        lv_image_set_src(img_obj, img_dsc);
        if (lv_image_get_src(img_obj) != img_dsc) {
            ILOG_ERROR("lv_image_set_src failed for tile %s", name);
            if (img_dsc->data && img_dsc->data_size > 0) {
                lv_free((void *)img_dsc->data);
            }
            lv_free(img_dsc);
            return false;
        }
    } else {
        ILOG_ERROR("Failed to decode tile image %s", name);
        return false;
    }

    if (saveCB && MapTileSettings::saveOK()) {
        bool saveResult = saveCB(name, pngImage, len);
        ILOG_DEBUG("save png to SD -> %s", saveResult ? "OK" : "failed");
        return true;
    }

    return true;
}

#endif
