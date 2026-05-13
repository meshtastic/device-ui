#pragma once

#include "input/policy/IActionBindingResolver.h"

namespace input_policy
{

class DefaultBindingResolver : public IActionBindingResolver
{
  public:
    InputAction resolveAction(const InputEvent &event, const InputContextSnapshot &context,
                              const InputCapabilities &capabilities) const override;
};

} // namespace input_policy
