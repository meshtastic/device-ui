#pragma once

#include "input/policy/InputTypes.h"
#include <vector>

namespace input_policy
{

struct CommandPayload {
    UICommand command = UICommand::None;
    int32_t argInt = 0;
    bool argBool = false;
};

struct PolicyDecision {
    DecisionType type = DecisionType::Pass;
    InputEvent remappedEvent{};
    CommandPayload command{};
    std::vector<InputEvent> emittedEvents{};
};

} // namespace input_policy
