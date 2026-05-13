#pragma once

#include "input/policy/IActionBindingResolver.h"
#include "input/policy/IInputContextProvider.h"
#include "input/policy/IUICommandDispatcher.h"
#include "input/policy/InputPipeline.h"
#include "input/policy/InputSourceRegistry.h"
#include "input/policy/PolicyChain.h"
#include <memory>

namespace input_policy
{

struct InputPolicyBuildResult {
    std::shared_ptr<IActionBindingResolver> bindingResolver;
    PolicyChain chain;
};

class InputPolicyFactory
{
  public:
    virtual ~InputPolicyFactory() = default;

    virtual InputPolicyBuildResult build(const InputSourceRegistry &registry,
                                         std::shared_ptr<IInputContextProvider> contextProvider,
                                         std::shared_ptr<IUICommandDispatcher> commandDispatcher) const = 0;
};

} // namespace input_policy
