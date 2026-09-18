#pragma once

#include <stdint.h>
#include <string.h>

/**
 * Global settings for raster tile map
 */
class MapTileSettings
{
  public:
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
    static void setPrefix(const char *p) { strcpy(prefix, p); }

    static const char *getTileStyle(void) { return tileStyle; }
    static void setTileStyle(const char *p)
    {
        strcpy(tileStyle, p);
        size_t len = strlen(tileStyle);
        if (len > 0 && tileStyle[len - 1] != '/')
            strcat(tileStyle, "/");
    }

    static const char *getTileFormat(void) { return tileFormat; }
    static void setTileFormat(const char *p) { strcpy(tileFormat, p); }

    static bool getDebug(void) { return debug; }
    static void setDebug(bool on) { debug = on; }

  private:
    static uint8_t zoomLevel;
    static uint8_t zoomDefault;
    static uint16_t tileSize;
    static uint32_t cacheSize;
    static float defaultLat;
    static float defaultLon;
    static char prefix[];
    static char tileStyle[];
    static char tileFormat[];
    static bool debug;
};
