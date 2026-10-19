#include "input/KeypadReleaseGuard.h"

namespace
{
lv_obj_t *longPressedObj = nullptr;

void keypadEventCb(lv_event_t *e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    auto *obj = static_cast<lv_obj_t *>(lv_event_get_param(e)); // object the indev is about to notify
    switch (code) {
    case LV_EVENT_PRESSED:
        longPressedObj = nullptr;
        break;
    case LV_EVENT_LONG_PRESSED:
        longPressedObj = obj;
        break;
    case LV_EVENT_LONG_PRESSED_REPEAT:
    case LV_EVENT_RELEASED:
    case LV_EVENT_CLICKED: {
        lv_obj_t *target = longPressedObj;
        if (code == LV_EVENT_CLICKED)
            longPressedObj = nullptr; // last event of a keypad press
        if (target && obj != target) {
            auto *indev = static_cast<lv_indev_t *>(lv_event_get_current_target(e));
            lv_indev_stop_processing(indev);
            if (lv_obj_is_valid(target))
                lv_obj_send_event(target, code, indev);
        }
        break;
    }
    default:
        break;
    }
}
} // namespace

namespace KeypadReleaseGuard
{
void attach(lv_indev_t *keypad)
{
    if (!keypad || lv_indev_get_type(keypad) != LV_INDEV_TYPE_KEYPAD)
        return;
    for (uint32_t i = 0; i < lv_indev_get_event_count(keypad); i++) {
        if (lv_event_dsc_get_cb(lv_indev_get_event_dsc(keypad, i)) == keypadEventCb)
            return;
    }
    lv_indev_add_event_cb(keypad, keypadEventCb, LV_EVENT_ALL, nullptr);
}
} // namespace KeypadReleaseGuard
