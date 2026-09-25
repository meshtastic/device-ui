#pragma once

#include "input/policy/InputTypes.h"
#include "input/policy/PolicyDecision.h"

namespace input_policy
{

/**
 * Defines the contract for input policy evaluators in the pipeline.
 * Each implementation inspects context and capabilities to return a decision.
 */
class IInputPolicy
{
  public:
    // Virtual destructor for safe polymorphic cleanup.
    virtual ~IInputPolicy() = default;

    // Evaluates an input event and returns the corresponding policy decision.
    virtual PolicyDecision evaluate(const InputEvent &event, const InputContextSnapshot &context,
                                    const InputCapabilities &capabilities) const = 0;
};

} // namespace input_policy
