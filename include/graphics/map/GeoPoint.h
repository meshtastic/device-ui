#pragma once

#include "Arduino.h"
#include "graphics/map/MapTileSettings.h"
#include "util/ILog.h"
#include <cmath>
#include <stdint.h>

#ifdef HIGH_PRECISION
#define FLOATING_POINT double
#else
#define FLOATING_POINT float
#endif

#ifdef ARCH_PORTDUINO
#ifndef PI
#define PI M_PIl
#endif
#endif

/**
 * Geographical coordinate that encompasses the OSM (raster) tile inverse mercator projection
 * (WSG84 Pseudo-Mercator EPSG:3857)
 */
class GeoPoint
{
  public:
    GeoPoint(uint32_t xtile, uint32_t ytile, uint8_t zoom) : xPos(0), yPos(0), xTile(xtile), yTile(ytile), zoomLevel(zoom)
    {
        // not used in any scenario yet; so comment out for now
        // reverse calculate from tile x/y/z back to lat/long (upper left corner 0/0)
        // auto n = 1 << zoom;
        // longitude = FLOATING_POINT(xTile) / n * FLOATING_POINT(360.0) - FLOATING_POINT(180.0);
        // latitude = FLOATING_POINT(180.0) / FLOATING_POINT(PI) *
        //           FLOATING_POINT(std::atan(std::sinh(FLOATING_POINT(PI) * (1.0 - 2.0 * FLOATING_POINT(yTile) / n))));
    }

    GeoPoint(float lat, float lon, uint8_t zoom) : latitude(lat), longitude(lon), zoomLevel(255) { setZoom(zoom); }

    void setZoom(uint8_t zoom)
    {
        if (zoom == zoomLevel)
            return;
        // calculate tile x/y/z and xPos/yPos from lat/long
        auto n = 1 << zoom;
        auto size = MapTileSettings::getTileSize();
        auto lat_rad = latitude * FLOATING_POINT(PI) / FLOATING_POINT(180.0);
        auto xRaw = (longitude + 180.0) / 360.0 * n;
        auto yRaw = (1.0 - std::log(std::tan(lat_rad) + (1.0 / std::cos(lat_rad))) / FLOATING_POINT(PI)) / 2.0 * n;
        xPos = uint16_t(xRaw * size) % size;
        yPos = uint16_t(yRaw * size) % size;
        xTile = uint32_t(xRaw);
        yTile = uint32_t(yRaw);
        zoomLevel = zoom;
        // ILOG_DEBUG("zoomed GeoPoint(%f, %f) %d/%d/%d (%d/%d)", latitude, longitude, zoom, xTile, yTile, xPos, yPos);
    }

    // move the GeoPoint position by pixels and recalculate the resulting tile and new lat/long
    void move(int16_t scrollX, int16_t scrollY)
    {
        // ILOG_DEBUG("move: tile offset: x: %d->%d, y: %d->%d pixel", xPos, xPos - scrollX, yPos, yPos - scrollY);
        auto size = MapTileSettings::getTileSize();
        xPos -= scrollX;
        yPos -= scrollY;
        if (xPos < 0) {
            xTile--;
            xPos += size;
        } else if (xPos >= size) {
            xTile++;
            xPos -= size;
        }
        if (yPos < 0) {
            yTile--;
            yPos += size;
        } else if (yPos >= size) {
            yTile++;
            yPos -= size;
        }

        auto n = 1 << zoomLevel;
        float lon = FLOATING_POINT(xTile) / n * FLOATING_POINT(360.0) - FLOATING_POINT(180.0);
        float lat = FLOATING_POINT(180.0) / FLOATING_POINT(PI) *
                    FLOATING_POINT(std::atan(std::sinh(FLOATING_POINT(PI) * (1.0 - 2.0 * FLOATING_POINT(yTile) / n))));
        float lon1 = FLOATING_POINT(xTile + 1) / n * FLOATING_POINT(360.0) - FLOATING_POINT(180.0);
        float lat1 = FLOATING_POINT(180.0) / FLOATING_POINT(PI) *
                     FLOATING_POINT(std::atan(std::sinh(FLOATING_POINT(PI) * (1.0 - 2.0 * FLOATING_POINT(yTile + 1) / n))));
        longitude = lon + (lon1 - lon) * (FLOATING_POINT(xPos) / FLOATING_POINT(size));
        latitude = lat + (lat1 - lat) * (FLOATING_POINT(yPos) / FLOATING_POINT(size));
    }

    // geographical coordinate
    float latitude;
    float longitude;
    // relative pixel position in tile
    int16_t xPos;
    int16_t yPos;
    // OSMtile raster index (inverse mercator projection)
    uint32_t xTile;
    uint32_t yTile;
    // level 0 (course) .. 18 (detail)
    uint8_t zoomLevel;
};

#ifdef UNIT_TEST
#include <doctest/doctest.h>

TEST_CASE("GeoPoint create000Tile")
{
    GeoPoint p(0U, 0U, 0);
    CHECK(p.zoomLevel == 0);
    CHECK(p.xTile == 0);
    CHECK(p.yTile == 0);
}

TEST_CASE("GeoPoint create000Location")
{
    GeoPoint p(0.0f, 0.0f, 0);
    CHECK(p.zoomLevel == 0);
    CHECK(p.xTile == 0);
    CHECK(p.yTile == 0);
}

TEST_CASE("GeoPoint locationMunichFrauenkirche")
{
    GeoPoint p(48.13867316206941f, 11.573006651462567f, 15);
    CHECK(p.zoomLevel == 15);
    CHECK(p.xTile == 17437);
    CHECK(p.yTile == 11371);
}

// TEST_CASE("GeoPoint reverseMunichFrauenkirche") {
//     GeoPoint p(17437U, 11371, 15);
//     CHECK(p.latitude == doctest::Approx(48.1440964f));
//     CHECK(p.longitude == doctest::Approx(11.5686035f));
// }

TEST_CASE("GeoPoint locationSanFrancisco")
{
    GeoPoint p(37.7749f, -122.4194f, 10);
    CHECK(p.zoomLevel == 10);
    CHECK(p.xTile == 163);
    CHECK(p.yTile == 395);
}

TEST_CASE("GeoPoint setZoom")
{
    GeoPoint point(37.7749f, -122.4194f, 10);
    point.setZoom(12);
    CHECK(point.zoomLevel == 12);
}

TEST_CASE("GeoPoint move")
{
    GeoPoint point(37.7749f, -122.4194f, 10);
    int16_t initialXPos = point.xPos;
    int16_t initialYPos = point.yPos;
    point.move(10, 20);
    CHECK(point.xPos == initialXPos - 10);
    CHECK(point.yPos == initialYPos - 20);
}
#endif