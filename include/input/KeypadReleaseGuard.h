#pragma once

#include "lvgl.h"

/**
 * LVGL's keypad indev sends RELEASED/CLICKED of a long press to the object focused at release time.
 * When a LONG_PRESSED handler moves the focus (e.g. by switching panels), the release would hit the newly
 * focused object. This guard routes it to the long-pressed object instead, like a pointer indev does.
 */
namespace KeypadReleaseGuard
{
// Registers the guard on a keypad indev; repeated calls for the same indev are ignored.
void attach(lv_indev_t *keypad);
} // namespace KeypadReleaseGuard
