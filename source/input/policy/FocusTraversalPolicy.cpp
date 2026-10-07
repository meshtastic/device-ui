#include "input/policy/FocusTraversalPolicy.h"
#include "input/policy/PolicyDebug.h"
#include "lvgl.h"

namespace input_policy
{

PolicyDecision FocusTraversalPolicy::evaluate(const InputEvent &event, const InputContextSnapshot &context,
                                              const InputCapabilities &) const
{
    // Keep directional keys in map, multi-line text-edit, and scrollable-panel contexts.
    if (context.focusSemantic == FocusSemantic::Map || context.focusSemantic == FocusSemantic::TextAreaMultiLine ||
        context.focusSemantic == FocusSemantic::Scrollable) {
        POLICY_DEBUG("[FocusTraversal] Keeping directional key in map/multi-line edit context (semantic=%d)",
                     (int)context.focusSemantic);
        return PolicyDecision{};
    }
    if (context.focusSemantic == FocusSemantic::Keyboard) {
        if (event.action == InputAction::NavigateLeft || event.action == InputAction::NavigateRight) {
            POLICY_DEBUG("[FocusTraversal] Keeping Left/Right for keyboard navigation");
            return PolicyDecision{};
        }
    }
    if (context.focusSemantic == FocusSemantic::ButtonMatrix) {
        POLICY_DEBUG("[FocusTraversal] Keeping up/down for button matrix navigation");
        return PolicyDecision{}; // pass through UP/DOWN/LEFT/RIGHT unchanged
    }
    PolicyDecision decision{};
    if (event.action == InputAction::NavigateUp) {
        decision.type = DecisionType::Remap;
        decision.remappedEvent = event;
        decision.remappedEvent.resolvedKeyCode = LV_KEY_PREV;
        POLICY_DEBUG("[FocusTraversal] Remapped NavigateUp -> LV_KEY_PREV");
        return decision;
    }

    if (event.action == InputAction::NavigateDown) {
        decision.type = DecisionType::Remap;
        decision.remappedEvent = event;
        decision.remappedEvent.resolvedKeyCode = LV_KEY_NEXT;
        POLICY_DEBUG("[FocusTraversal] Remapped NavigateDown -> LV_KEY_NEXT");
        return decision;
    }

    POLICY_DEBUG("[FocusTraversal] No remapping for action %d", (int)event.action);
    return PolicyDecision{};
}

} // namespace input_policy
