#pragma once

#include "lvgl.h"

/**
 * Keypad behaviour for dropdowns, similar to encoder edit mode:
 * ENTER opens the list and commits the selection, UP/DOWN move inside the open list,
 * ESC closes only the list, LEFT/RIGHT never open it.
 * The open list is published as FocusSemantic::Dropdown + edit mode to the input policy context.
 */
namespace DropdownKeyNavigation
{
// Registers the handler on every dropdown in the tree below root; call once per tree.
void attach(lv_obj_t *root);
} // namespace DropdownKeyNavigation
