#include "input/policy/FocusTraversalPolicy.h"
#include "lvgl.h"
#include "util/ILog.h"

namespace input_policy
{

PolicyDecision FocusTraversalPolicy::evaluate(const InputEvent &event, const InputContextSnapshot &context,
                                              const InputCapabilities &) const
{
    // Keep directional keys in map and text-edit contexts.
    if (context.focusSemantic == FocusSemantic::Map || context.focusSemantic == FocusSemantic::TextEdit || context.isEditMode) {
        ILOG_DEBUG("[FocusTraversal] Keeping directional key in map/edit context (semantic=%d editMode=%d)",
                   (int)context.focusSemantic, context.isEditMode);
        return PolicyDecision{};
    }

    PolicyDecision decision{};
    if (event.action == InputAction::NavigateUp) {
        decision.type = DecisionType::Remap;
        decision.remappedEvent = event;
        decision.remappedEvent.resolvedKeyCode = LV_KEY_PREV;
        ILOG_DEBUG("[FocusTraversal] Remapped NavigateUp -> LV_KEY_PREV");
        return decision;
    }

    if (event.action == InputAction::NavigateDown) {
        decision.type = DecisionType::Remap;
        decision.remappedEvent = event;
        decision.remappedEvent.resolvedKeyCode = LV_KEY_NEXT;
        ILOG_DEBUG("[FocusTraversal] Remapped NavigateDown -> LV_KEY_NEXT");
        return decision;
    }

    ILOG_DEBUG("[FocusTraversal] No remapping for action %d", (int)event.action);
    return PolicyDecision{};
}

} // namespace input_policy
