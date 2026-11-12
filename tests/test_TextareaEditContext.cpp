#include "graphics/view/TFT/TextareaEditContext.h"
#include "input/policy/DefaultInputPolicyFactory.h"
#include "input/policy/InputContextState.h"
#include "input/policy/InputPipeline.h"
#include "input/policy/InputSourceRegistry.h"
#include "input/policy/UICommandDispatcher.h"
#include <doctest/doctest.h>

namespace
{
struct TextareaFixture {
    lv_obj_t *screen;
    lv_obj_t *textarea;
    input_policy::InputContextSnapshot previousContext;
    std::shared_ptr<input_policy::InputPipeline> pipeline;

    TextareaFixture()
    {
        if (!lv_is_initialized())
            lv_init();
        if (!lv_display_get_default())
            lv_display_create(320, 240);
        screen = lv_obj_create(nullptr);
        textarea = lv_textarea_create(screen);
        previousContext = input_policy::InputContextState::instance().getSnapshot();

        auto context = std::shared_ptr<input_policy::IInputContextProvider>(&input_policy::InputContextState::instance(),
                                                                            [](input_policy::IInputContextProvider *) {});
        auto dispatcher = std::shared_ptr<input_policy::IUICommandDispatcher>(&input_policy::UICommandDispatcher::instance(),
                                                                              [](input_policy::IUICommandDispatcher *) {});
        input_policy::InputSourceRegistry registry;
        input_policy::DefaultInputPolicyFactory factory;
        auto result = factory.build(registry, context, dispatcher);
        pipeline = std::make_shared<input_policy::InputPipeline>(result.bindingResolver, context, dispatcher);
        pipeline->setPolicyChain(std::move(result.chain));
    }

    ~TextareaFixture()
    {
        input_policy::InputContextState::instance().setSnapshot(previousContext);
        lv_obj_delete(screen);
    }

    uint32_t processedKey(uint32_t key)
    {
        input_policy::InputEvent event{};
        event.sourceId = "textarea-test";
        event.rawKeyCode = event.resolvedKeyCode = key;
        std::vector<input_policy::InputEvent> output;
        CHECK(pipeline->process(event, input_policy::InputCapabilities{}, output));
        REQUIRE(output.size() == 1);
        return output.front().resolvedKeyCode;
    }
};
} // namespace

TEST_CASE("multi-line textarea focus keeps vertical arrows for cursor movement")
{
    TextareaFixture f;
    lv_textarea_set_text(f.textarea, "first line\nsecond line");
    lv_obj_set_size(f.textarea, 240, 80);
    lv_obj_update_layout(f.textarea);
    lv_textarea_set_cursor_pos(f.textarea, LV_TEXTAREA_CURSOR_LAST);
    const uint32_t originalPosition = lv_textarea_get_cursor_pos(f.textarea);
    TextareaEditContext::focus(f.textarea);
    const auto context = input_policy::InputContextState::instance().getSnapshot();
    CHECK(context.focusSemantic == input_policy::FocusSemantic::TextAreaMultiLine);
    CHECK(context.isEditMode);
    CHECK(context.canLeaveEditMode);

    uint32_t key = f.processedKey(LV_KEY_UP);
    CHECK(key == LV_KEY_UP);
    lv_obj_send_event(f.textarea, LV_EVENT_KEY, &key);
    CHECK(lv_textarea_get_cursor_pos(f.textarea) < originalPosition);
    CHECK(f.processedKey(LV_KEY_DOWN) == LV_KEY_DOWN);
}

TEST_CASE("one-line textarea focus keeps vertical focus navigation")
{
    TextareaFixture f;
    lv_textarea_set_one_line(f.textarea, true);
    TextareaEditContext::focus(f.textarea);
    const auto context = input_policy::InputContextState::instance().getSnapshot();
    CHECK(context.focusSemantic == input_policy::FocusSemantic::TextEdit);
    CHECK(context.isEditMode);
    CHECK(context.canLeaveEditMode);
    CHECK(f.processedKey(LV_KEY_UP) == LV_KEY_PREV);
    CHECK(f.processedKey(LV_KEY_DOWN) == LV_KEY_NEXT);

    lv_textarea_set_one_line(f.textarea, false);
    TextareaEditContext::focus(f.textarea);
    CHECK(f.processedKey(LV_KEY_UP) == LV_KEY_UP);
}
