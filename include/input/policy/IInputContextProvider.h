#pragma once

#include "input/policy/InputTypes.h"

namespace input_policy
{

/**
 * Provides read access to the current input and UI context snapshot.
 * Policies use this interface to evaluate events against the latest state.
 */
class IInputContextProvider
{
  public:
    // Virtual destructor for safe polymorphic cleanup.
    virtual ~IInputContextProvider() = default;
    // Returns the latest context snapshot used by the policy pipeline.
    virtual InputContextSnapshot getSnapshot() const = 0;
};

} // namespace input_policy
