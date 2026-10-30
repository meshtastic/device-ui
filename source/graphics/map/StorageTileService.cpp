#include "graphics/map/StorageTileService.h"
#include "graphics/map/MapTileSettings.h"
#include "storage/IStorage.h"
#include "util/ILog.h"
#include "util/PNGDecoder.h"
#include <cstdint>
#include <string>

StorageTileService::StorageTileService(IStorage &storage) : ITileService(nullptr), storage(storage)
{
    initPNGDecoder();
}

bool StorageTileService::load(const char *name, void *img)
{
    uint32_t start = lv_tick_get();
    auto file = storage.open(name, OpenMode::Read);
    if (!file) {
        ILOG_DEBUG("Failed to open tile %s", name);
        return false;
    }
    uint64_t size = file->size();
    if (size == 0 || size > UINT32_MAX) {
        ILOG_DEBUG("Tile %s has invalid size", name);
        return false;
    }
    uint32_t len = (uint32_t)size;
    uint8_t *png = (uint8_t *)lv_malloc(len);
    if (!png) {
        ILOG_ERROR("lv_malloc failed for %s (%u bytes)", name, (unsigned int)len);
        return false;
    }
    int32_t got = file->read(png, len);
    file.reset();
    if (got != (int32_t)len) {
        ILOG_ERROR("read error %s : %d != %u", name, (int)got, (unsigned int)len);
        lv_free(png);
        return false;
    }

    lv_image_dsc_t *img_dsc = nullptr;
    bool decoded = MapTileSettings::color() ? decodeImgColor(png, len, &img_dsc) : decodeImgGrey(png, len, &img_dsc);
    lv_free(png);
    if (!decoded) {
        ILOG_ERROR("Failed to decode tile image %s", name);
        return false;
    }

    lv_obj_t *img_obj = (lv_obj_t *)img;
    lv_image_set_src(img_obj, img_dsc);
    if (lv_image_get_src(img_obj) != img_dsc) {
        ILOG_ERROR("lv_image_set_src failed for tile %s", name);
        if (img_dsc->data && img_dsc->data_size > 0)
            lv_free((void *)img_dsc->data);
        lv_free(img_dsc);
        return false;
    }
    ILOG_DEBUG("Tile %s loaded in %d ms.", name, lv_tick_get() - start);
    return true;
}

bool StorageTileService::save(const char *name, void *img, size_t len)
{
    ILOG_DEBUG("StorageTileService::save(%s): %d", name, len);
    std::string dir(name);
    size_t slash = dir.rfind('/');
    if (slash != std::string::npos && slash > 0 && !storage.mkdir(dir.substr(0, slash).c_str())) {
        ILOG_ERROR("failed to create directory for %s", name);
        return false;
    }
    auto file = storage.open(name, OpenMode::Write);
    if (!file) {
        ILOG_ERROR("failed to write %s", name);
        return false;
    }
    bool written = file->write(img, len) == (int32_t)len;
    file.reset();
    // a truncated tile would satisfy the next load and never be fetched again
    if (!written)
        storage.remove(name);
    return written;
}
