#pragma once

#include "input/policy/IActionBindingResolver.h"
#include "input/policy/IInputContextProvider.h"
#include "input/policy/IUICommandDispatcher.h"
#include "input/policy/PolicyChain.h"
#include <memory>
#include <vector>

namespace input_policy
{

class InputPipeline
{
  public:
    InputPipeline(std::shared_ptr<IActionBindingResolver> bindingResolver, std::shared_ptr<IInputContextProvider> contextProvider,
                  std::shared_ptr<IUICommandDispatcher> commandDispatcher);

    void setPolicyChain(PolicyChain chain);
    bool process(InputEvent &event, const InputCapabilities &capabilities, std::vector<InputEvent> &outEvents);

  private:
    std::shared_ptr<IActionBindingResolver> bindingResolver;
    std::shared_ptr<IInputContextProvider> contextProvider;
    std::shared_ptr<IUICommandDispatcher> commandDispatcher;
    PolicyChain chain;
};

} // namespace input_policy
