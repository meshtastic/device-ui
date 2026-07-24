#include "graphics/map/CURLService.h"
#include "graphics/map/MapTileSettings.h"
#include "graphics/map/TileProvider.h"
#include "lvgl.h"
#include "util/ILog.h"

#ifdef ARCH_PORTDUINO

#include <chrono>
#include <curl/curl.h>

namespace
{
constexpr long CURL_CONNECT_TIMEOUT_MS = 500L;
constexpr long CURL_REQUEST_TIMEOUT_MS = 900L;
constexpr long CURL_LOW_SPEED_LIMIT_BYTES = 1L;
constexpr long CURL_LOW_SPEED_TIME_S = 1L;
constexpr uint64_t CURL_OFFLINE_BACKOFF_MS = 5000ULL;

uint64_t monotonicMs()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

bool isNetworkUnavailable(CURLcode code)
{
    return code == CURLE_OPERATION_TIMEDOUT || code == CURLE_COULDNT_RESOLVE_HOST || code == CURLE_COULDNT_CONNECT ||
           code == CURLE_GOT_NOTHING || code == CURLE_RECV_ERROR || code == CURLE_SEND_ERROR ||
           code == CURLE_COULDNT_RESOLVE_PROXY;
}
} // namespace

// from ConvertPNG.c
extern "C" {
bool decodeImgGrey(const void *data, size_t size, lv_img_dsc_t **img);
bool decodeImgColor(const void *data, size_t size, lv_img_dsc_t **img);
}

// libcurl write callback: appends received data into a std::vector<uint8_t>
struct CurlBuffer {
    uint8_t *data = nullptr;
    size_t size = 0;
    size_t capacity = 0;
};

static size_t curlWriteCallback(void *contents, size_t memberSize, size_t nmemb, void *userp)
{
    size_t incoming = memberSize * nmemb;
    CurlBuffer *buf = static_cast<CurlBuffer *>(userp);

    size_t needed = buf->size + incoming;
    if (needed > buf->capacity) {
        size_t newCapacity = needed * 2;
        uint8_t *newData = (uint8_t *)lv_realloc(buf->data, newCapacity);
        if (!newData)
            return 0; // signal error to libcurl
        buf->data = newData;
        buf->capacity = newCapacity;
    }

    memcpy(buf->data + buf->size, contents, incoming);
    buf->size += incoming;
    return incoming;
}

CURLService::CURLService(Callback cb) : ITileService("HTTP:"), saveCB(cb)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curlHandle = curl_easy_init();
    if (!curlHandle) {
        ILOG_ERROR("curl_easy_init failed in CURLService ctor");
    }
}

CURLService::~CURLService()
{
    if (curlHandle) {
        curl_easy_cleanup(curlHandle);
        curlHandle = nullptr;
    }
    curl_global_cleanup();
}

bool CURLService::load(const char *name, void *img)
{
    const uint64_t nowMs = monotonicMs();
    if (nowMs < offlineUntilMs) {
        return false;
    }

    std::string url = TileProvider::url(name);
    if (url.empty()) {
        ILOG_ERROR("empty URL for tile %s", name ? name : "(null)");
        return false;
    }

    CURL *curl = curlHandle;
    if (!curl) {
        ILOG_ERROR("curl_easy_init failed for tile %s", name ? name : "(null)");
        return false;
    }

    curl_easy_reset(curl);

    CurlBuffer buf{};
    struct CurlHeaderGuard {
        curl_slist *list = nullptr;
        ~CurlHeaderGuard()
        {
            if (list) {
                curl_slist_free_all(list);
            }
        }
    } headers;

    headers.list = curl_slist_append(headers.list, "Accept: image/png,image/*;q=0.9,*/*;q=0.8");
    headers.list = curl_slist_append(headers.list, "Connection: keep-alive");
    if (url.find("google.com") != std::string::npos) {
        headers.list = curl_slist_append(headers.list, "Referer: https://www.google.com/");
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.list);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "MUI/1.0 (+https://meshtastic.org)");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, CURL_CONNECT_TIMEOUT_MS);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, CURL_REQUEST_TIMEOUT_MS);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, CURL_LOW_SPEED_LIMIT_BYTES);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, CURL_LOW_SPEED_TIME_S);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 300L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

    CURLcode res = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    if (res != CURLE_OK) {
        if (isNetworkUnavailable(res)) {
            offlineUntilMs = monotonicMs() + CURL_OFFLINE_BACKOFF_MS;
            ILOG_WARN("Network unavailable, pausing HTTP tile fetches for %lu ms", (unsigned long)CURL_OFFLINE_BACKOFF_MS);
        }
        ILOG_ERROR("ERROR GET %s : %s (HTTP %ld)", url.c_str(), curl_easy_strerror(res), httpCode);
        lv_free(buf.data);
        return false;
    }

    if (httpCode < 200 || httpCode >= 300) {
        ILOG_ERROR("ERROR GET %s : HTTP %ld", url.c_str(), httpCode);
        lv_free(buf.data);
        return false;
    }

    if (buf.size == 0) {
        ILOG_WARN("GET %s : empty response", url.c_str());
        lv_free(buf.data);
        return false;
    }

    ILOG_DEBUG("SUCCESS: GET %s (%u bytes, HTTP %ld)", url.c_str(), (unsigned int)buf.size, httpCode);
    offlineUntilMs = 0;

    // save png tile (e.g. to filesystem cache)
    if (saveCB && MapTileSettings::saveOK()) {
        bool result = saveCB(name, buf.data, buf.size);
        ILOG_DEBUG("save png to cache -> %s", result ? "OK" : "failed");
    }

    // decode png via STBI library
    lv_img_dsc_t *img_dsc = nullptr;
    bool decoded =
        MapTileSettings::color() ? decodeImgColor(buf.data, buf.size, &img_dsc) : decodeImgGrey(buf.data, buf.size, &img_dsc);
    lv_free(buf.data);

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
