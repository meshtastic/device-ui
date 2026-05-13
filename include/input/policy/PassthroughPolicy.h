#pragma once

#include "input/policy/IInputPolicy.h"

namespace input_policy
{

class PassthroughPolicy : public IInputPolicy
{
  public:
    PolicyDecision evaluate(const InputEvent &event, const InputContextSnapshot &context,
                            const InputCapabilities &capabilities) const override;
};

} // namespace input_policy
