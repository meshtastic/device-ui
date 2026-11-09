#pragma once

#include "lvgl.h"

inline constexpr uint32_t LV_KEY_CUSTOM_DIACRITIC = 0x110;

namespace DiacriticRotation
{
void rotatePreviousCharacter(lv_obj_t *textarea);
void attach(lv_obj_t *root);
}