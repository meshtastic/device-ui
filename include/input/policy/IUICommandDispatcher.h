#pragma once

#include "input/policy/InputTypes.h"
#include "input/policy/PolicyDecision.h"

namespace input_policy
{

/**
 * Dispatches high-level UI commands emitted by the policy layer.
 * Implementations translate commands into concrete UI operations.
 */
class IUICommandDispatcher
{
  public:
    // Virtual destructor for safe polymorphic cleanup.
    virtual ~IUICommandDispatcher() = default;

    // Dispatches a UI command with optional payload data.
    virtual void dispatch(UICommand command, const CommandPayload &payload) = 0;
};

} // namespace input_policy
