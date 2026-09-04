#include "graphics/map/PMTileService.h"

#if defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC) || defined(HAS_SDCARD) || defined(SENSECAP_INDICATOR)

#include "util/Gunzip.h"
#include "util/ILog.h"
#include "util/PNGDecoder.h"
#include <cstdio>
#include <cstring>

PMTileService::PMTileService(ITileService *fb, IMapFileSystem *fs)
    : ITileService(nullptr), fallback(fb), archiveFS(fs), archiveValid(false), pmHeader{}
{
    openedStyle[0] = '\0';
    memset(cachedDirOffset, 0, sizeof(cachedDirOffset));
    initPNGDecoder();
}

static bool archivePathForStyle(char *dst, size_t dstSize)
{
    char dir[MapTileSettings::TILE_STYLE_SIZE];
    MapTileSettings::styleToDir(MapTileSettings::getTileStyle(), dir, sizeof(dir));
    if (!dir[0])
        return false;
    int len = snprintf(dst, dstSize, "%s/%s/%s%s", MapTileSettings::getPrefix(), dir, dir, MapTileSettings::PMTILES_EXTENSION);
    return len >= 0 && len < (int)dstSize;
}

PMTileService::~PMTileService()
{
    closeArchive();
    delete fallback;
    delete archiveFS;
}

bool PMTileService::load(const char *name, void *img)
{
    uint32_t z, x, y;
    if (MapTileSettings::isPMTiles() && parseZXY(name, z, x, y) && openArchive() && loadFromArchive(z, x, y, img)) {
        return true;
    }
    return fallback ? fallback->load(name, img) : false;
}

bool PMTileService::save(const char *name, void *img, size_t len)
{
    return fallback ? fallback->save(name, img, len) : false;
}

/**
 * extract z/x/y from a ".../<z>/<x>/<y>.png" tile path
 */
bool PMTileService::parseZXY(const char *name, uint32_t &z, uint32_t &x, uint32_t &y)
{
    if (!name)
        return false;

    const char *slash[3] = {nullptr, nullptr, nullptr};
    for (const char *p = name; *p; ++p) {
        if (*p == '/') {
            slash[0] = slash[1];
            slash[1] = slash[2];
            slash[2] = p;
        }
    }
    if (!slash[0])
        return false;

    return sscanf(slash[0], "/%u/%u/%u", &z, &x, &y) == 3;
}

void PMTileService::closeArchive(void)
{
    if (archiveFS)
        archiveFS->close();
    archiveValid = false;
    openedStyle[0] = '\0';
    for (uint8_t i = 0; i < MAX_DIR_DEPTH; i++) {
        cachedDirOffset[i] = 0;
        cachedDirEntries[i].clear();
        cachedDirEntries[i].shrink_to_fit();
    }
}

bool PMTileService::openArchive(void)
{
    const char *style = MapTileSettings::getTileStyle();
    if (openedStyle[0] && strcmp(openedStyle, style) == 0) {
        return archiveValid;
    }
    closeArchive();
    strncpy(openedStyle, style, sizeof(openedStyle) - 1);
    openedStyle[sizeof(openedStyle) - 1] = '\0';

    char fname[128];
    if (!archivePathForStyle(fname, sizeof(fname))) {
        ILOG_ERROR("Invalid pmtiles style: %s", style);
        return false;
    }
    ILOG_DEBUG("SD open pmtiles file: %s", fname);

    if (!archiveFS || !archiveFS->open(fname)) {
        ILOG_ERROR("Failed to open pmtiles file %s", fname);
        return false;
    }

    uint8_t header[pmtiles::HEADER_BYTES];
    if (!archiveFS->readAt(0, header, sizeof(header)) || !pmtiles::deserialize_header(header, sizeof(header), pmHeader)) {
        archiveFS->close();
        return false;
    }

    ILOG_DEBUG("PMTiles tile type: %d (UNK/MVT/PNG/JPG/WEBP/AVIF)", (uint16_t)pmHeader.tile_type);
    ILOG_DEBUG("PMTiles tile compression: %d (UNK/NO/GZ/BR/Z)", (uint16_t)pmHeader.tile_compression);
    ILOG_DEBUG("PMTiles internal compression: %d", (uint16_t)pmHeader.internal_compression);
    ILOG_DEBUG("PMTiles zoom: %d..%d, clustered: %d", (uint16_t)pmHeader.min_zoom, (uint16_t)pmHeader.max_zoom,
               pmHeader.clustered);

    if (pmHeader.tile_type != pmtiles::TILETYPE_PNG && pmHeader.tile_type != pmtiles::TILETYPE_UNKNOWN) {
        ILOG_ERROR("Unsupported tile type: %d", (uint16_t)pmHeader.tile_type);
    } else if (pmHeader.tile_compression != pmtiles::COMPRESSION_NONE &&
               pmHeader.tile_compression != pmtiles::COMPRESSION_UNKNOWN) {
        ILOG_ERROR("Unsupported tile compression: %d", (uint16_t)pmHeader.tile_compression);
    } else if (pmHeader.internal_compression != pmtiles::COMPRESSION_GZIP) {
        ILOG_ERROR("Unsupported internal compression: %d", (uint16_t)pmHeader.internal_compression);
    } else {
        archiveValid = true;
    }

    if (!archiveValid) {
        archiveFS->close();
    }
    return archiveValid;
}

