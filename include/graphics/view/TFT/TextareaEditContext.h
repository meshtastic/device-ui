#pragma once

#include "lvgl.h"

namespace TextareaEditContext
{
void focus(lv_obj_t *textarea);
// Clear only this textarea's context; delayed exit events must not reset a new focus or panel.
void release(lv_obj_t *textarea);
void handleEvent(lv_event_t *event);
} // namespace TextareaEditContext
