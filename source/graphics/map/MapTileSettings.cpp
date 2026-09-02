#include "graphics/map/MapTileSettings.h"
#include "lv_conf.h"
#include "lvgl.h"

uint8_t MapTileSettings::zoomLevel = 13;   // current zoomLevel
uint8_t MapTileSettings::zoomDefault = 13; // default for initial or home position
uint16_t MapTileSettings::tileSize = 256;
int16_t MapTileSettings::tileProviderId = -1;       // default url index to load from (backup service)
uint32_t MapTileSettings::cacheSize = 50 * 1024;    // LV_FS_CACHE_FROM_BUFFER
uint32_t MapTileSettings::uniqueId = 0xFFFFFFFF;    // to be updated with node number
float MapTileSettings::defaultLat = 51.5003646652f; // @theBigBentern
float MapTileSettings::defaultLon = -0.1214328476f;
char MapTileSettings::prefix[MapTileSettings::PREFIX_SIZE] = "/maps";        // default map tile directory
char MapTileSettings::tileStyle[MapTileSettings::TILE_STYLE_SIZE] = "";      // { osm/, atlas/, world.pmtiles/, ...}
char MapTileSettings::tileDir[MapTileSettings::TILE_STYLE_SIZE] = "";        // tileStyle without the .pmtiles extension
char MapTileSettings::tileFormat[MapTileSettings::TILE_FORMAT_SIZE] = "png"; // use jpg or png
bool MapTileSettings::pmTiles = false;                                       // selected style is a .pmtiles archive
bool MapTileSettings::debug = false;                                         // draw tile frame and info
bool MapTileSettings::save = false;                                          // ok to save tile back to SD card
#ifdef MAP_TILES_GREY
bool MapTileSettings::colorTiles = false;
#else
bool MapTileSettings::colorTiles = true;
#endif

void MapTileSettings::setTileStyle(const char *p)
{
    copyBounded(tileStyle, TILE_STYLE_SIZE, p);
    std::string_view styleView(tileStyle);
    pmTiles = (styleView.size() >= PMTILES_EXTENSION_LEN &&
               styleView.compare(styleView.size() - PMTILES_EXTENSION_LEN, PMTILES_EXTENSION_LEN, PMTILES_EXTENSION) == 0);
    styleToDir(tileStyle, tileDir, TILE_STYLE_SIZE);
    appendSlash(tileStyle);
    appendSlash(tileDir);
}

void MapTileSettings::styleToDir(const char *style, char *dst, size_t dstSize)
{
    copyBounded(dst, dstSize, style);
    std::string_view dstView(dst);

    if (dstView.size() >= PMTILES_EXTENSION_LEN &&
        dstView.compare(dstView.size() - PMTILES_EXTENSION_LEN, PMTILES_EXTENSION_LEN, PMTILES_EXTENSION) == 0) {
        dst[dstView.size() - PMTILES_EXTENSION_LEN] = '\0';
        dstView = std::string_view(dst, dstView.size() - PMTILES_EXTENSION_LEN);
    }

    if (!dstView.empty() && dstView.back() == '/') {
        dst[dstView.size() - 1] = '\0';
    }
}