#pragma once

#include "input/policy/IInputPolicy.h"
#include <memory>
#include <vector>

namespace input_policy
{

/**
 * Stores policy instances in evaluation order for event processing.
 * Pass means "not handled here", so evaluation continues until a policy emits
 * a non-pass decision or the chain is exhausted.
 */
class PolicyChain
{
  public:
    using PolicyPtr = std::shared_ptr<IInputPolicy>;

    // Removes all policies from the chain.
    void clear();
    // Appends a policy to the end of the chain.
    void addPolicy(PolicyPtr policy);
    // Returns the number of policies currently registered.
    int getPolicyCount() const { return (int)policies.size(); }
    // Evaluates policies in order and returns the first non-pass decision.
    PolicyDecision evaluate(const InputEvent &event, const InputContextSnapshot &context,
                            const InputCapabilities &capabilities) const;

  private:
    std::vector<PolicyPtr> policies;
};

} // namespace input_policy
