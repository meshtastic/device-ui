#pragma once

#include "graphics/map/GeoPoint.h"
#include "graphics/map/MapTile.h"
#include "graphics/map/TileService.h"
#include "lvgl.h"

#include <memory>
#include <unordered_map>

/**
 * Size independent map for x/y/z raster tiles
 * Draws the tiles and objects to the provided lvgl panel.
 */
class MapPanel
{
  public:
    MapPanel(lv_obj_t *panel, ITileService *s = nullptr);

    // draw callback for map objects: img, x, y, zoom
    using DrawCallback = std::function<void(uint32_t, uint16_t, uint16_t, uint8_t)>;

    // replace service for loading tiles
    void setTileService(ITileService *s);
    void setBackupService(ITileService *s);
    // zooming
    void setZoom(uint8_t zoom);
    // follow GPS
    void setLocked(bool lock);

    // positioning
    // set new home position according current
    void setHomePosition(void);
    void getHomeLocation(float &lat, float &lon) const;
    // set new home position
    void setHomeLocation(float lat, float lon);
    void setGpsPosition(float lat, float lon);
    void scroll(int16_t deltaX, int16_t deltaY, uint16_t fraction = 3); // -1, 0, +1, 1/3
    void moveHome(bool zoomDefault = true);
    void moveCurrent(void);
    // placing objects (uses *user_data as internal reference!)
    void add(uint32_t id, float lat, float lon, DrawCallback drawCB);
    void update(uint32_t id, float lat, float lon);
    void remove(uint32_t id);
    uint32_t getObjectsOnMap(void) { return objectsOnMap; }
    // images
    void setHomeLocationImage(lv_obj_t *img);
    void setGpsPositionImage(lv_obj_t *img);
    void setNoTileImage(const lv_image_dsc_t *img_src);
    // must be called for incremental drawing of all changes
    void task_handler(void);
    ~MapPanel(void);

  protected:
    struct MapObject {
        uint32_t id;
        GeoPoint point;
        DrawCallback draw;
    };

    void center(void);
    void redraw(void);
    void drawLocation(void);
    void drawObjects(void);
    void drawObject(const MapObject &obj);

    bool needsRedraw = false;
    bool locked = false; // map follows GPS location

    int16_t widthPixel;  // visible panel width
    int16_t heightPixel; // visible panel height
    int16_t xOffset;     // pixel offset x panel to upper left corner of (fully visible) upper left tile
    int16_t yOffset;     // pixel offset y panel to upper left corner of (fully visible) upper left tile
    uint32_t xStart;     // xTile number of (partly covered) upper left tile
    uint32_t yStart;     // yTile number of (partly covered) upper left tile
    uint8_t tilesX;      // number of (partly) visible tiles horizontal
    uint8_t tilesY;      // number of (partly) visible tiles vertical

    GeoPoint home;     // home location
    GeoPoint current;  // current (GPS) location
    GeoPoint scrolled; // current scrolled location (lat/lon in visible tile)

    lv_obj_t *panel;                   // lvgl parent panel object
    lv_obj_t *homeLocationImage;       // lvgl image of home position
    lv_obj_t *gpsPositionImage;        // lvgl image of actual position
    const lv_image_dsc_t *noTileImage; // lvgl image src for displaying "no tile"
    TileService *service;              // tile service provider
    uint32_t objectsOnMap;             // num of visible objcts on map
    std::unordered_map<uint32_t, std::unique_ptr<MapTile>> tiles;
    std::unordered_map<uintptr_t, std::unique_ptr<MapObject>> mapObjects;
};