bool PMTileService::loadFromArchive(uint32_t z, uint32_t x, uint32_t y, void *img)
{
    if (!archiveValid || z < pmHeader.min_zoom || z > pmHeader.max_zoom) {
        return false;
    }

    const uint32_t start = lv_tick_get();
    (void)start;
    const uint64_t tile_id = pmtiles::zxy_to_tileid(z, x, y);
    uint64_t dir_offset = pmHeader.root_dir_offset;
    uint32_t dir_length = (uint32_t)pmHeader.root_dir_bytes;
    uint8_t *tileData = nullptr;
    uint32_t tileLength = 0;

    for (uint8_t depth = 0; depth < MAX_DIR_DEPTH && !tileData; depth++) {
        if (dir_offset != cachedDirOffset[depth] || cachedDirEntries[depth].empty()) {
            uint8_t *dirBuffer = (uint8_t *)lv_malloc(dir_length);
            if (!dirBuffer) {
                ILOG_ERROR("Failed to allocate %u bytes for pmtiles directory", (unsigned int)dir_length);
                return false;
            }
            {
                if (!archiveFS->readAt(dir_offset, dirBuffer, dir_length)) {
                    ILOG_ERROR("Failed to read %u bytes of pmtiles directory at %llu", (unsigned int)dir_length,
                               (unsigned long long)dir_offset);
                    lv_free(dirBuffer);
                    return false;
                }
            }

            size_t decompressedSize = 0;
            uint8_t *decompressedDir = decompressGzip(dirBuffer, dir_length, decompressedSize);
            lv_free(dirBuffer);
            if (!decompressedDir) {
                return false;
            }
            cachedDirEntries[depth] = pmtiles::deserialize_directory((const char *)decompressedDir, decompressedSize);
            cachedDirOffset[depth] = dir_offset;
            lv_free(decompressedDir);
        }

        pmtiles::entryv3 entry = pmtiles::find_tile(cachedDirEntries[depth], tile_id);
        if (entry.length == 0) {
            ILOG_DEBUG("Tile %d/%d/%d not in archive", z, x, y);
            return false;
        }

        if (entry.run_length == 0) {
            // leaf directory, descend
            dir_offset = pmHeader.leaf_dirs_offset + entry.offset;
            dir_length = entry.length;
            continue;
        }

        tileLength = entry.length;
        tileData = (uint8_t *)lv_malloc(tileLength);
        if (!tileData) {
            ILOG_ERROR("Failed to allocate %u bytes for tile data", (unsigned int)tileLength);
            return false;
        }
        {
            if (!archiveFS->readAt(pmHeader.tile_data_offset + entry.offset, tileData, tileLength)) {
                ILOG_ERROR("Failed to read %u bytes of tile data", (unsigned int)tileLength);
                lv_free(tileData);
                return false;
            }
        }
    }

    if (!tileData) {
        ILOG_WARN("Tile %d/%d/%d not found after traversing directories", z, x, y);
        return false;
    }

    lv_image_dsc_t *img_dsc = nullptr;
    bool decoded =
        MapTileSettings::color() ? decodeImgColor(tileData, tileLength, &img_dsc) : decodeImgGrey(tileData, tileLength, &img_dsc);
    lv_free(tileData);

    if (!decoded) {
        ILOG_ERROR("Failed to decode tile image %d/%d/%d", z, x, y);
        return false;
    }

    lv_obj_t *img_obj = (lv_obj_t *)img;
    lv_image_set_src(img_obj, img_dsc);
    if (lv_image_get_src(img_obj) != img_dsc) {
        ILOG_ERROR("lv_image_set_src failed for tile %d/%d/%d", z, x, y);
        if (img_dsc->data) {
            lv_free((void *)img_dsc->data);
        }
        lv_free(img_dsc);
        return false;
    }

    ILOG_DEBUG("Tile %d/%d/%d loaded from archive in %d ms", z, x, y, lv_tick_get() - start);
    return true;
}

#endif
