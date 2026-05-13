#include "input/policy/DefaultBindingResolver.h"
#include "lvgl.h"
#include "util/ILog.h"

namespace input_policy
{

InputAction DefaultBindingResolver::resolveAction(const InputEvent &event, const InputContextSnapshot &,
                                                  const InputCapabilities &) const
{
    constexpr uint32_t KEY_PAGE_UP = 0x21;
    constexpr uint32_t KEY_PAGE_DOWN = 0x22;
    constexpr uint32_t KEY_CMD_HOME = 0x100;
    constexpr uint32_t KEY_CMD_CHATS = 0x101;
    constexpr uint32_t KEY_CMD_MAP = 0x102;
    constexpr uint32_t KEY_CMD_TOGGLE_GPS = 0x103;
    constexpr uint32_t KEY_CMD_SEND_PING = 0x104;

    if (event.action != InputAction::None) {
        ILOG_DEBUG("[Resolver] Action already set to %d", (int)event.action);
        return event.action;
    }

    InputAction action = InputAction::None;
    switch (event.rawKeyCode) {
    case KEY_CMD_HOME:
        action = InputAction::CommandHome;
        break;
    case KEY_CMD_CHATS:
        action = InputAction::CommandOpenChats;
        break;
    case KEY_CMD_MAP:
        action = InputAction::CommandOpenMap;
        break;
    case KEY_CMD_TOGGLE_GPS:
        action = InputAction::CommandToggleGps;
        break;
    case KEY_CMD_SEND_PING:
        action = InputAction::CommandSendPing;
        break;
    case KEY_PAGE_UP:
        action = InputAction::NavigatePgUp;
        break;
    case KEY_PAGE_DOWN:
        action = InputAction::NavigatePgDown;
        break;
    case LV_KEY_HOME:
        action = InputAction::NavigateHome;
        break;
    case LV_KEY_END:
        action = InputAction::NavigateEnd;
        break;
    case LV_KEY_UP:
        action = InputAction::NavigateUp;
        break;
    case LV_KEY_DOWN:
        action = InputAction::NavigateDown;
        break;
    case LV_KEY_LEFT:
        action = InputAction::NavigateLeft;
        break;
    case LV_KEY_RIGHT:
        action = InputAction::NavigateRight;
        break;
    case LV_KEY_ENTER:
        action = InputAction::Activate;
        break;
    case LV_KEY_ESC:
        action = InputAction::Cancel;
        break;
    case LV_KEY_NEXT:
        action = InputAction::NavigateDown;
        break;
    case LV_KEY_PREV:
        action = InputAction::NavigateUp;
        break;
    default:
        action = InputAction::None;
        break;
    }

    if (action != InputAction::None) {
        ILOG_DEBUG("[Resolver] Resolved key 0x%x -> action %d", event.rawKeyCode, (int)action);
    }
    return action;
}

} // namespace input_policy
