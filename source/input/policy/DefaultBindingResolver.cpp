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

    if (event.action != InputAction::None) {
        ILOG_DEBUG("[Resolver] Action already set to %d", (int)event.action);
        return event.action;
    }

    InputAction action = InputAction::None;
    switch (event.rawKeyCode) {
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
