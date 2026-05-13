#pragma once

#include "input/policy/IInputPolicy.h"

namespace input_policy
{

class FocusTraversalPolicy : public IInputPolicy
{
  public:
    PolicyDecision evaluate(const InputEvent &event, const InputContextSnapshot &context,
                            const InputCapabilities &capabilities) const override;
};

} // namespace input_policy
