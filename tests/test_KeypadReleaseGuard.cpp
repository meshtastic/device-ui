#include "input/KeypadReleaseGuard.h"

#include <doctest/doctest.h>

#include <vector>

namespace
{
void ensureLvgl()
{
    if (!lv_is_initialized())
        lv_init();
    if (!lv_display_get_default())
        lv_display_create(320, 240);
}

void readEnterKey(lv_indev_t *indev, lv_indev_data_t *data)
{
    data->key = LV_KEY_ENTER;
    data->state = *static_cast<lv_indev_state_t *>(lv_indev_get_user_data(indev));
}

void recordEvent(lv_event_t *e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_LONG_PRESSED || code == LV_EVENT_RELEASED || code == LV_EVENT_CLICKED)
        static_cast<std::vector<lv_event_code_t> *>(lv_event_get_user_data(e))->push_back(code);
}

void focusUserData(lv_event_t *e)
{
    lv_group_focus_obj(static_cast<lv_obj_t *>(lv_event_get_user_data(e)));
}

// Two buttons in the group of a keypad whose ENTER key is driven by hand through lv_indev_read().
struct KeypadFixture {
    lv_indev_state_t enter = LV_INDEV_STATE_RELEASED;
    lv_indev_t *keypad = nullptr;
    lv_group_t *group = nullptr;
    lv_obj_t *screen = nullptr;
    lv_obj_t *first = nullptr;
    lv_obj_t *second = nullptr;
    std::vector<lv_event_code_t> firstEvents;
    std::vector<lv_event_code_t> secondEvents;

    KeypadFixture()
    {
        ensureLvgl();
        screen = lv_obj_create(nullptr);
        first = lv_button_create(screen);
        second = lv_button_create(screen);
        lv_obj_add_event_cb(first, recordEvent, LV_EVENT_ALL, &firstEvents);
        lv_obj_add_event_cb(second, recordEvent, LV_EVENT_ALL, &secondEvents);

        group = lv_group_create();
        lv_group_add_obj(group, first);
        lv_group_add_obj(group, second);
        lv_group_focus_obj(first);

        keypad = lv_indev_create();
        lv_indev_set_type(keypad, LV_INDEV_TYPE_KEYPAD);
        lv_indev_set_read_cb(keypad, readEnterKey);
        lv_indev_set_user_data(keypad, &enter);
        lv_indev_set_group(keypad, group);
        lv_indev_set_long_press_time(keypad, 10);
        KeypadReleaseGuard::attach(keypad);
        KeypadReleaseGuard::attach(keypad); // the view attaches again after keyboard hotplug
    }

    ~KeypadFixture()
    {
        lv_indev_delete(keypad);
        lv_group_delete(group);
        lv_obj_delete(screen);
    }

    void press()
    {
        enter = LV_INDEV_STATE_PRESSED;
        lv_indev_read(keypad);
    }

    void holdLong()
    {
        lv_tick_inc(50);
        lv_indev_read(keypad);
    }

    void release()
    {
        enter = LV_INDEV_STATE_RELEASED;
        lv_indev_read(keypad);
    }
};
} // namespace

TEST_CASE("keypad long press release reaches the long-pressed object after its handler moved the focus")
{
    KeypadFixture f;
    lv_obj_add_event_cb(f.first, focusUserData, LV_EVENT_LONG_PRESSED, f.second);

    f.press();
    f.holdLong();
    REQUIRE(lv_group_get_focused(f.group) == f.second);
    f.release();

    const std::vector<lv_event_code_t> longPress{LV_EVENT_LONG_PRESSED, LV_EVENT_RELEASED, LV_EVENT_CLICKED};
    CHECK(f.firstEvents == longPress);
    CHECK(f.secondEvents.empty());

    // the next ordinary press belongs to the newly focused object
    f.press();
    f.release();
    const std::vector<lv_event_code_t> click{LV_EVENT_RELEASED, LV_EVENT_CLICKED};
    CHECK(f.secondEvents == click);
    CHECK(f.firstEvents == longPress);
}

TEST_CASE("keypad long press without focus change is delivered unchanged")
{
    KeypadFixture f;

    f.press();
    f.holdLong();
    f.release();

    const std::vector<lv_event_code_t> longPress{LV_EVENT_LONG_PRESSED, LV_EVENT_RELEASED, LV_EVENT_CLICKED};
    CHECK(f.firstEvents == longPress);
    CHECK(f.secondEvents.empty());
}
