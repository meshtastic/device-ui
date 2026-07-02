#include "input/policy/DefaultInputPolicyFactory.h"
#include "input/policy/CommandDispatchPolicy.h"
#include "input/policy/DefaultBindingResolver.h"
#include "input/policy/FocusTraversalPolicy.h"
#include "input/policy/PassthroughPolicy.h"
#include "util/ILog.h"

namespace input_policy
{

InputPolicyBuildResult DefaultInputPolicyFactory::build(const InputSourceRegistry &registry,
                                                        std::shared_ptr<IInputContextProvider> contextProvider,
                                                        std::shared_ptr<IUICommandDispatcher> commandDispatcher) const
{
    InputPolicyBuildResult result;

    // Create binding resolver - maps input events to actions
    result.bindingResolver = std::make_shared<DefaultBindingResolver>();

    // Query merged capabilities from all registered input sources
    const auto capabilities = registry.mergedCapabilities();

    // Build policy chain based on merged capabilities and feature availability
    PolicyChain chain;
    ILOG_DEBUG("[InputFactory] Building policy chain with capabilities: arrows=%d tab=%d home=%d end=%d pgupdown=%d",
               capabilities.hasArrowKeys, capabilities.hasTabKey, capabilities.hasHomeKey, capabilities.hasEndKey,
               capabilities.hasPgUpPgDownKeys);

    // Core policy: focus traversal (up/down conversion outside map/edit context)
    // This runs first to intercept focus navigation before other policies
    ILOG_DEBUG("[InputFactory] Adding FocusTraversalPolicy");
    chain.addPolicy(std::make_shared<FocusTraversalPolicy>());

    // Conditional policies: add PageNavigationPolicy if hardware supports pgup/pgdown
    // (Future expansion point when PageNavigationPolicy is implemented)
    // if (capabilities.hasPgUpPgDownKeys) {
    //     chain.addPolicy(std::make_shared<PageNavigationPolicy>());
    // }

    // Conditional policies: add special key policies when available
    // (Future expansion point for Home/Chat/Location/GPS toggle policies)
    if (commandDispatcher) {
        ILOG_DEBUG("[InputFactory] Adding CommandDispatchPolicy");
        chain.addPolicy(std::make_shared<CommandDispatchPolicy>());
    }

    // Fallback policy: pass through all unhandled events to LVGL
    // This must be last in the chain to ensure everything gets a chance
    ILOG_DEBUG("[InputFactory] Adding PassthroughPolicy as fallback");
    chain.addPolicy(std::make_shared<PassthroughPolicy>());

    result.chain = std::move(chain);

    return result;
}

} // namespace input_policy
