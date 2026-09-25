#pragma once

#include "input/policy/IInputPolicy.h"

namespace input_policy
{

/**
 * Provides a terminal fallback policy for unhandled input.
 * It forwards events without remapping when no other policy acts.
 */
class PassthroughPolicy : public IInputPolicy
{
  public:
    // Returns a pass-through decision for the incoming event.
    PolicyDecision evaluate(const InputEvent &event, const InputContextSnapshot &context,
                            const InputCapabilities &capabilities) const override;
};

} // namespace input_policy
