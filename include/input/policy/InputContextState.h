#pragma once

#include "input/policy/IInputContextProvider.h"

namespace input_policy
{

/**
 * Stores mutable UI/input context used by policy evaluation.
 * Exposed as a singleton so producers and consumers share one state source.
 */
class InputContextState : public IInputContextProvider
{
  public:
    // Returns the singleton instance.
    static InputContextState &instance();

    // Returns the current context snapshot.
    InputContextSnapshot getSnapshot() const override;
    // Replaces the full context snapshot.
    void setSnapshot(const InputContextSnapshot &snapshot);

    // Sets the current focus semantic.
    void setFocusSemantic(FocusSemantic semantic);
    // Sets whether edit mode is active.
    void setEditMode(bool enabled);
    // Sets whether edit mode can be exited.
    void setCanLeaveEditMode(bool enabled);
    // Sets the active panel identifier.
    void setActivePanelId(uint32_t panelId);
    // Sets a hint for the currently focused widget class.
    void setFocusedClassHint(const std::string &hint);

  private:
    // Creates the singleton context state instance.
    InputContextState() = default;

    InputContextSnapshot snapshot{};
};

} // namespace input_policy
