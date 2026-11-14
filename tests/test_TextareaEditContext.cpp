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
        TextareaEditContext::release(textarea);
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

TEST_CASE("textarea exit events clear edit context on every panel")
{
    TextareaFixture f;
    lv_obj_add_event_cb(f.textarea, TextareaEditContext::handleEvent, LV_EVENT_ALL, nullptr);
    auto &context = input_policy::InputContextState::instance();
    for (uint32_t panelId : {1u, 2u, 3u, 4u}) {
        // Messages, settings, nodes and setup follow the same lifecycle without panel-specific exceptions.
        context.setActivePanelId(panelId);
        for (auto event : {LV_EVENT_DEFOCUSED, LV_EVENT_LEAVE, LV_EVENT_READY, LV_EVENT_CANCEL}) {
            CAPTURE(panelId);
            CAPTURE(event);
            lv_obj_send_event(f.textarea, LV_EVENT_FOCUSED, nullptr);
            lv_obj_send_event(f.textarea, event, nullptr);
            const auto snapshot = context.getSnapshot();
            CHECK(snapshot.focusSemantic == input_policy::FocusSemantic::Unknown);
            CHECK_FALSE(snapshot.isEditMode);
            CHECK_FALSE(snapshot.canLeaveEditMode);
        }
    }
}

TEST_CASE("delayed textarea exits preserve a new panel or focused widget context")
{
    TextareaFixture f;
    lv_obj_t *newTextarea = lv_textarea_create(f.screen);
    lv_obj_add_event_cb(f.textarea, TextareaEditContext::handleEvent, LV_EVENT_ALL, nullptr);
    lv_obj_add_event_cb(newTextarea, TextareaEditContext::handleEvent, LV_EVENT_ALL, nullptr);
    auto &context = input_policy::InputContextState::instance();
    context.setActivePanelId(1);
    lv_obj_send_event(f.textarea, LV_EVENT_FOCUSED, nullptr);

    SUBCASE("panel transition")
    {
        context.setActivePanelId(2);
        context.setFocusSemantic(input_policy::FocusSemantic::Map);
        context.setEditMode(false);
        context.setCanLeaveEditMode(false);
        lv_obj_send_event(f.textarea, LV_EVENT_DEFOCUSED, nullptr);
        CHECK(context.getSnapshot().focusSemantic == input_policy::FocusSemantic::Map);
        CHECK(context.getSnapshot().activePanelId == 2);
    }
    SUBCASE("panel changes before its focus semantic")
    {
        context.setActivePanelId(2);
        lv_obj_send_event(f.textarea, LV_EVENT_DEFOCUSED, nullptr);
        CHECK(context.getSnapshot().focusSemantic == input_policy::FocusSemantic::TextAreaMultiLine);
        CHECK(context.getSnapshot().isEditMode);
        CHECK(context.getSnapshot().canLeaveEditMode);
    }
    SUBCASE("new editor in the same panel")
    {
        // Both editors use the same semantic; ownership, rather than the semantic alone, distinguishes them.
        lv_obj_send_event(newTextarea, LV_EVENT_FOCUSED, nullptr);
        lv_obj_send_event(f.textarea, LV_EVENT_DEFOCUSED, nullptr);
        CHECK(context.getSnapshot().focusSemantic == input_policy::FocusSemantic::TextAreaMultiLine);
        CHECK(context.getSnapshot().isEditMode);
        CHECK(context.getSnapshot().canLeaveEditMode);
        lv_obj_send_event(newTextarea, LV_EVENT_DEFOCUSED, nullptr);
        CHECK(context.getSnapshot().focusSemantic == input_policy::FocusSemantic::Unknown);
    }
    SUBCASE("virtual keyboard in the same panel")
    {
        context.setFocusSemantic(input_policy::FocusSemantic::Keyboard);
        lv_obj_send_event(f.textarea, LV_EVENT_DEFOCUSED, nullptr);
        CHECK(context.getSnapshot().focusSemantic == input_policy::FocusSemantic::Keyboard);
    }
    SUBCASE("escape already left editing")
    {
        context.setFocusSemantic(input_policy::FocusSemantic::Unknown);
        context.setEditMode(false);
        context.setCanLeaveEditMode(false);
        lv_obj_send_event(f.textarea, LV_EVENT_CANCEL, nullptr);
        CHECK(context.getSnapshot().focusSemantic == input_policy::FocusSemantic::Unknown);
        CHECK_FALSE(context.getSnapshot().isEditMode);
        CHECK_FALSE(context.getSnapshot().canLeaveEditMode);
    }
    TextareaEditContext::release(newTextarea);
}

TEST_CASE("screen blank and wake restoration re-establishes textarea edit context")
{
    TextareaFixture f;
    lv_group_t *group = lv_group_create();
    lv_textarea_set_one_line(f.textarea, true);
    lv_obj_add_event_cb(f.textarea, TextareaEditContext::handleEvent, LV_EVENT_ALL, nullptr);
    lv_group_add_obj(group, f.textarea);

    auto &context = input_policy::InputContextState::instance();
    context.setActivePanelId(1);
    lv_group_focus_obj(f.textarea);

    CHECK(lv_group_get_focused(group) == f.textarea);
    CHECK(context.getSnapshot().focusSemantic == input_policy::FocusSemantic::TextEdit);
    CHECK(context.getSnapshot().isEditMode);

    // Simulate screen unload during blankScreen(true) / screenSaving(true):
    // LVGL clears focus state on the unloaded screen's widgets and sends LV_EVENT_DEFOCUSED
    // while leaving the group's focused object pointer intact.
    lv_obj_remove_state(f.textarea, LV_STATE_FOCUSED | LV_STATE_FOCUS_KEY | LV_STATE_EDITED);
    lv_obj_send_event(f.textarea, LV_EVENT_DEFOCUSED, nullptr);
    CHECK(lv_group_get_focused(group) == f.textarea);
    CHECK(context.getSnapshot().focusSemantic == input_policy::FocusSemantic::Unknown);
    CHECK_FALSE(context.getSnapshot().isEditMode);

    // Simulate restoreActivePanelFocus(): reset panel context first, then re-focus the group's last focused object.
    context.setFocusSemantic(input_policy::FocusSemantic::Unknown);
    lv_group_focus_obj(lv_group_get_focused(group));

    CHECK(lv_obj_has_state(f.textarea, LV_STATE_FOCUSED));
    CHECK(context.getSnapshot().focusSemantic == input_policy::FocusSemantic::TextEdit);
    CHECK(context.getSnapshot().isEditMode);
    CHECK(context.getSnapshot().canLeaveEditMode);
    CHECK(f.processedKey(LV_KEY_UP) == LV_KEY_PREV);

    lv_group_delete(group);
}
