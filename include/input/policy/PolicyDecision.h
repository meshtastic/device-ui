#pragma once

#include "input/policy/InputTypes.h"
#include <vector>

namespace input_policy
{

// Carries optional arguments for a dispatched UI command.
struct CommandPayload {
    UICommand command = UICommand::None;
    int32_t argInt = 0;
    bool argBool = false;
};

// Describes the result produced by a policy evaluation step.
struct PolicyDecision {
    DecisionType type = DecisionType::Pass;
    InputEvent remappedEvent{};
    CommandPayload command{};
    std::vector<InputEvent> emittedEvents{};
};

} // namespace input_policy
