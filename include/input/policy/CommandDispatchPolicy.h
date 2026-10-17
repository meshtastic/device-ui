#pragma once

#include "input/policy/IInputPolicy.h"

namespace input_policy
{

/**
 * Emits UI commands for command-like input actions such as Home and Map.
 */
class CommandDispatchPolicy : public IInputPolicy
{
  public:
    PolicyDecision evaluate(const InputEvent &event, const InputContextSnapshot &context,
                            const InputCapabilities &capabilities) const override;
};

} // namespace input_policy