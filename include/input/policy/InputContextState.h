#pragma once

#include "input/policy/IInputContextProvider.h"

namespace input_policy
{

class InputContextState : public IInputContextProvider
{
  public:
    static InputContextState &instance();

    InputContextSnapshot getSnapshot() const override;
    void setSnapshot(const InputContextSnapshot &snapshot);

    void setFocusSemantic(FocusSemantic semantic);
    void setEditMode(bool enabled);
    void setCanLeaveEditMode(bool enabled);
    void setActivePanelId(uint32_t panelId);
    void setFocusedClassHint(const std::string &hint);

  private:
    InputContextState() = default;

    InputContextSnapshot snapshot{};
};

} // namespace input_policy
