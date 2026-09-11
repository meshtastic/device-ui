#pragma once

#include "input/policy/IInputPolicy.h"

namespace input_policy
{

/**
 * Applies focus navigation remaps for directional input when appropriate.
 * This policy keeps traversal behavior context-aware and consistent.
 */
class FocusTraversalPolicy : public IInputPolicy
{
  public:
    // Evaluates whether the input should be remapped for focus traversal.
    PolicyDecision evaluate(const InputEvent &event, const InputContextSnapshot &context,
                            const InputCapabilities &capabilities) const override;
};

} // namespace input_policy
