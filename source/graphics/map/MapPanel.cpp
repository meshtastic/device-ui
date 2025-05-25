#include "graphics/map/MapPanel.h"
#include "graphics/map/MapTileSettings.h"
#include "screens.h"
#include "util/ILog.h"
#include <assert.h>

MapPanel::MapPanel(lv_obj_t *p, ITileService *s)
    : home(GeoPoint(MapTileSettings::getDefaultLat(), MapTileSettings::getDefaultLon(), MapTileSettings::getZoomLevel())),
      current(home), scrolled(home), panel(p), homeLocationImage(nullptr), gpsPositionImage(nullptr), noTileImage(nullptr),
      service(new TileService(s)), objectsOnMap(0)
{
    assert(lv_obj_is_valid(p));
    lv_obj_update_layout(panel);
    widthPixel = lv_obj_get_width(panel);
    heightPixel = lv_obj_get_height(panel);
    ILOG_DEBUG("panel size: %dx%d", widthPixel, heightPixel);

    extern OSMTiles<lv_obj_t> *osm;
    osm = OSMTiles<lv_obj_t>::create([this](const char *name, void *img) -> bool { return service->load(name, img); });

    center();
}

/**
 * incremental redraw function; draw one row of tiles per invocation
 */
void MapPanel::redraw(void)
{
    static bool done = false;
    static int16_t x = INT16_MAX;
    static int16_t y = INT16_MAX;

    if (needsRedraw) {
        needsRedraw = false;
        done = false;
        x = 0;
        y = 0;
        tiles.clear();
    }

    if (done)
        return;

    for (int i = 0; i < tilesY; i++) {
        if (x < tilesX && y < tilesY) {
            int16_t size = MapTileSettings::getTileSize();
            uint32_t hash = ((xStart + x) << 16) | (yStart + y);
            tiles[hash] = std::move(std::unique_ptr<MapTile>(new MapTile(xStart + x, yStart + y)));
            tiles[hash]->load(panel, x * size + xOffset, y * size + yOffset, noTileImage);
            x++;
        } else {
            if (y < tilesY) {
                x = 0;
                y++;
                if (y >= tilesY) {
                    done = true;
                    drawLocation();
                    drawObjects();
                }
            }
        }
    }
}

/**
 * draw a pin/pos at home location and current GPS location
 */
void MapPanel::drawLocation(void)
{
    if (gpsPositionImage) {
        auto it = tiles.find(current.xTile << 16 | current.yTile);
        if (it != tiles.end()) {
            MapTile &tile = *it->second;
            lv_obj_set_pos(gpsPositionImage, current.xPos + tile.getX() - 4, current.yPos + tile.getY() - 10);
            lv_obj_clear_flag(gpsPositionImage, LV_OBJ_FLAG_HIDDEN);
            lv_obj_move_foreground(gpsPositionImage);
        } else {
            lv_obj_add_flag(gpsPositionImage, LV_OBJ_FLAG_HIDDEN);
        }
    }
    if (homeLocationImage) {
        auto it = tiles.find(home.xTile << 16 | home.yTile);
        if (it != tiles.end()) {
            MapTile &tile = *it->second;
            lv_obj_set_pos(homeLocationImage, home.xPos + tile.getX() - 4, home.yPos + tile.getY() - 10);
            lv_obj_clear_flag(homeLocationImage, LV_OBJ_FLAG_HIDDEN);
            lv_obj_move_foreground(homeLocationImage);
        } else {
            lv_obj_add_flag(homeLocationImage, LV_OBJ_FLAG_HIDDEN);
        }
    }
    char buf[30];
    sprintf(buf, "%0.4f %0.4f", scrolled.latitude, scrolled.longitude);
    lv_label_set_text(objects.map_location_label, buf);
    lv_obj_move_foreground(objects.map_location_label);
}

/**
 * draw all added objects onto map
 * TODO: allow incremental drawing via task_handler
 */
void MapPanel::drawObjects(void)
{
    objectsOnMap = 0;
    for (auto &it : mapObjects) {
        // check if object's tile is visible
        MapObject &obj = *it.second;
        obj.point.setZoom(MapTileSettings::getZoomLevel());
        drawObject(obj);
    }
}

void MapPanel::drawObject(const MapObject &obj)
{
    if (obj.draw) {
        auto tileIt = tiles.find(obj.point.xTile << 16 | obj.point.yTile);
        if (tileIt != tiles.end()) {
            MapTile &tile = *tileIt->second;
            objectsOnMap++; // FIXME: counts also objects that are clipped away
            obj.draw(obj.id, tile.getX() + obj.point.xPos, tile.getY() + obj.point.yPos, MapTileSettings::getZoomLevel());
        } else {
            obj.draw(obj.id, 0, 0, 0); // hide object
        }
    }
}

