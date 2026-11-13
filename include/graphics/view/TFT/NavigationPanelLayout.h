#pragma once

#include "graphics/driver/DisplayDriverConfig.h"
#include "lvgl.h"
#include <initializer_list>

namespace NavigationPanelLayout
{
// Call after sizing the navigation buttons so the layout uses their actual geometry.
void configure(lv_obj_t *panel, std::initializer_list<lv_obj_t *> buttons, DisplayDriverConfig::scrollbar_mode_t scrollbarMode);
} // namespace NavigationPanelLayout
