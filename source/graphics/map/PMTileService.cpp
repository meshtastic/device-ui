#if defined(ARCH_PORTDUINO) || defined(HAS_SDCARD)

#include "graphics/map/PMTileService.h"
#include "graphics/map/MapTileSettings.h"
#include "util/ILog.h"

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_MALLOC(sz) lv_malloc(sz)
#define STBI_REALLOC(p, newsz) lv_realloc(p, newsz)
#define STBI_FREE(p) lv_free(p)
#define STB_IMAGE_IMPLEMENTATION
#include "graphics/map/stb_image.h"

#if defined(ARCH_PORTDUINO)
#define O_RDONLY "r"
#endif

// gzip helper
extern std::string decompressGzip(const std::string &compressedData);
extern uint8_t *decompressGzip(const uint8_t *compressedData, size_t length, size_t &actualDecompressedSize);

PMTileService::PMTileService() : ITileService(nullptr)
{
    // Set the default tile prefix
    char fname[128];
    sprintf(fname, "%s/%s", MapTileSettings::getPrefix(), MapTileSettings::getTileStyle());
    ILOG_DEBUG("SD open pmtiles file: %s", fname);

    pmTiles = SDFs.open(fname, O_RDONLY);
    if (pmTiles) {
        // Read the header
        std::string header_s(sizeof(pmtiles::headerv3), '\0');
        pmTiles.readBytes(&header_s[0], sizeof(pmtiles::headerv3));
        pmHeader = pmtiles::deserialize_header(header_s);
        if (strncmp(header_s.c_str(), "PMTiles", 7) == 0) {
            if (header_s.c_str()[7] == '\003') {
                ILOG_DEBUG("PMTiles version: %d", (uint16_t)(header_s.c_str()[7]));
                ILOG_DEBUG("PMTiles tile type: %d (UNK/MVT/PNG/JPG/WEBP/AVIF)", (uint16_t)pmHeader.tile_type);
                ILOG_DEBUG("PMTiles tile compression: %d (UNK/NO/GZ/BR/Z)", (uint16_t)pmHeader.tile_compression);
                ILOG_DEBUG("PMTiles internal compression: %d", (uint16_t)pmHeader.internal_compression);
                ILOG_DEBUG("PMTiles min zoom: %d", (uint16_t)pmHeader.min_zoom);
                ILOG_DEBUG("PMTiles max zoom: %d", (uint16_t)pmHeader.max_zoom);
                ILOG_DEBUG("PMTiles clustered: %d", pmHeader.clustered);

                if (pmHeader.tile_type != pmtiles::TILETYPE_JPEG && pmHeader.tile_type != pmtiles::TILETYPE_PNG &&
                    pmHeader.tile_type != pmtiles::TILETYPE_UNKNOWN) {
                    ILOG_ERROR("Unsupported tile type: %d", (uint16_t)pmHeader.tile_type);
                    pmTiles.close();
                } else if (pmHeader.tile_compression != pmtiles::COMPRESSION_NONE &&
                           pmHeader.tile_compression != pmtiles::COMPRESSION_UNKNOWN) {
                    ILOG_ERROR("Unsupported tile compression: %d", (uint16_t)pmHeader.tile_compression);
                    pmTiles.close();
                } else if (pmHeader.internal_compression != pmtiles::COMPRESSION_GZIP) {
                    ILOG_ERROR("Unsupported internal compression: %d", (uint16_t)pmHeader.internal_compression);
                    pmTiles.close();
                }
            } else {
                ILOG_ERROR("Unsupported PMTiles version: %d", (uint16_t)(header_s.c_str()[7]));
                pmTiles.close();
            }
        } else {
            ILOG_ERROR("Invalid .pmtiles file %s", fname);
            pmTiles.close();
        }
    } else {
        ILOG_ERROR("Failed to open pmtiles file %s", fname);
    }
}

PMTileService::~PMTileService()
{
    if (pmTiles) {
        pmTiles.close();
    }
}

void PMTileService::convertRGB888ToRGB565(uint8_t *src, uint16_t *dst, int width, int height)
{
    uint32_t totalPixels = width * height;
    uint32_t srcIndex = 0;
    for (int i = 0; i < totalPixels; i++) {
        uint8_t r = src[srcIndex];
        uint8_t g = src[srcIndex + 1];
        uint8_t b = src[srcIndex + 2];
        dst[i] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3); // Convert RGB888 to RGB565
        srcIndex += 3;
    }
}

