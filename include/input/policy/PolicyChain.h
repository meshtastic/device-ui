#pragma once

#include "input/policy/IInputPolicy.h"
#include <memory>
#include <vector>

namespace input_policy
{

class PolicyChain
{
  public:
    using PolicyPtr = std::shared_ptr<IInputPolicy>;

    void clear();
    void addPolicy(PolicyPtr policy);
    int getPolicyCount() const { return (int)policies.size(); }
    PolicyDecision evaluate(const InputEvent &event, const InputContextSnapshot &context,
                            const InputCapabilities &capabilities) const;

  private:
    std::vector<PolicyPtr> policies;
};

} // namespace input_policy
