#pragma once

#include "input/policy/InputTypes.h"
#include "input/policy/PolicyDecision.h"

namespace input_policy
{

class IUICommandDispatcher
{
  public:
    virtual ~IUICommandDispatcher() = default;

    virtual void dispatch(UICommand command, const CommandPayload &payload) = 0;
};

} // namespace input_policy
