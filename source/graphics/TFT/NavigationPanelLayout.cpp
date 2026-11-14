#include "graphics/view/TFT/NavigationPanelLayout.h"

void NavigationPanelLayout::configure(lv_obj_t *panel, std::initializer_list<lv_obj_t *> buttons,
                                      DisplayDriverConfig::scrollbar_mode_t scrollbarMode)
{
    lv_obj_update_layout(panel);
    const bool vertical = (lv_obj_get_style_flex_flow(panel, LV_PART_MAIN) & LV_FLEX_COLUMN) != 0;
    int32_t requiredExtent = 0;
    int32_t visibleButtons = 0;
    for (lv_obj_t *button : buttons) {
        if (!lv_obj_has_flag(button, LV_OBJ_FLAG_HIDDEN)) {
            requiredExtent += vertical ? lv_obj_get_height(button) : lv_obj_get_width(button);
            ++visibleButtons;
        }
    }
    if (visibleButtons > 1)
        requiredExtent += (visibleButtons - 1) * (vertical ? lv_obj_get_style_pad_row(panel, LV_PART_MAIN)
                                                           : lv_obj_get_style_pad_column(panel, LV_PART_MAIN));

    const int32_t availableExtent = vertical ? lv_obj_get_content_height(panel) : lv_obj_get_content_width(panel);
    const bool overflow = requiredExtent > availableExtent;
    lv_obj_set_flag(panel, LV_OBJ_FLAG_SCROLLABLE, overflow);
    lv_obj_set_scroll_dir(panel, vertical ? LV_DIR_VER : LV_DIR_HOR);
    lv_obj_set_flex_align(panel, overflow ? LV_FLEX_ALIGN_START : LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    for (lv_obj_t *button : buttons)
        lv_obj_set_flag(button, LV_OBJ_FLAG_SCROLL_ON_FOCUS, overflow);

    lv_scrollbar_mode_t mode = LV_SCROLLBAR_MODE_AUTO;
    if (scrollbarMode == DisplayDriverConfig::scrollbar_mode_t::ON)
        mode = LV_SCROLLBAR_MODE_ON;
    else if (scrollbarMode == DisplayDriverConfig::scrollbar_mode_t::OFF)
        mode = LV_SCROLLBAR_MODE_OFF;
    lv_obj_set_scrollbar_mode(panel, mode);
}
