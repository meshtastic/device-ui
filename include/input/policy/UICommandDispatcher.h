#pragma once

#include "input/policy/IUICommandDispatcher.h"
#include <array>
#include <functional>

namespace input_policy
{

/**
 * Generic command dispatcher with callback registration per UI command.
 * Views can bind lambdas without exposing view-specific APIs to the input layer.
 */
class UICommandDispatcher : public IUICommandDispatcher
{
  public:
    using CommandHandler = std::function<void(const CommandPayload &)>;

    // Returns the process-wide singleton instance.
    static UICommandDispatcher &instance();

    // Registers or replaces a callback handler for the given command.
    bool registerHandler(UICommand command, CommandHandler handler);
    // Removes a callback handler for the given command.
    bool unregisterHandler(UICommand command);
    // Clears all registered command handlers.
    void clearHandlers();

    // Dispatches the command to its registered callback.
    void dispatch(UICommand command, const CommandPayload &payload) override;

    // Maps an InputAction command to UICommand and dispatches it.
    bool dispatchAction(InputAction action, const CommandPayload &payload = {});
    // Converts command-like InputAction values to UICommand.
    static UICommand toUICommand(InputAction action);

  private:
    // Returns array index for a UI command, or -1 when invalid.
    static int commandIndex(UICommand command);

    std::array<CommandHandler, static_cast<size_t>(UICommand::Count)> handlers{};
};

} // namespace input_policy
