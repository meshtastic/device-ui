#include "graphics/map/MapTileSettings.h"
#include <cstring>
#include <doctest/doctest.h>
#include <string>

TEST_CASE("MapTileSettings::setTileStyle")
{
    SUBCASE("keeps a long directory name intact and appends the slash")
    {
        // 19 chars, the longest name allowed by meshtastic_Map.style (#410)
        MapTileSettings::setTileStyle("OpenTopoMapOutdoors");
        CHECK(std::string(MapTileSettings::getTileStyle()) == "OpenTopoMapOutdoors/");
        CHECK(std::string(MapTileSettings::getTileDir()) == "OpenTopoMapOutdoors/");
    }

    SUBCASE("accepts a trailing slash and legacy archive extension")
    {
        MapTileSettings::setTileStyle("osm/");
        CHECK(std::string(MapTileSettings::getTileStyle()) == "osm/");
        CHECK(std::string(MapTileSettings::getTileDir()) == "osm/");

        MapTileSettings::setTileStyle("osm.pmtiles");
        CHECK(std::string(MapTileSettings::getTileStyle()) == "osm/");
        CHECK(std::string(MapTileSettings::getTileDir()) == "osm/");
    }

    SUBCASE("empty style")
    {
        MapTileSettings::setTileStyle("");
        CHECK(std::string(MapTileSettings::getTileStyle()).empty());
        CHECK(std::string(MapTileSettings::getTileDir()).empty());
    }
}

TEST_CASE("MapTileSettings::isValidStyleName")
{
    CHECK(MapTileSettings::isValidStyleName("osm"));
    CHECK(MapTileSettings::isValidStyleName("OpenTopoMapOutdoors"));
    CHECK(MapTileSettings::isValidStyleName("OpenTopoMapOutdoors/"));
    CHECK(MapTileSettings::isValidStyleName("osm.pmtiles"));
    CHECK_FALSE(MapTileSettings::isValidStyleName("OpenTopoMapOutdoors1"));
    CHECK_FALSE(MapTileSettings::isValidStyleName(""));
    CHECK_FALSE(MapTileSettings::isValidStyleName(nullptr));
}
