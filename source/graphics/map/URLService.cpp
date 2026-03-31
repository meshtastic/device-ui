#include "graphics/map/URLService.h"
#include "graphics/map/MapTileSettings.h"
#include "graphics/map/TileProvider.h"
#include "lvgl.h"
#include "util/ILog.h"

#ifdef ARDUINO_ARCH_ESP32

// from ConvertPNG.c
extern "C" {
bool decodeImgGrey(const void *data, size_t size, lv_img_dsc_t **img);
bool decodeImgColor(const void *data, size_t size, lv_img_dsc_t **img);
}

URLService::URLService(Callback cb) : ITileService("HTTP:"), saveCB(cb) {}

URLService::~URLService() {}

bool URLService::load(const char *name, void *img)
{
    struct HttpEndGuard {
        decltype(http) &client;
        ~HttpEndGuard() { client.end(); }
    } httpGuard{http};

    struct LvFreeGuard {
        uint8_t *&ptr;
        ~LvFreeGuard() { lv_free(ptr); }
    };

    // transform filename to provider url
    std::string url = TileProvider::url(name);
    http.begin(url.c_str());
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        ILOG_ERROR("ERROR GET %s : %d", url.c_str(), httpCode);
        return false;
    }

    WiFiClient *stream = http.getStreamPtr();
    size_t len = http.getSize();
    if (len == 0) {
        ILOG_WARN("GET %s : empty", url.c_str());
        return false;
    }

    uint8_t *pngImage = (uint8_t *)lv_malloc(len);
    LvFreeGuard pngGuard{pngImage};
    if (!pngImage) {
        ILOG_ERROR("lv_malloc failed for %s (%u bytes)", url.c_str(), (unsigned int)len);
        return false;
    }

    size_t bytesRead = stream->readBytes(pngImage, len);
    if (bytesRead != len) {
        ILOG_ERROR("http read error %s : %u != %u", url.c_str(), (unsigned int)bytesRead, (unsigned int)len);
        return false;
    }
    ILOG_DEBUG("SUCCESS: GET %s (%u bytes)", url.c_str(), (unsigned int)bytesRead);

    // save png tile to SD card
    if (saveCB && MapTileSettings::saveOK()) {
        bool result = saveCB(name, pngImage, len);
        ILOG_DEBUG("save png to SD -> %s", result ? "OK" : "failed");
    }

    // decode png via STBI library
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

    return true;
}

#endif