#include "input/policy/UICommandDispatcher.h"
#include "util/ILog.h"

namespace input_policy
{

UICommandDispatcher &UICommandDispatcher::instance()
{
    static UICommandDispatcher singleton;
    return singleton;
}

bool UICommandDispatcher::registerHandler(UICommand command, CommandHandler handler)
{
    const int idx = commandIndex(command);
    if (idx < 0) {
        ILOG_WARN("[UICommandDispatcher] Reject register for invalid command=%d", (int)command);
        return false;
    }

    handlers[(size_t)idx] = std::move(handler);
    ILOG_DEBUG("[UICommandDispatcher] Registered handler for command=%d", (int)command);
    return true;
}

bool UICommandDispatcher::unregisterHandler(UICommand command)
{
    const int idx = commandIndex(command);
    if (idx < 0) {
        return false;
    }

    handlers[(size_t)idx] = nullptr;
    return true;
}

void UICommandDispatcher::clearHandlers()
{
    handlers.fill(nullptr);
}

void UICommandDispatcher::dispatch(UICommand command, const CommandPayload &payload)
{
    const int idx = commandIndex(command);
    if (idx < 0) {
        ILOG_WARN("[UICommandDispatcher] Ignoring invalid command=%d", (int)command);
        return;
    }

    const auto &handler = handlers[(size_t)idx];
    if (!handler) {
        ILOG_DEBUG("[UICommandDispatcher] No handler registered for command=%d", (int)command);
        return;
    }

    handler(payload);
}

bool UICommandDispatcher::dispatchAction(InputAction action, const CommandPayload &payload)
{
    const UICommand command = toUICommand(action);
    if (command == UICommand::None) {
        return false;
    }

    CommandPayload resolved = payload;
    resolved.command = command;
    dispatch(command, resolved);
    return true;
}

UICommand UICommandDispatcher::toUICommand(InputAction action)
{
    switch (action) {
    case InputAction::CommandHome:
        return UICommand::GoHome;
    case InputAction::CommandOpenChats:
        return UICommand::OpenChats;
    case InputAction::CommandQuickChat:
        return UICommand::QuickChat;
    case InputAction::CommandOpenMap:
        return UICommand::OpenMap;
    case InputAction::CommandToggleGps:
        return UICommand::ToggleGps;
    case InputAction::CommandSendPing:
        return UICommand::SendPing;
    default:
        return UICommand::None;
    }
}

int UICommandDispatcher::commandIndex(UICommand command)
{
    const int idx = (int)command;
    const int max = (int)UICommand::LeaveEditMode;
    if (idx < 0 || idx > max) {
        return -1;
    }
    return idx;
}

} // namespace input_policy
