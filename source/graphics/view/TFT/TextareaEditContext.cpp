#include "graphics/view/TFT/TextareaEditContext.h"
#include "input/policy/InputContextState.h"

void TextareaEditContext::focus(lv_obj_t *textarea)
{
    auto &context = input_policy::InputContextState::instance();
    context.setFocusSemantic(lv_textarea_get_one_line(textarea) ? input_policy::FocusSemantic::TextEdit
                                                                : input_policy::FocusSemantic::TextAreaMultiLine);
    context.setEditMode(true);
    context.setCanLeaveEditMode(true);
}
