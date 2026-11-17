#include "graphics/view/TFT/NavigationPanelLayout.h"
#include <array>
#include <doctest/doctest.h>

namespace
{
struct NavigationFixture {
    lv_obj_t *screen;
    lv_obj_t *panel;
    std::array<lv_obj_t *, 6> buttons;

    NavigationFixture()
    {
        if (!lv_is_initialized())
            lv_init();
        if (!lv_display_get_default())
            lv_display_create(320, 240);
        screen = lv_obj_create(nullptr);
        panel = lv_obj_create(screen);
        lv_obj_set_size(panel, 38, 170);
        lv_obj_set_style_border_width(panel, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_top(panel, 1, LV_PART_MAIN);
        lv_obj_set_style_pad_bottom(panel, 1, LV_PART_MAIN);
        lv_obj_set_style_pad_left(panel, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_right(panel, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_column(panel, 4, LV_PART_MAIN);
        lv_obj_set_style_pad_row(panel, 4, LV_PART_MAIN);
        lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
        for (lv_obj_t *&button : buttons) {
            button = lv_button_create(panel);
            lv_obj_set_size(button, 36, 36);
        }
    }

    ~NavigationFixture() { lv_obj_delete(screen); }

    void configure(DisplayDriverConfig::scrollbar_mode_t mode = DisplayDriverConfig::scrollbar_mode_t::AUTO)
    {
        NavigationPanelLayout::configure(panel, {buttons[0], buttons[1], buttons[2], buttons[3], buttons[4], buttons[5]}, mode);
    }
};
} // namespace

TEST_CASE("navigation detects actual overflow while scrollbar visibility remains configurable")
{
    NavigationFixture f;
    f.configure(DisplayDriverConfig::scrollbar_mode_t::OFF);
    CHECK(lv_obj_has_flag(f.panel, LV_OBJ_FLAG_SCROLLABLE));
    CHECK(lv_obj_get_scrollbar_mode(f.panel) == LV_SCROLLBAR_MODE_OFF);
    for (lv_obj_t *button : f.buttons)
        CHECK(lv_obj_has_flag(button, LV_OBJ_FLAG_SCROLL_ON_FOCUS));

    f.configure(DisplayDriverConfig::scrollbar_mode_t::ON);
    CHECK(lv_obj_get_scrollbar_mode(f.panel) == LV_SCROLLBAR_MODE_ON);
    f.configure();
    CHECK(lv_obj_get_scrollbar_mode(f.panel) == LV_SCROLLBAR_MODE_AUTO);
}

TEST_CASE("navigation overflow follows resized and hidden buttons rather than a fixed screen height")
{
    NavigationFixture f;
    lv_obj_set_height(f.panel, 220);
    f.configure();
    CHECK(lv_obj_has_flag(f.panel, LV_OBJ_FLAG_SCROLLABLE));

    lv_obj_add_flag(f.buttons.back(), LV_OBJ_FLAG_HIDDEN);
    f.configure();
    CHECK_FALSE(lv_obj_has_flag(f.panel, LV_OBJ_FLAG_SCROLLABLE));

    lv_obj_remove_flag(f.buttons.back(), LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_height(f.panel, 480);
    for (lv_obj_t *button : f.buttons)
        lv_obj_set_height(button, 72);
    f.configure();
    CHECK_FALSE(lv_obj_has_flag(f.panel, LV_OBJ_FLAG_SCROLLABLE));

    lv_obj_set_height(f.buttons.back(), 120);
    f.configure();
    CHECK(lv_obj_has_flag(f.panel, LV_OBJ_FLAG_SCROLLABLE));
}

TEST_CASE("horizontal navigation preserves its layout and scrolls only when its width overflows")
{
    NavigationFixture f;
    lv_obj_set_flex_flow(f.panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(f.panel, 240, 38);
    f.configure();
    CHECK_FALSE(lv_obj_has_flag(f.panel, LV_OBJ_FLAG_SCROLLABLE));
    CHECK(lv_obj_get_scroll_dir(f.panel) == LV_DIR_HOR);
    lv_obj_set_width(f.panel, 200);
    f.configure();
    CHECK(lv_obj_has_flag(f.panel, LV_OBJ_FLAG_SCROLLABLE));
    CHECK(lv_obj_get_style_flex_flow(f.panel, LV_PART_MAIN) == LV_FLEX_FLOW_ROW);
}
