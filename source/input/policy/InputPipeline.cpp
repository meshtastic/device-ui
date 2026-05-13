#include "input/policy/InputPipeline.h"
#include "util/ILog.h"

namespace input_policy
{

InputPipeline::InputPipeline(std::shared_ptr<IActionBindingResolver> bindingResolver,
                             std::shared_ptr<IInputContextProvider> contextProvider,
                             std::shared_ptr<IUICommandDispatcher> commandDispatcher)
    : bindingResolver(std::move(bindingResolver)), contextProvider(std::move(contextProvider)),
      commandDispatcher(std::move(commandDispatcher))
{
}

void InputPipeline::setPolicyChain(PolicyChain chain)
{
    this->chain = std::move(chain);
}

bool InputPipeline::process(InputEvent &event, const InputCapabilities &capabilities, std::vector<InputEvent> &outEvents)
{
    outEvents.clear();

    ILOG_DEBUG("[Pipeline] Processing event: rawKey=0x%x action=%d press=%d source=%s", event.rawKeyCode, event.action,
               (int)event.pressKind, event.sourceId.c_str());

    InputContextSnapshot context{};
    if (contextProvider) {
        context = contextProvider->getSnapshot();
        ILOG_DEBUG("[Pipeline] Context: activePanel=%d semantic=%d editMode=%d", context.activePanelId,
                   (int)context.focusSemantic, context.isEditMode);
    }

    if (bindingResolver) {
        auto oldAction = event.action;
        event.action = bindingResolver->resolveAction(event, context, capabilities);
        if (oldAction != event.action) {
            ILOG_DEBUG("[Pipeline] Binding resolved: %d -> %d", (int)oldAction, (int)event.action);
        }
    }

    ILOG_DEBUG("[Pipeline] Evaluating policy chain with %d policies", chain.getPolicyCount());
    PolicyDecision decision = chain.evaluate(event, context, capabilities);
    ILOG_DEBUG("[Pipeline] Policy chain decision: type=%d", (int)decision.type);

    switch (decision.type) {
    case DecisionType::Pass:
        ILOG_DEBUG("[Pipeline] Decision=Pass, forwarding event 0x%x", event.rawKeyCode);
        outEvents.push_back(event);
        return true;
    case DecisionType::Remap:
        ILOG_DEBUG("[Pipeline] Decision=Remap, 0x%x -> 0x%x", event.rawKeyCode,
                   decision.remappedEvent.resolvedKeyCode != 0 ? decision.remappedEvent.resolvedKeyCode
                                                               : decision.remappedEvent.rawKeyCode);
        outEvents.push_back(decision.remappedEvent);
        return true;
    case DecisionType::Consume:
        ILOG_DEBUG("[Pipeline] Decision=Consume, dropping event 0x%x", event.rawKeyCode);
        return false;
    case DecisionType::EmitCommand:
        ILOG_DEBUG("[Pipeline] Decision=EmitCommand, cmd=%d", (int)decision.command.command);
        if (commandDispatcher && decision.command.command != UICommand::None) {
            commandDispatcher->dispatch(decision.command.command, decision.command);
        }
        return false;
    case DecisionType::EmitSequence:
        ILOG_DEBUG("[Pipeline] Decision=EmitSequence, %d events", (int)decision.emittedEvents.size());
        outEvents = std::move(decision.emittedEvents);
        return !outEvents.empty();
    }

    outEvents.push_back(event);
    return true;
}

} // namespace input_policy
