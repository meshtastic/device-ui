#include "input/policy/PolicyChain.h"

namespace input_policy
{

void PolicyChain::clear()
{
    policies.clear();
}

void PolicyChain::addPolicy(PolicyPtr policy)
{
    if (policy) {
        policies.push_back(policy);
    }
}

PolicyDecision PolicyChain::evaluate(const InputEvent &event, const InputContextSnapshot &context,
                                     const InputCapabilities &capabilities) const
{
    for (const auto &policy : policies) {
        if (!policy) {
            continue;
        }
        PolicyDecision decision = policy->evaluate(event, context, capabilities);
        if (decision.type == DecisionType::Pass) {
            continue;
        }

        return decision;
    }

    return PolicyDecision{};
}

} // namespace input_policy
