#include "graphics/view/TFT/TextareaEditContext.h"
#include "input/policy/InputContextState.h"

namespace
{
lv_obj_t *focusedTextarea = nullptr;
uint32_t focusedPanelId = 0;
input_policy::FocusSemantic focusedSemantic = input_policy::FocusSemantic::Unknown;
} // namespace

void TextareaEditContext::focus(lv_obj_t *textarea)
{
    auto &context = input_policy::InputContextState::instance();
    focusedTextarea = textarea;
    focusedPanelId = context.getSnapshot().activePanelId;
    focusedSemantic = lv_textarea_get_one_line(textarea) ? input_policy::FocusSemantic::TextEdit
                                                         : input_policy::FocusSemantic::TextAreaMultiLine;
    context.setFocusSemantic(focusedSemantic);
    context.setEditMode(true);
    context.setCanLeaveEditMode(true);
}

void TextareaEditContext::release(lv_obj_t *textarea)
{
    if (!textarea || focusedTextarea != textarea)
        return;
    focusedTextarea = nullptr;

    auto &context = input_policy::InputContextState::instance();
    const auto snapshot = context.getSnapshot();
    if (snapshot.activePanelId != focusedPanelId || snapshot.focusSemantic != focusedSemantic)
        return;

    context.setFocusSemantic(input_policy::FocusSemantic::Unknown);
    context.setEditMode(false);
    context.setCanLeaveEditMode(false);
}

void TextareaEditContext::handleEvent(lv_event_t *event)
{
    const auto code = lv_event_get_code(event);
    if (code == LV_EVENT_FOCUSED) {
        focus(lv_event_get_target_obj(event));
    } else if (code == LV_EVENT_DEFOCUSED || code == LV_EVENT_LEAVE || code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        release(lv_event_get_target_obj(event));
    }
}
