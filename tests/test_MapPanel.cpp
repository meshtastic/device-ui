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
    int16_t getWidthPixel(void) const { return widthPixel; }
    int16_t getHeightPixel(void) const { return heightPixel; }
    bool redrawRequested(void) const { return needsRedraw; }
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

TEST_CASE("MapPanel::resize")
{
    if (!lv_is_initialized())
        lv_init();
    lv_display_t *disp = lv_display_get_default();
    if (!disp)
        disp = lv_display_create(320, 240);

    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_t *outerPanel = lv_obj_create(screen);
    lv_obj_set_size(outerPanel, LV_PCT(88), LV_PCT(90));
    lv_obj_t *panel = lv_obj_create(outerPanel);
    lv_obj_set_size(panel, LV_PCT(100), LV_PCT(100));
    lv_obj_update_layout(panel);

    {
        TestMapPanel mapPanel(panel, NULL);
        CHECK(mapPanel.getWidthPixel() == (int16_t)lv_obj_get_width(panel));
        CHECK(mapPanel.getHeightPixel() == (int16_t)lv_obj_get_height(panel));

        // consume the redraw the constructor requested
        mapPanel.redraw();
        CHECK_FALSE(mapPanel.redrawRequested());

        SUBCASE("A larger display requests a redraw with the new panel size")
        {
            lv_display_set_resolution(disp, 640, 480);
            lv_obj_update_layout(panel);

            CHECK(mapPanel.redrawRequested());
            CHECK(mapPanel.getWidthPixel() == (int16_t)lv_obj_get_width(panel));
            CHECK(mapPanel.getHeightPixel() == (int16_t)lv_obj_get_height(panel));
            lv_display_set_resolution(disp, 320, 240);
        }

        SUBCASE("A resized child of the panel does not request a redraw")
        {
            lv_obj_t *child = lv_obj_create(panel);
            lv_obj_set_event_bubble(child, true);
            lv_obj_set_size(child, 10, 10);
            lv_obj_update_layout(child);

            CHECK_FALSE(mapPanel.redrawRequested());
            lv_obj_delete(child);
        }
    }

    lv_obj_delete(screen);
}
