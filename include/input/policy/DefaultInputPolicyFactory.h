#pragma once

#include "input/policy/InputPolicyFactory.h"
#include <memory>

namespace input_policy
{

/**
 * Builds the standard input policy stack used by the UI runtime.
 * It composes default resolver and policy-chain components from capabilities.
 */
class DefaultInputPolicyFactory : public InputPolicyFactory
{
  public:
    // Virtual destructor for safe polymorphic cleanup.
    virtual ~DefaultInputPolicyFactory() = default;

    // Builds resolver and policy chain from current sources and context hooks.
    InputPolicyBuildResult build(const InputSourceRegistry &registry, std::shared_ptr<IInputContextProvider> contextProvider,
                                 std::shared_ptr<IUICommandDispatcher> commandDispatcher) const override;
};

} // namespace input_policy
