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
    static constexpr size_t TILE_STYLE_SIZE = 20;
    static constexpr size_t TILE_FORMAT_SIZE = 10;

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
        size_t len = strlen(tileStyle);
        if (len > 0 && tileStyle[len - 1] != '/' && len + 1 < TILE_STYLE_SIZE) {
            tileStyle[len] = '/';
            tileStyle[len + 1] = '\0';
        }
    }

    static const char *getTileFormat(void) { return tileFormat; }
    static void setTileFormat(const char *p) { copyBounded(tileFormat, TILE_FORMAT_SIZE, p); }

    static uint16_t getTileProvider(void) { return tileProviderId; }
    static void setTileProvider(uint16_t id) { tileProviderId = id; }

    static bool color(void) { return colorTiles; }
    static void setColor(bool on) { colorTiles = on; }

    static bool getDebug(void) { return debug; }
    static void setDebug(bool on) { debug = on; }

    static bool saveOK(void) { return save; }
    static void setSaveOK(bool ok) { save = ok; }

  private:
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
    static uint16_t tileProviderId;
    static bool colorTiles;
    static uint32_t cacheSize;
    static float defaultLat;
    static float defaultLon;
    static char prefix[];
    static char tileStyle[];
    static char tileFormat[];
    static bool debug;
    static bool save;
};
