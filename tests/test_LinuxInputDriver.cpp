#include "input/LinuxInputDriver.h"
#include "input/policy/InputContextState.h"
#include <doctest/doctest.h>

#if defined(ARCH_PORTDUINO) && LV_USE_LIBINPUT
namespace
{
lv_indev_data_t physicalInput{};
unsigned deletedDevices = 0;

void readPhysicalInput(lv_indev_t *, lv_indev_data_t *data)
{
    *data = physicalInput;
}

class TestLinuxInputDriver : public LinuxInputDriver
{
  public:
    using LinuxInputDriver::LinuxInputDriver;
    static void reset()
    {
        if (inputGroup)
            lv_group_delete(inputGroup);
        inputGroup = nullptr;
        driver = nullptr;
    }
};

struct KeyboardFixture {
    TestLinuxInputDriver driver;
    lv_obj_t *screen;
    lv_obj_t *previousScreen;
    lv_obj_t *first;
    lv_obj_t *second;

    explicit KeyboardFixture(bool navigation = true) : driver("event-test", "", navigation)
    {
        if (!lv_is_initialized())
            lv_init();
        if (!lv_display_get_default())
            lv_display_create(320, 240);
        driver.init();
        previousScreen = lv_screen_active();
        screen = lv_obj_create(nullptr);
        first = lv_button_create(screen);
        second = lv_button_create(screen);
        lv_group_add_obj(driver.getInputGroup(), first);
        lv_group_add_obj(driver.getInputGroup(), second);
        lv_group_focus_obj(first);
        lv_screen_load(screen);
        setContext(input_policy::FocusSemantic::Button);
    }

    ~KeyboardFixture()
    {
        driver.releaseKeyboardDevice();
        TestLinuxInputDriver::reset();
        lv_screen_load(previousScreen);
        lv_obj_delete(screen);
    }

    void setContext(input_policy::FocusSemantic semantic, bool editing = false)
    {
        input_policy::InputContextSnapshot context{};
        context.focusSemantic = semantic;
        context.isEditMode = editing;
        input_policy::InputContextState::instance().setSnapshot(context);
    }

    lv_indev_data_t read(uint32_t key, lv_indev_state_t state)
    {
        physicalInput = {};
        physicalInput.key = key;
        physicalInput.state = state;
        lv_indev_data_t result{};
        lv_indev_get_read_cb(driver.getKeyboard())(driver.getKeyboard(), &result);
        return result;
    }
};
} // namespace

// Keep physical input deterministic while exercising the production wrapper and LVGL groups.
extern "C" lv_indev_t *lv_libinput_create(lv_indev_type_t type, const char *)
{
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, type);
    lv_indev_set_read_cb(indev, readPhysicalInput);
    return indev;
}

extern "C" void lv_libinput_delete(lv_indev_t *indev)
{
    ++deletedDevices;
    lv_indev_delete(indev);
}

TEST_CASE("Linux contextual arrows traverse LVGL focus")
{
    KeyboardFixture f;
    physicalInput.key = LV_KEY_DOWN;
    physicalInput.state = LV_INDEV_STATE_PRESSED;
    lv_indev_read(f.driver.getKeyboard());
    CHECK(lv_group_get_focused(f.driver.getInputGroup()) == f.second);
    f.read(LV_KEY_DOWN, LV_INDEV_STATE_RELEASED);
}

TEST_CASE("Linux raw keyboard mode preserves the physical key")
{
    KeyboardFixture raw(false);
    CHECK(raw.read(LV_KEY_DOWN, LV_INDEV_STATE_PRESSED).key == LV_KEY_DOWN);
    CHECK(raw.read(LV_KEY_DOWN, LV_INDEV_STATE_RELEASED).key == LV_KEY_DOWN);
}

TEST_CASE("Linux keyboard preserves edit contexts and the matching release across focus changes")
{
    using input_policy::FocusSemantic;
    KeyboardFixture f;
    for (auto semantic : {FocusSemantic::Map, FocusSemantic::TextAreaMultiLine, FocusSemantic::Scrollable,
                          FocusSemantic::Keyboard, FocusSemantic::ButtonMatrix}) {
        f.setContext(semantic);
        CHECK(f.read(LV_KEY_DOWN, LV_INDEV_STATE_PRESSED).key == LV_KEY_DOWN);
        CHECK(f.read(LV_KEY_DOWN, LV_INDEV_STATE_RELEASED).key == LV_KEY_DOWN);
    }
    f.setContext(FocusSemantic::Dropdown, true);
    CHECK(f.read(LV_KEY_UP, LV_INDEV_STATE_PRESSED).key == LV_KEY_UP);
    f.setContext(FocusSemantic::Button);
    CHECK(f.read(LV_KEY_UP, LV_INDEV_STATE_RELEASED).key == LV_KEY_UP);
    CHECK(f.read(LV_KEY_UP, LV_INDEV_STATE_PRESSED).key == LV_KEY_PREV);
    f.setContext(FocusSemantic::Map);
    CHECK(f.read(LV_KEY_UP, LV_INDEV_STATE_PRESSED).key == LV_KEY_PREV);
    CHECK(f.read(LV_KEY_UP, LV_INDEV_STATE_RELEASED).key == LV_KEY_PREV);
}

TEST_CASE("Linux keyboard treats printable command-code overlaps as text and cleans up reconnects")
{
    KeyboardFixture f;
    for (uint32_t key : {uint32_t('!'), uint32_t('"'), uint32_t(0x100), uint32_t(0x1f600)}) {
        CHECK(f.read(key, LV_INDEV_STATE_PRESSED).key == key);
        CHECK(f.read(key, LV_INDEV_STATE_RELEASED).key == key);
    }
    const unsigned before = deletedDevices;
    CHECK(f.driver.useKeyboardDevice("event-reconnected"));
    CHECK(deletedDevices == before + 1);
    CHECK(f.read(LV_KEY_DOWN, LV_INDEV_STATE_PRESSED).key == LV_KEY_NEXT);
}
#endif
