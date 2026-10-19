#include "graphics/map/OSMTiles.h"
#include "lvgl.h"
#include "stdint.h"

/**
 * Map tile graphical element. Creates and loads a tile image using the OSMTile provider.
 */
class MapTile : public OSMTiles<lv_obj_t>::Tile
{
  public:
    MapTile(uint32_t xTile, uint32_t yTile);
    bool load(lv_obj_t *p, int16_t posx, int16_t posy, const lv_image_dsc_t *noTile);
    // apply a decoded image from the background worker (UI task only)
    void applyImage(lv_image_dsc_t *img_dsc);
    bool move(int16_t posx, int16_t posy);
    void unload(void);

    int16_t getX(void) const { return x; }
    int16_t getY(void) const { return y; }

    void removeImage(void);
    ~MapTile();

    bool isPending = false; // enqueued in AsyncTileLoader, result not yet applied

  protected:
    int16_t x;     // x-pos in parent panel
    int16_t y;     // y-pos in parent panel
    lv_obj_t *img; // lvgl tile image
    lv_obj_t *lbl; // debug label
};