#pragma once

#include "input/policy/InputTypes.h"

namespace input_policy
{

/**
 * Represents an input producer that feeds events into the policy pipeline.
 * Sources expose capabilities and provide normalized events on poll.
 */
class IInputSource
{
  public:
    // Virtual destructor for safe polymorphic cleanup.
    virtual ~IInputSource() = default;

    // Returns a stable source identifier used for diagnostics and routing.
    virtual const char *getSourceId() const = 0;
    // Returns the capability flags exposed by this source.
    virtual InputCapabilities getCapabilities() const = 0;
    // Polls the source and writes the next event when available.
    virtual bool poll(InputEvent &outEvent) = 0;
};

} // namespace input_policy