/**
 * center map at current (scrolled) location
 */
void MapPanel::center(void)
{
    int16_t size = MapTileSettings::getTileSize();
    int16_t xpos = widthPixel / 2 - scrolled.xPos;
    int16_t ypos = heightPixel / 2 - scrolled.yPos;
    xOffset = xpos % size - size;
    yOffset = ypos % size - size;
    tilesX = (widthPixel - xOffset + size - 2) / size;
    tilesY = (heightPixel - yOffset + size - 2) / size;
    xStart = scrolled.xTile - (xpos / size + 1);
    yStart = scrolled.yTile - (ypos / size + 1);
    needsRedraw = true;
}

void MapPanel::setTileService(ITileService *s)
{
    service->setService(s);
}

void MapPanel::setBackupService(ITileService *s)
{
    service->setBackupService(s);
}

void MapPanel::setHomePosition(void)
{
    home = scrolled;
    MapTileSettings::setDefaultZoom(MapTileSettings::getZoomLevel());
    drawLocation();
}

void MapPanel::getHomeLocation(float &lat, float &lon) const
{
    lat = home.latitude;
    lon = home.longitude;
}

void MapPanel::setHomeLocation(float lat, float lon)
{
    home = GeoPoint(lat, lon, MapTileSettings::getZoomLevel());
    current = home;
    scrolled = home;
    center();
}

void MapPanel::setGpsPosition(float lat, float lon)
{
    current = GeoPoint(lat, lon, MapTileSettings::getZoomLevel());
    if (locked) {
        scrolled = current;
        center();
    } else {
        drawLocation();
    }
}

void MapPanel::moveHome(bool zoomDefault)
{
    setZoom(MapTileSettings::getDefaultZoom());
    scrolled = home;
    center();
}

void MapPanel::moveCurrent(void)
{
    ILOG_DEBUG("moveCurrent: pos=%0.4f, %0.4f (%d/%d/%d)", current.latitude, current.longitude, current.zoomLevel, current.xTile,
               current.yTile);
    scrolled = current;
    center();
}

void MapPanel::setZoom(uint8_t zoom)
{
    if (zoom > 1 && zoom <= 20) {
        ILOG_DEBUG("setZoom: %d", zoom);
        MapTileSettings::setZoomLevel(zoom);
        home.setZoom(zoom);
        current.setZoom(zoom);
        scrolled.setZoom(zoom);
        center();
    }
}

void MapPanel::setLocked(bool lock)
{
    locked = lock;
    if (lock)
        moveCurrent();
}

/**
 * move map in direction x/y -1, 0, 1 by fraction of panel width but not more than tile size
 */
