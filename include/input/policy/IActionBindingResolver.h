#pragma once

#include "input/policy/InputTypes.h"

namespace input_policy
{

/**
 * Maps low-level input events to high-level semantic actions.
 * This keeps keycode details isolated from policy logic.
 */
class IActionBindingResolver
{
  public:
    // Virtual destructor for safe polymorphic cleanup.
    virtual ~IActionBindingResolver() = default;

    // Resolves a high-level action from a raw input event and context.
    virtual InputAction resolveAction(const InputEvent &event, const InputContextSnapshot &context,
                                      const InputCapabilities &capabilities) const = 0;
};

} // namespace input_policy
