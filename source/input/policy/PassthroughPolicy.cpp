#include "input/policy/PassthroughPolicy.h"

namespace input_policy
{

PolicyDecision PassthroughPolicy::evaluate(const InputEvent &, const InputContextSnapshot &, const InputCapabilities &) const
{
    return PolicyDecision{};
}

} // namespace input_policy
