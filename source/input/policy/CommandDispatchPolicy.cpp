#include "input/policy/CommandDispatchPolicy.h"
#include "input/policy/PolicyDebug.h"
#include "input/policy/UICommandDispatcher.h"

namespace input_policy
{

PolicyDecision CommandDispatchPolicy::evaluate(const InputEvent &event, const InputContextSnapshot &,
                                               const InputCapabilities &) const
{
    if (event.pressKind == PressKind::Release) {
        return PolicyDecision{};
    }

    const UICommand command = UICommandDispatcher::toUICommand(event.action);
    if (command == UICommand::None) {
        return PolicyDecision{};
    }

    PolicyDecision decision{};
    decision.type = DecisionType::EmitCommand;
    decision.command.command = command;
    POLICY_DEBUG("[CommandDispatch] Emitting command %d for action %d", (int)command, (int)event.action);
    return decision;
}

} // namespace input_policy