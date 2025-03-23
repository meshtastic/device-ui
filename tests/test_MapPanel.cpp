#include "graphics/map/MapPanel.h"
#include <doctest/doctest.h>

class TestMapPanel : public MapPanel
{
  public:
    TestMapPanel(lv_obj_t *p, ITileService *s = nullptr) : MapPanel(p, s) {}
    void setHome(GeoPoint &p) { home = p; }
    void setCurrent(GeoPoint &p) { current = p; }
    void setScrolled(GeoPoint &p) { scrolled = p; }
    void redraw(void) { MapPanel::redraw(); }
    void center(void) { MapPanel::center(); }
    void setWidthPixel(int16_t width) { widthPixel = width; }
    void setHeightPixel(int16_t height) { heightPixel = height; }
    void setXStart(uint32_t x) { xStart = x; }
    void setYStart(uint32_t y) { yStart = y; }
    void setTilesX(uint32_t x) { tilesX = x; }
    void setTilesY(uint32_t y) { tilesY = y; }
    void setXOffset(int16_t x) { xOffset = x; }
    void setYOffset(int16_t y) { yOffset = y; }
    uint32_t getXStart() const { return xStart; }
    uint32_t getYStart() const { return yStart; }
    uint8_t getTilesX() const { return tilesX; }
    uint8_t getTilesY() const { return tilesY; }
    int16_t getXOffset() const { return xOffset; }
    int16_t getYOffset() const { return yOffset; }

    void redrawAll(void)
    {
        for (int i = 0; i <= tilesX; i++) {
            MapPanel::redraw();
        }
    }
};

TEST_CASE("MapPanel::scroll")
{
    TestMapPanel mapPanel(nullptr, nullptr);

    mapPanel.redrawAll();
    mapPanel.printTiles();

    SUBCASE("Scroll left by one pixel")
    {
        for (int i = 0; i < 256; i++) {
            CHECK(mapPanel.scroll(-1, 0, 256));
            mapPanel.printTiles();
        }
    }

    SUBCASE("Scroll right by one pixel")
    {
        for (int i = 0; i < 256; i++) {
            CHECK(mapPanel.scroll(1, 0, 256));
            mapPanel.printTiles();
        }
    }

    SUBCASE("Scroll up by one pixel")
    {
        for (int i = 0; i < 256; i++) {
            CHECK(mapPanel.scroll(0, -1, 256));
            mapPanel.printTiles();
        }
    }

    SUBCASE("Scroll down by one pixel")
    {
        for (int i = 0; i < 256; i++) {
            CHECK(mapPanel.scroll(0, 1, 256));
            mapPanel.printTiles();
        }
    }
}