bool PMTileService::decodeImg(const void *data, size_t size, lv_img_dsc_t *&img)
{
    int width, height, channels;
    uint8_t *decodedData = stbi_load_from_memory((stbi_uc *)data, size, &width, &height, &channels, STBI_rgb);
    if (decodedData) {
        // Allocate memory for RGB565 data
        uint16_t *rgb565Data = (uint16_t *)lv_malloc(width * height * sizeof(uint16_t));
        if (!rgb565Data) {
            lv_free(decodedData);
            return false;
        }

        // Convert RGB888 to RGB565
        convertRGB888ToRGB565(decodedData, rgb565Data, width, height);

        // Free the original RGB888 data
        lv_free(decodedData);

        // Populate the LVGL image descriptor
        img = (lv_img_dsc_t *)lv_malloc_zeroed(sizeof(lv_img_dsc_t));
        img->header.magic = LV_IMAGE_HEADER_MAGIC;
        img->header.w = width;
        img->header.h = height;
        img->header.cf = LV_COLOR_FORMAT_RGB565;
        img->header.flags = LV_IMAGE_FLAGS_ALLOCATED | LV_IMAGE_FLAGS_MODIFIABLE;
        img->data = (uint8_t *)rgb565Data;
        img->data_size = width * height * sizeof(uint16_t);
        return true;
    }
    return false;
}

std::string PMTileService::decompress(const std::string &data, uint8_t compression)
{
    if (compression == pmtiles::COMPRESSION_NONE) {
        return data;
    } else if (compression == pmtiles::COMPRESSION_GZIP) {
        std::string decompressed = decompressGzip(data);
        if (decompressed.size()) {
            return decompressed;
        } else {
            ILOG_ERROR("Failed to decompress");
        }
    } else {
        ILOG_ERROR("Unsupported compression type: %d", compression);
    }
    return {};
}

bool PMTileService::load(uint32_t x, uint32_t y, uint32_t z, void *img)
{
    if (!pmTiles || !pmTiles.available() || z < pmHeader.min_zoom || z > pmHeader.max_zoom) {
        return false;
    }
    uint32_t timing = micros();

    // Locate the tile using the directory
    uint64_t dir_offset = pmHeader.root_dir_offset;
    uint32_t dir_length = static_cast<uint32_t>(pmHeader.root_dir_bytes);

    for (int depth = 0; depth <= 3; depth++) {
        const std::vector<pmtiles::entryv3> *dirEntries = nullptr;
        if (dir_offset != cachedDirOffset[depth]) {
            // Read the directory
            uint8_t *dirBuffer = (uint8_t *)lv_malloc(dir_length);
            if (!dirBuffer) {
                ILOG_ERROR("Failed to allocate memory for dirBuffer");
                return false;
            }
            pmTiles.seek(dir_offset);
#ifdef ARCH_PORTDUINO
            pmTiles.readBytes((char *)dirBuffer, dir_length);
#else
            pmTiles.readBytes(dirBuffer, dir_length);
#endif
            // Decompress and deserialize the directory
            // std::string decompressedDir = decompress(dirBuffer, pmHeader.internal_compression);
            size_t decompressedSize = 0;
            uint8_t *decompressedDir = decompressGzip(dirBuffer, dir_length, decompressedSize);
            if (!decompressedDir) {
                lv_free(dirBuffer);
                return false;
            }
            // Deserialize the directory
            cachedDirOffset[depth] = dir_offset;
            cachedDirEntries[depth] = pmtiles::deserialize_directory((const char *)decompressedDir, decompressedSize);
            lv_free(decompressedDir);
            lv_free(dirBuffer);
        }
        dirEntries = &cachedDirEntries[depth];

        // Find the tile entry
        uint64_t tile_id = pmtiles::zxy_to_tileid(z, x, y);
        auto entry = pmtiles::find_tile(*dirEntries, tile_id);

        if (entry.length > 0) {
            if (entry.run_length > 0) {
                int32_t find_tile_time = micros() - timing;
                // Tile found
                uint64_t tile_offset = pmHeader.tile_data_offset + entry.offset;
                uint32_t tile_length = entry.length;

                // Read the tile data
                uint8_t *tileData = (uint8_t *)lv_malloc(tile_length);
                if (!tileData) {
                    ILOG_ERROR("Failed to allocate memory for tile data");
                    return false;
                }
                pmTiles.seek(tile_offset);
#ifdef ARCH_PORTDUINO
                pmTiles.readBytes((char *)tileData, tile_length);
#else
                pmTiles.readBytes(tileData, tile_length);
#endif

                int32_t read_tile_time = micros() - find_tile_time - timing;

                // Decode the tile data
                lv_img_dsc_t *img_dsc;
                if (decodeImg(tileData, tile_length, img_dsc)) {
                    lv_image_set_src((lv_obj_t *)img, img_dsc);
                    int32_t decode_time = micros() - read_tile_time - find_tile_time - timing;
                    ILOG_DEBUG("Loading tile %d/%d/%d took %d-%d-%d us", z, x, y, find_tile_time, read_tile_time, decode_time);
                    lv_free(tileData);
                    return true;
                } else {
                    ILOG_ERROR("Failed to decode tile image %d/%d/%d", z, x, y);
                    lv_free(tileData);
                    return false;
                }
            } else {
                // Navigate to the next directory
                dir_offset = pmHeader.leaf_dirs_offset + entry.offset;
                dir_length = entry.length;
            }
        } else {
            ILOG_WARN("Tile %d/%d/%d not found!", z, x, y);
            return false;
        }
    }

    ILOG_WARN("Tile not found after traversing directories!");
    return false;
}

#endif