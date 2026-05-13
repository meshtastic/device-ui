#pragma once

#include "input/policy/InputTypes.h"

namespace input_policy
{

class IActionBindingResolver
{
  public:
    virtual ~IActionBindingResolver() = default;

    virtual InputAction resolveAction(const InputEvent &event, const InputContextSnapshot &context,
                                      const InputCapabilities &capabilities) const = 0;
};

} // namespace input_policy
