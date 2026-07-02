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

/**
 * Defines how input policy components are assembled for runtime use.
 * Implementations provide a resolver and ordered policy chain.
 */
class InputPolicyFactory
{
  public:
    // Virtual destructor for safe polymorphic cleanup.
    virtual ~InputPolicyFactory() = default;

    // Builds a resolver and policy chain for the provided source registry and context.
    virtual InputPolicyBuildResult build(const InputSourceRegistry &registry,
                                         std::shared_ptr<IInputContextProvider> contextProvider,
                                         std::shared_ptr<IUICommandDispatcher> commandDispatcher) const = 0;
};

} // namespace input_policy
