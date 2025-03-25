#include "graphics/map/MapTile.h"
#include "graphics/map/MapTileSettings.h"
#include "graphics/map/TileService.h"
#include "lvgl.h"
#include "util/ILog.h"

#include <assert.h>

LV_IMAGE_DECLARE(img_no_tile_image);

OSMTiles<lv_obj_t> *osm = nullptr;

MapTile::MapTile(uint32_t xTile, uint32_t yTile)
    : OSMTiles<lv_obj_t>::Tile(xTile, yTile, MapTileSettings::getZoomLevel()), img(nullptr), lbl(nullptr)
{
    // singleton should be already created
    assert(osm != nullptr);
}

/**
 * load map tile to display position x/y
 */
bool MapTile::load(lv_obj_t *p, int16_t posx, int16_t posy, const lv_image_dsc_t *img_src)
{
    x = posx;
    y = posy;
    if (!p)
        return false;
    removeImage();
    img = lv_image_create(p);
    lv_obj_set_pos(img, posx, posy);
    lv_obj_set_style_opa(img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(img, MapTileSettings::getTileSize(), MapTileSettings::getTileSize());
    if (MapTileSettings::getDebug()) {
        lv_obj_set_style_border_width(img, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
        lbl = lv_label_create(img);
        lv_obj_set_pos(lbl, 0, 0);
        lv_obj_set_size(lbl, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_text_color(lbl, lv_color_hex(0xff101010), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(lbl, "(%d/%d/%d) -> %d,%d", MapTileSettings::getZoomLevel(), xTile, yTile, posx, posy);
    }

    bool result = false;
#if LV_USE_FS_ARDUINO_SD
    // use lvgl built-in img loader
    char fname[128];
    fname[0] = LV_FS_ARDUINO_SD_LETTER;
    sprintf(&fname[1], ":%s/%s%d/%d/%d.%s", MapTileSettings::getPrefix(), MapTileSettings::getTileStyle(), zoomLevel, xTile,
            yTile, MapTileSettings::getTileFormat());
    ILOG_DEBUG("SD file: %s", fname);
    lv_image_set_src(img, fname);
    if (lv_image_get_src((lv_obj_t *)img)) {
        result = true;
    }
#endif
    // use configured TileService
    if (!result) {
        result = osm->load(*this, img);
        if (!result) {
            if (img_src) {
                lv_image_set_src((lv_obj_t *)img, img_src);
                lv_obj_set_style_opa(img, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
                if (!MapTileSettings::getDebug()) {
                    lv_obj_t *lbl = lv_label_create(img);
                    lv_obj_set_pos(lbl, 0, 50);
                    lv_obj_set_align(lbl, LV_ALIGN_CENTER);
                    lv_obj_set_size(lbl, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(lbl, lv_color_hex(0xff505050), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_fmt(lbl, "(%d/%d/%d)", MapTileSettings::getZoomLevel(), xTile, yTile);
                }
            }
        }
    }
    return result;
}

bool MapTile::move(int16_t posx, int16_t posy)
{
    x += posx;
    y += posy;
    if (img)
        lv_obj_set_pos(img, x, y);
    if (MapTileSettings::getDebug()) {
        lv_label_set_text_fmt(lbl, "(%d/%d/%d) -> %d,%d", MapTileSettings::getZoomLevel(), xTile, yTile, x, y);
    }
    return true;
}

void MapTile::removeImage(void)
{
    if (img) {
        // ILOG_DEBUG("remove image %d/%d", xTile, yTile);
        lv_obj_delete(img);
        img = nullptr;
    }
}

MapTile::~MapTile()
{
    // ILOG_DEBUG("MapTile::~MapTile %d/%d/%d", zoomLevel, xTile, yTile);
    removeImage();
}
