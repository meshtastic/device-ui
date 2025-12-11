#pragma once

#include "graphics/map/GeoPoint.h"
#include "graphics/map/MapTileSettings.h"
#include <functional>
#include <stdint.h>

/**
 * Generic OSM raster tile loader (singleton class).
 * Of course can also load other raster styles than just osm, lol.
 */
template <class IMG> class OSMTiles
{
  public:
    struct Tile : public GeoPoint {
        Tile(float lat, float lon, uint8_t zoom) : GeoPoint(lat, lon, zoom) { }
        Tile(uint32_t xTile, uint32_t yTile, uint8_t zoom) : GeoPoint(xTile, yTile, zoom) { }
    };

    // create instance of this class and provide cb function for loading images
    static OSMTiles *create(std::function<bool(uint32_t x, uint32_t y, uint32_t z, IMG *)> cb);

    // load tile via callback provider
    bool load(OSMTiles::Tile &tile, IMG *img)
    {
        return loadcb(tile.xTile, tile.yTile, tile.zoomLevel, img);
    }

  protected:
    bool load(const GeoPoint &tile, IMG *img)
    {
        return loadcb(tile.xTile, tile.yTile, tile.zoomLevel, img);
    }

  private:
    OSMTiles() = default;
    static std::function<bool(uint32_t x, uint32_t y, uint32_t z, IMG *)> loadcb;
};

template <class IMG> OSMTiles<IMG> *OSMTiles<IMG>::create(std::function<bool(uint32_t x, uint32_t y, uint32_t z, IMG *)> cb)
{
    loadcb = cb;
    return new OSMTiles;
}

template <class IMG> std::function<bool(uint32_t x, uint32_t y, uint32_t z, IMG *)> OSMTiles<IMG>::loadcb = nullptr;
