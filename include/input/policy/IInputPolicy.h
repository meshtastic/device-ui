#pragma once

#include "input/policy/InputTypes.h"
#include "input/policy/PolicyDecision.h"

namespace input_policy
{

class IInputPolicy
{
  public:
    virtual ~IInputPolicy() = default;

    virtual PolicyDecision evaluate(const InputEvent &event, const InputContextSnapshot &context,
                                    const InputCapabilities &capabilities) const = 0;
};

} // namespace input_policy
