#pragma once

#include "input/policy/InputPolicyFactory.h"
#include <memory>

namespace input_policy
{

/**
 * @class DefaultInputPolicyFactory
 * @brief Concrete factory for building input policy pipelines with capability-driven composition
 *
 * Constructs an InputPipeline with a default policy chain based on the capabilities
 * of registered input sources. Always includes FocusTraversalPolicy and PassthroughPolicy,
 * with optional conditional policies based on hardware capabilities.
 *
 * Usage:
 *   DefaultInputPolicyFactory factory;
 *   auto result = factory.build(registry, contextProvider, commandDispatcher);
 *   pipeline->setPolicyChain(std::move(result.chain));
 */
class DefaultInputPolicyFactory : public InputPolicyFactory
{
  public:
    virtual ~DefaultInputPolicyFactory() = default;

    /**
     * @brief Build a default input policy configuration from registered sources
     * @param registry Input source registry with merged capabilities
     * @param contextProvider UI context provider (e.g., InputContextState)
     * @param commandDispatcher UI command dispatcher for high-level actions
     * @return InputPolicyBuildResult containing binding resolver and policy chain
     */
    InputPolicyBuildResult build(const InputSourceRegistry &registry, std::shared_ptr<IInputContextProvider> contextProvider,
                                 std::shared_ptr<IUICommandDispatcher> commandDispatcher) const override;
};

} // namespace input_policy
