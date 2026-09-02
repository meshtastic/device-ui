#pragma once

#include <stdint.h>
#include <string.h>

/**
 * Global settings for raster tile map
 */
class MapTileSettings
{
  public:
    static constexpr size_t PREFIX_SIZE = 10;
    static constexpr size_t TILE_STYLE_SIZE = 24;
    static constexpr size_t TILE_FORMAT_SIZE = 10;
    static constexpr const char *PMTILES_EXTENSION = ".pmtiles";

    MapTileSettings() = default;
    static uint8_t getDefaultZoom(void) { return zoomDefault; }
    static void setDefaultZoom(uint8_t zoom) { zoomDefault = zoom; }

    static uint8_t getZoomLevel(void) { return zoomLevel; }
    static void setZoomLevel(uint8_t level) { zoomLevel = level; }

    static int16_t getTileSize(void) { return tileSize; }
    static void setTileSize(uint16_t size) { tileSize = size; }

    static uint32_t getCacheSize(void) { return cacheSize; }

    static float getDefaultLat(void) { return defaultLat; }
    static void setDefaultLat(float lat) { defaultLat = lat; }

    static float getDefaultLon(void) { return defaultLon; }
    static void setDefaultLon(float lon) { defaultLon = lon; }

    static const char *getPrefix(void) { return prefix; }
    static void setPrefix(const char *p) { copyBounded(prefix, PREFIX_SIZE, p); }

    static const char *getTileStyle(void) { return tileStyle; }
    static void setTileStyle(const char *p)
    {
        copyBounded(tileStyle, TILE_STYLE_SIZE, p);
        pmTiles = (strstr(tileStyle, PMTILES_EXTENSION) != nullptr);
        styleToDir(tileStyle, tileDir, TILE_STYLE_SIZE);
        appendSlash(tileStyle);
        appendSlash(tileDir);
    }

    // directory holding z/x/y tiles for the selected style; same as the style unless it is an archive
    static const char *getTileDir(void) { return tileDir; }
    static bool isPMTiles(void) { return pmTiles; }

    // an archive cannot be written back to, so tiles fetched for it are cached in a
    // z/x/y directory named after the archive; strips the extension and trailing slash
    static void styleToDir(const char *style, char *dst, size_t dstSize)
    {
        copyBounded(dst, dstSize, style);
        char *ext = strstr(dst, PMTILES_EXTENSION);
        if (ext) {
            *ext = '\0';
        }
        size_t len = strlen(dst);
        if (len > 0 && dst[len - 1] == '/') {
            dst[len - 1] = '\0';
        }
    }

    static const char *getTileFormat(void) { return tileFormat; }
    static void setTileFormat(const char *p) { copyBounded(tileFormat, TILE_FORMAT_SIZE, p); }

    static int16_t getTileProvider(void) { return tileProviderId; }
    static void setTileProvider(int16_t id) { tileProviderId = id; }

    static uint32_t getUniqueId(void) { return uniqueId; }
    static void setUniqueId(uint32_t id) { uniqueId = id; }

    static bool color(void) { return colorTiles; }
    static void setColor(bool on) { colorTiles = on; }

    static bool getDebug(void) { return debug; }
    static void setDebug(bool on) { debug = on; }

    static bool saveOK(void) { return save; }
    static void setSaveOK(bool ok) { save = ok; }

  private:
    static void appendSlash(char *dst)
    {
        size_t len = strlen(dst);
        if (len > 0 && dst[len - 1] != '/' && len + 1 < TILE_STYLE_SIZE) {
            dst[len] = '/';
            dst[len + 1] = '\0';
        }
    }

    static void copyBounded(char *dst, size_t dstSize, const char *src)
    {
        if (!dst || dstSize == 0) {
            return;
        }
        if (!src) {
            dst[0] = '\0';
            return;
        }

        strncpy(dst, src, dstSize - 1);
        dst[dstSize - 1] = '\0';
    }

    static uint8_t zoomLevel;
    static uint8_t zoomDefault;
    static uint16_t tileSize;
    static int16_t tileProviderId;
    static bool colorTiles;
    static uint32_t cacheSize;
    static uint32_t uniqueId;
    static float defaultLat;
    static float defaultLon;
    static char prefix[];
    static char tileStyle[];
    static char tileDir[];
    static char tileFormat[];
    static bool pmTiles;
    static bool debug;
    static bool save;
};
