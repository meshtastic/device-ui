#pragma once

#include "input/policy/IActionBindingResolver.h"
#include "input/policy/IInputContextProvider.h"
#include "input/policy/IUICommandDispatcher.h"
#include "input/policy/PolicyChain.h"
#include <memory>
#include <vector>

namespace input_policy
{

/**
 * Orchestrates action resolution, policy evaluation, and command dispatch.
 * This is the main runtime entry point for processing input events.
 */
class InputPipeline
{
  public:
    // Creates a pipeline with resolver, context provider, and command dispatcher.
    InputPipeline(std::shared_ptr<IActionBindingResolver> bindingResolver, std::shared_ptr<IInputContextProvider> contextProvider,
                  std::shared_ptr<IUICommandDispatcher> commandDispatcher);

    // Replaces the active policy chain.
    void setPolicyChain(PolicyChain chain);
    // Processes an input event and emits zero or more output events.
    bool process(InputEvent &event, const InputCapabilities &capabilities, std::vector<InputEvent> &outEvents);

  private:
    std::shared_ptr<IActionBindingResolver> bindingResolver;
    std::shared_ptr<IInputContextProvider> contextProvider;
    std::shared_ptr<IUICommandDispatcher> commandDispatcher;
    PolicyChain chain;
};

} // namespace input_policy