void MapPanel::scroll(int16_t deltaX, int16_t deltaY, uint16_t fraction)
{
    int16_t size = MapTileSettings::getTileSize();
    int16_t scrollX, scrollY;

    // scroll by fraction but not more(!) than a tile size
    if (widthPixel / fraction > size)
        scrollX = deltaX * size;
    else
        scrollX = deltaX * widthPixel / fraction;

    if (heightPixel / fraction > size)
        scrollY = deltaY * size;
    else
        scrollY = deltaY * heightPixel / fraction;

    // first check if we are already at the edge of the world tile map
    // TODO: allow sub-tile movement to the exact border (adapt scrollX/scrollY)
    if ((xStart == 0 && scrollX > 0) || (yStart == 0 && scrollY > 0) ||
        (xStart + tilesX > (uint32_t)pow(2, MapTileSettings::getZoomLevel()) && scrollX < 0) ||
        (yStart + tilesY > (uint32_t)pow(2, MapTileSettings::getZoomLevel()) && scrollY < 0)) {
        return;
    }

    // check if the scrolling requires new tiles at the beginning row or column
    auto sit = tiles.find((xStart << 16) | yStart);
    if (sit != tiles.end()) {
        MapTile &tile00 = *sit->second;
        if (tile00.getX() + scrollX > 0) {
            if (xStart == 0)
                return;
            xStart--;
            tilesX++;
        }
        if (tile00.getY() + scrollY > 0) {
            if (yStart == 0)
                return;
            yStart--;
            tilesY++;
        }
    } else {
        ILOG_ERROR("scroll: start tile %d/%d missing", xStart, yStart);
        return;
    }
    // check if scrolling requires new tiles at the ending row or column
    auto eit = tiles.find(((xStart + tilesX - 1) << 16) | (yStart + tilesY - 1));
    if (eit != tiles.end()) {
        MapTile &tileNN = *eit->second;
        if (tileNN.getX() + scrollX < widthPixel - size) {
            tilesX++;
        }
        if (tileNN.getY() + scrollY < heightPixel - size) {
            tilesY++;
        }
    } else {
        ILOG_ERROR("scroll: end tile %d/%d missing", xStart + tilesX - 1, yStart + tilesY - 1);
        return;
    }

    // calculate new x/y offset of the first entirely visible tile
    xOffset += scrollX;
    yOffset += scrollY;

    if (xOffset <= -size)
        xOffset += size;
    if (yOffset <= -size)
        yOffset += size;
    if (xOffset >= size)
        xOffset -= size;
    if (yOffset >= size)
        yOffset -= size;

    // remember if scroll changes start or number of tiles
    bool changeXstart = false;
    bool changeYstart = false;
    bool changeXtiles = false;
    bool changeYtiles = false;

    ILOG_DEBUG("load tiles (%d*%d), from %d/%d/%d to %d/%d/%d", tilesX, tilesY, MapTileSettings::getZoomLevel(), xStart, yStart,
               MapTileSettings::getZoomLevel(), xStart + tilesX - 1, yStart + tilesY - 1);

    // now loop over the extended square of tiles, create, move and delete as required
    for (int x = 0; x < tilesX; x++) {
        for (int y = 0; y < tilesY; y++) {
            uint32_t hash = ((xStart + x) << 16) | (yStart + y);
            if (tiles.find(hash) == tiles.end()) {
                // create new tiles at panel pos x/y
                int16_t xpos = x * size + xOffset;
                int16_t ypos = y * size + yOffset;
                if ((x == 0 && xpos >= 0) || (x == tilesX - 1 && xpos >= widthPixel)) {
                    xpos -= size;
                    xOffset -= size;
                }
                if ((y == 0 && ypos >= 0) || (y == tilesY - 1 && ypos >= heightPixel)) {
                    ypos -= size;
                    yOffset -= size;
                }
                tiles[hash] = std::move(std::unique_ptr<MapTile>(new MapTile(xStart + x, yStart + y)));
                tiles[hash]->load(panel, xpos, ypos, noTileImage);
            } else {
                // check if tile is still visible after scrolling
                MapTile &tile = *tiles[hash];
                int16_t newX = tile.getX() + scrollX;
                int16_t newY = tile.getY() + scrollY;
                if (newX >= -size && newX < widthPixel) {
                    if (newY >= -size && newY < heightPixel) {
                        tile.move(scrollX, scrollY);
                    } else {
                        tiles.erase(hash);
                        changeYtiles = true;
                        if (newY < -size)
                            changeYstart = true;
                    }
                } else {
                    tiles.erase(hash);
                    changeXtiles = true;
                    if (newX < -size)
                        changeXstart = true;
                }
            }
        }
    }

    // correct x/y offset of the first entirely visible tile
    if (xOffset <= -size)
        xOffset += size;
    if (yOffset <= -size)
        yOffset += size;
    if (xOffset >= size)
        xOffset -= size;
    if (yOffset >= size)
        yOffset -= size;

    // correct tile square size after scroll
    // note: works only if scroll step < tile size
    if (changeXstart)
        xStart++;
    if (changeYstart)
        yStart++;
    if (changeXtiles)
        tilesX--;
    if (changeYtiles)
        tilesY--;

    if (tilesX * tilesY != tiles.size())
        ILOG_ERROR("tile size mismatch: %d*%d != %d", tilesX, tilesY, tiles.size());

    scrolled.move(scrollX, scrollY);
    drawLocation();
    drawObjects();
}

void MapPanel::add(uint32_t id, float lat, float lon, DrawCallback drawCB)
{
    auto it = mapObjects.find(id);
    if (it != mapObjects.end()) {
        it->second->point = GeoPoint(lat, lon, MapTileSettings::getZoomLevel());
        drawObject(*it->second);
    } else {
        auto object = std::unique_ptr<MapObject>(
            new MapObject({.id = id, .point = GeoPoint(lat, lon, MapTileSettings::getZoomLevel()), .draw = drawCB}));
        drawObject(*object);
        mapObjects.emplace(id, std::move(object));
    }
}

void MapPanel::update(uint32_t id, float lat, float lon)
{
    auto it = mapObjects.find(id);
    if (it != mapObjects.end()) {
        it->second->point = GeoPoint(lat, lon, MapTileSettings::getZoomLevel());
    }
    drawObject(*it->second);
}

void MapPanel::remove(uint32_t id)
{
    mapObjects.erase(id);
    objectsOnMap--;
}

void MapPanel::setHomeLocationImage(lv_obj_t *img)
{
    homeLocationImage = img;
}

void MapPanel::setGpsPositionImage(lv_obj_t *img)
{
    gpsPositionImage = img;
}

void MapPanel::setNoTileImage(const lv_image_dsc_t *img_src)
{
    noTileImage = img_src;
}

void MapPanel::task_handler(void)
{
    redraw();
}

MapPanel::~MapPanel(void)
{
    delete service;
}
