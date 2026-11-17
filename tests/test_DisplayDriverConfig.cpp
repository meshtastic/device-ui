#include "graphics/driver/DisplayDriverConfig.h"
#include <doctest/doctest.h>
#include <utility>

TEST_CASE("native input and navigation configuration can override independent defaults")
{
    DisplayDriverConfig config(DisplayDriverConfig::device_t::FB, 320, 170);
    CHECK(config.keyboardNavigation());
    CHECK(config.cursorMode() == DisplayDriverConfig::cursor_mode_t::AUTO);
    CHECK(config.navigation().scrollbarMode == DisplayDriverConfig::scrollbar_mode_t::AUTO);

    DisplayDriverConfig::input_config_t input;
    input.keyboardDevice = "/dev/input/by-path/keyboard-event";
    input.pointerDevice = "event-mouse";
    input.keyboardNavigation = false;
    input.cursorMode = DisplayDriverConfig::cursor_mode_t::NEVER;
    DisplayDriverConfig::navigation_config_t navigation;
    navigation.scrollbarMode = DisplayDriverConfig::scrollbar_mode_t::OFF;
    config.input(std::move(input)).navigation(std::move(navigation));

    CHECK_FALSE(config.keyboardNavigation());
    CHECK(config.cursorMode() == DisplayDriverConfig::cursor_mode_t::NEVER);
    CHECK(config.keyboard() == "/dev/input/by-path/keyboard-event");
    CHECK(config.pointer() == "event-mouse");
    CHECK(config.navigation().scrollbarMode == DisplayDriverConfig::scrollbar_mode_t::OFF);
}
