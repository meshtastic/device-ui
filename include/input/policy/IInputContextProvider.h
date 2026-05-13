#pragma once

#include "input/policy/InputTypes.h"

namespace input_policy
{

class IInputContextProvider
{
  public:
    virtual ~IInputContextProvider() = default;
    virtual InputContextSnapshot getSnapshot() const = 0;
};

} // namespace input_policy
