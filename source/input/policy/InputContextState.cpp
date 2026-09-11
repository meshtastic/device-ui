#include "input/policy/InputContextState.h"
#include "input/policy/PolicyDebug.h"
namespace input_policy
{

InputContextState &InputContextState::instance()
{
    static InputContextState ctx;
    return ctx;
}

InputContextSnapshot InputContextState::getSnapshot() const
{
    return snapshot;
}

void InputContextState::setSnapshot(const InputContextSnapshot &snapshot)
{
    this->snapshot = snapshot;
}

void InputContextState::setFocusSemantic(FocusSemantic semantic)
{
    POLICY_DEBUG("setFocusSemantic %d", (int)semantic);
    snapshot.focusSemantic = semantic;
}

void InputContextState::setEditMode(bool enabled)
{
    POLICY_DEBUG("setEditMode %d", (int)enabled);
    snapshot.isEditMode = enabled;
}

void InputContextState::setCanLeaveEditMode(bool enabled)
{
    snapshot.canLeaveEditMode = enabled;
}

void InputContextState::setActivePanelId(uint32_t panelId)
{
    snapshot.activePanelId = panelId;
}

void InputContextState::setFocusedClassHint(const std::string &hint)
{
    snapshot.focusedClassHint = hint;
}

} // namespace input_policy
