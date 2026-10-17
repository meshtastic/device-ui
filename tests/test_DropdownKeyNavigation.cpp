#include "graphics/view/TFT/DropdownKeyNavigation.h"
#include "input/policy/InputContextState.h"

#include <doctest/doctest.h>

#include <vector>

using input_policy::FocusSemantic;
using input_policy::InputContextState;

namespace
{
void ensureLvgl()
{
    if (!lv_is_initialized())
        lv_init();
    if (!lv_display_get_default())
        lv_display_create(320, 240);
}

void recordKey(lv_event_t *e)
{
    static_cast<std::vector<uint32_t> *>(lv_event_get_user_data(e))->push_back(lv_event_get_key(e));
}

// Dropdown inside a parent that records the keys bubbling up to it (the settings dialog in the real UI).
struct DropdownFixture {
    lv_obj_t *screen = nullptr;
    lv_obj_t *parent = nullptr;
    lv_obj_t *dropdown = nullptr;
    std::vector<uint32_t> parentKeys;

    DropdownFixture()
    {
        ensureLvgl();
        screen = lv_obj_create(nullptr);
        parent = lv_obj_create(screen);
        lv_obj_add_event_cb(parent, recordKey, LV_EVENT_KEY, &parentKeys);
        dropdown = lv_dropdown_create(parent);
        lv_dropdown_set_options(dropdown, "one\ntwo\nthree");
        lv_obj_add_flag(dropdown, LV_OBJ_FLAG_EVENT_BUBBLE);
        DropdownKeyNavigation::attach(screen);

        auto &ctx = InputContextState::instance();
        ctx.setFocusSemantic(FocusSemantic::Unknown);
        ctx.setEditMode(false);
        ctx.setCanLeaveEditMode(false);
    }

    ~DropdownFixture() { lv_obj_delete(screen); }

    void sendKey(uint32_t key) { lv_obj_send_event(dropdown, LV_EVENT_KEY, &key); }
};
} // namespace

TEST_CASE("opening a dropdown enters edit mode and closing restores the previous context")
{
    DropdownFixture f;
    InputContextState::instance().setFocusSemantic(FocusSemantic::Map);

    lv_dropdown_open(f.dropdown);
    auto ctx = InputContextState::instance().getSnapshot();
    CHECK(ctx.focusSemantic == FocusSemantic::Dropdown);
    CHECK(ctx.isEditMode);
    CHECK(ctx.canLeaveEditMode);

    lv_dropdown_close(f.dropdown);
    ctx = InputContextState::instance().getSnapshot();
    CHECK(ctx.focusSemantic == FocusSemantic::Map);
    CHECK_FALSE(ctx.isEditMode);
    CHECK_FALSE(ctx.canLeaveEditMode);
}

TEST_CASE("left/right do not open a closed dropdown but still bubble to its parent")
{
    DropdownFixture f;

    f.sendKey(LV_KEY_LEFT);
    f.sendKey(LV_KEY_RIGHT);

    CHECK_FALSE(lv_dropdown_is_open(f.dropdown));
    CHECK(InputContextState::instance().getSnapshot().focusSemantic == FocusSemantic::Unknown);
    const std::vector<uint32_t> expected{LV_KEY_LEFT, LV_KEY_RIGHT};
    CHECK(f.parentKeys == expected);
}

TEST_CASE("up/down move the selection of an open dropdown while left/right are swallowed")
{
    DropdownFixture f;
    lv_dropdown_open(f.dropdown);

    f.sendKey(LV_KEY_DOWN);
    f.sendKey(LV_KEY_DOWN);
    CHECK(lv_dropdown_get_selected(f.dropdown) == 2u);
    f.sendKey(LV_KEY_UP);
    CHECK(lv_dropdown_get_selected(f.dropdown) == 1u);

    f.sendKey(LV_KEY_LEFT);
    f.sendKey(LV_KEY_RIGHT);
    CHECK(lv_dropdown_get_selected(f.dropdown) == 1u);
    CHECK(lv_dropdown_is_open(f.dropdown));
    const std::vector<uint32_t> expected{LV_KEY_DOWN, LV_KEY_DOWN, LV_KEY_UP};
    CHECK(f.parentKeys == expected);
}

TEST_CASE("escape closes an open dropdown without reaching the surrounding dialog")
{
    DropdownFixture f;
    lv_dropdown_open(f.dropdown);
    f.sendKey(LV_KEY_DOWN);
    f.parentKeys.clear();

    f.sendKey(LV_KEY_ESC);
    CHECK_FALSE(lv_dropdown_is_open(f.dropdown));
    CHECK(lv_dropdown_get_selected(f.dropdown) == 0u);
    CHECK_FALSE(InputContextState::instance().getSnapshot().isEditMode);
    CHECK(f.parentKeys.empty());

    // a second ESC on the closed dropdown is meant for the dialog
    f.sendKey(LV_KEY_ESC);
    const std::vector<uint32_t> expected{LV_KEY_ESC};
    CHECK(f.parentKeys == expected);
}

TEST_CASE("escape on a closed non-bubbling dropdown still reaches its parent")
{
    DropdownFixture f;
    lv_obj_remove_flag(f.dropdown, LV_OBJ_FLAG_EVENT_BUBBLE);

    f.sendKey(LV_KEY_LEFT);
    CHECK(f.parentKeys.empty());

    f.sendKey(LV_KEY_ESC);
    const std::vector<uint32_t> expected{LV_KEY_ESC};
    CHECK(f.parentKeys == expected);

    lv_dropdown_open(f.dropdown);
    f.parentKeys.clear();
    f.sendKey(LV_KEY_ESC);
    CHECK_FALSE(lv_dropdown_is_open(f.dropdown));
    CHECK(f.parentKeys.empty());
}

TEST_CASE("deleting an open dropdown leaves edit mode")
{
    DropdownFixture f;
    lv_dropdown_open(f.dropdown);

    lv_obj_delete(f.dropdown);

    const auto ctx = InputContextState::instance().getSnapshot();
    CHECK(ctx.focusSemantic == FocusSemantic::Unknown);
    CHECK_FALSE(ctx.isEditMode);
}
