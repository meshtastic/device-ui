#include "graphics/view/TFT/DropdownKeyNavigation.h"
#include "input/policy/InputContextState.h"
#include "input/policy/PolicyDebug.h"

namespace
{
using input_policy::FocusSemantic;
using input_policy::InputContextSnapshot;
using input_policy::InputContextState;

lv_obj_t *openDropdown = nullptr;
InputContextSnapshot contextBeforeOpen{};

void enterEditMode(lv_obj_t *dropdown)
{
    POLICY_DEBUG("[DropdownKeyNavigation] enterEditMode");
    auto &ctx = InputContextState::instance();
    if (!openDropdown)
        contextBeforeOpen = ctx.getSnapshot();
    openDropdown = dropdown;
    ctx.setFocusSemantic(FocusSemantic::Dropdown);
    ctx.setEditMode(true);
    ctx.setCanLeaveEditMode(true);
}

void leaveEditMode(lv_obj_t *dropdown)
{
    POLICY_DEBUG("[DropdownKeyNavigation] leaveEditMode");
    if (dropdown != openDropdown)
        return;
    openDropdown = nullptr;
    auto &ctx = InputContextState::instance();
    // someone else took over the context while the list was open
    if (ctx.getSnapshot().focusSemantic != FocusSemantic::Dropdown)
        return;
    ctx.setFocusSemantic(contextBeforeOpen.focusSemantic);
    ctx.setEditMode(contextBeforeOpen.isEditMode);
    ctx.setCanLeaveEditMode(contextBeforeOpen.canLeaveEditMode);
}

// Runs as preprocess handler, i.e. before the dropdown class handler.
void handleKey(lv_event_t *e, lv_obj_t *dropdown)
{
    const uint32_t key = lv_event_get_key(e);
    const bool open = lv_dropdown_is_open(dropdown);
    POLICY_DEBUG("[DropdownKeyNavigation] handleKey 0x%04x", key & 0xFFFF);
    if (key == LV_KEY_LEFT || key == LV_KEY_RIGHT) {
        // LVGL treats LEFT/RIGHT like UP/DOWN: they would open the list or move its selection
        lv_event_stop_processing(e);
        // stop_processing also ends bubbling, so pass a closed dropdown's key on as if it bubbled
        lv_obj_t *parent = lv_obj_get_parent(dropdown);
        if (!open && parent && lv_obj_has_flag(dropdown, LV_OBJ_FLAG_EVENT_BUBBLE))
            lv_obj_send_event(parent, LV_EVENT_KEY, lv_event_get_param(e));
    } else if (key == LV_KEY_ESC) {
        lv_obj_t *parent = lv_obj_get_parent(dropdown);
        if (open) {
            // close the list only, not the dialog around it
            lv_event_stop_bubbling(e);
        } else if (parent && !lv_obj_has_flag(dropdown, LV_OBJ_FLAG_EVENT_BUBBLE)) {
            // generated dropdowns don't bubble, but ESC must still leave the panel or dialog
            lv_obj_send_event(parent, LV_EVENT_KEY, lv_event_get_param(e));
        }
    }
}

void dropdownEventCb(lv_event_t *e)
{
    POLICY_DEBUG("[DropdownKeyNavigation] dropdownEventCb");
    lv_obj_t *dropdown = lv_event_get_current_target_obj(e);
    switch (lv_event_get_code(e)) {
    case LV_EVENT_READY: // sent by lv_dropdown_open()
        enterEditMode(dropdown);
        break;
    case LV_EVENT_CANCEL: // sent by lv_dropdown_close(), also by the keypad indev on ESC
        if (!lv_dropdown_is_open(dropdown))
            leaveEditMode(dropdown);
        break;
    case LV_EVENT_DELETE:
        leaveEditMode(dropdown);
        break;
    case LV_EVENT_KEY:
        handleKey(e, dropdown);
        break;
    default:
        break;
    }
}
} // namespace

namespace DropdownKeyNavigation
{
void attach(lv_obj_t *root)
{
    lv_obj_tree_walk(
        root,
        [](lv_obj_t *obj, void *) -> lv_obj_tree_walk_res_t {
            if (lv_obj_check_type(obj, &lv_dropdown_class)) {
                lv_obj_add_event_cb(obj, dropdownEventCb, LV_EVENT_READY, nullptr);
                lv_obj_add_event_cb(obj, dropdownEventCb, LV_EVENT_CANCEL, nullptr);
                lv_obj_add_event_cb(obj, dropdownEventCb, LV_EVENT_DELETE, nullptr);
                lv_obj_add_event_cb(obj, dropdownEventCb, (lv_event_code_t)(LV_EVENT_KEY | LV_EVENT_PREPROCESS), nullptr);
            }
            return LV_OBJ_TREE_WALK_NEXT;
        },
        nullptr);
}
} // namespace DropdownKeyNavigation
