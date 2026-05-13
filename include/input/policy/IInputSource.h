#pragma once

#include "input/policy/InputTypes.h"

namespace input_policy
{

class IInputSource
{
  public:
    virtual ~IInputSource() = default;

    virtual const char *getSourceId() const = 0;
    virtual InputCapabilities getCapabilities() const = 0;
    virtual bool poll(InputEvent &outEvent) = 0;
};

} // namespace input_policy
