#pragma once

#include "input/policy/IActionBindingResolver.h"

namespace input_policy
{

/**
 * Provides the default mapping from LVGL key events to input actions.
 * This implementation centralizes standard key behavior across sources.
 */
class DefaultBindingResolver : public IActionBindingResolver
{
  public:
    // Resolves an action from raw key input using the default mapping table.
    InputAction resolveAction(const InputEvent &event, const InputContextSnapshot &context,
                              const InputCapabilities &capabilities) const override;
};

} // namespace input_policy
