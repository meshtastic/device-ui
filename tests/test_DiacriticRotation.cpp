#include "input/DiacriticRotation.h"

#include <doctest/doctest.h>

#include <string>

namespace
{
struct TextareaFixture {
    lv_obj_t *screen;
    lv_obj_t *textarea;

    TextareaFixture()
    {
        if (!lv_is_initialized())
            lv_init();
        if (!lv_display_get_default())
            lv_display_create(320, 240);
        screen = lv_obj_create(nullptr);
        textarea = lv_textarea_create(screen);
        DiacriticRotation::attach(screen);
    }

    ~TextareaFixture() { lv_obj_delete(screen); }

    void rotate()
    {
        uint32_t key = LV_KEY_CUSTOM_DIACRITIC;
        lv_obj_send_event(textarea, LV_EVENT_KEY, &key);
    }
};
}

TEST_CASE("diacritic key rotates UTF-8 variants and preserves case")
{
    TextareaFixture fixture;
    lv_textarea_set_text(fixture.textarea, "a");
    fixture.rotate();
    CHECK(std::string(lv_textarea_get_text(fixture.textarea)) == "\xC3\xA4");
    fixture.rotate();
    CHECK(std::string(lv_textarea_get_text(fixture.textarea)) == "\xC3\xA0");
    CHECK(lv_textarea_get_cursor_pos(fixture.textarea) == 1u);

    lv_textarea_set_text(fixture.textarea, "\xC3\x86");
    fixture.rotate();
    CHECK(std::string(lv_textarea_get_text(fixture.textarea)) == "A");
    fixture.rotate();
    CHECK(std::string(lv_textarea_get_text(fixture.textarea)) == "\xC3\x84");
}

TEST_CASE("diacritic rotation replaces only the character before the cursor")
{
    TextareaFixture fixture;
    lv_textarea_set_text(fixture.textarea, "\xF0\x9F\x98\x80\xE2\x82\xAC" "c!");
    lv_textarea_set_cursor_pos(fixture.textarea, 3);
    fixture.rotate();
    CHECK(std::string(lv_textarea_get_text(fixture.textarea)) == "\xF0\x9F\x98\x80\xE2\x82\xAC\xC3\xA7!");
    CHECK(lv_textarea_get_cursor_pos(fixture.textarea) == 3u);
}

TEST_CASE("diacritic rotation leaves unsupported characters and cursor zero unchanged")
{
    TextareaFixture fixture;
    for (const char *text : {"", "!", "\xE2\x82\xAC", "\xF0\x9F\x98\x80"}) {
        lv_textarea_set_text(fixture.textarea, text);
        fixture.rotate();
        CHECK(std::string(lv_textarea_get_text(fixture.textarea)) == text);
    }
    lv_textarea_set_text(fixture.textarea, "a");
    lv_textarea_set_cursor_pos(fixture.textarea, 0);
    fixture.rotate();
    CHECK(std::string(lv_textarea_get_text(fixture.textarea)) == "a");
    CHECK(lv_textarea_get_cursor_pos(fixture.textarea) == 0u);
}

TEST_CASE("diacritic rotation respects accepted characters and character limits")
{
    TextareaFixture fixture;
    lv_textarea_set_accepted_chars(fixture.textarea, "a");
    lv_textarea_set_text(fixture.textarea, "a");
    fixture.rotate();
    CHECK(std::string(lv_textarea_get_text(fixture.textarea)) == "a");

    lv_textarea_set_accepted_chars(fixture.textarea, nullptr);
    lv_textarea_set_max_length(fixture.textarea, 1);
    fixture.rotate();
    CHECK(std::string(lv_textarea_get_text(fixture.textarea)) == "\xC3\xA4");
}

TEST_CASE("diacritic rotation follows supplied ligature and three-byte character ordering")
{
    TextareaFixture fixture;
    const char *cycles[] = {
        "\x61\xC3\xA4\xC3\xA0\xC3\xA1\xC3\xA2\xC3\xA3\xC3\xA5\xC7\x8E\xC4\x83\xC4\x85\xC3\xA6",
        "\x65\xC3\xAB\xC3\xA8\xC3\xA9\xC3\xAA\xC4\x9B\xC4\x99\xC4\x95\xC4\x97",
        "\x69\xC3\xAF\xC3\xAC\xC3\xAD\xC3\xAE\xC4\xB1\xC4\xAB",
        "\x6F\xC3\xB6\xC3\xB2\xC3\xB3\xC3\xB4\xC3\xB5\xC3\xB8\xC5\x91\xC5\x93",
        "\x75\xC3\xBC\xC3\xB9\xC3\xBA\xC3\xBB\xC5\xAF\xC5\xB1\xC7\x94\xC5\xAB",
        "\x79\xC3\xBF\xE1\xBB\xB3\xC3\xBD\xC5\xB7",
        "\x63\xC3\xA7\xC4\x87\xC4\x8D\xC4\x89\xC4\x8B",
        "\x6E\xC3\xB1\xC5\x84\xC5\x88",
        "\x73\xC3\x9F\xC5\x9B\xC5\xA1\xC5\x9F\xC8\x99\xC5\x9D",
        "\x7A\xC5\xBA\xC5\xBE\xC5\xBC",
        "\x6C\xC5\x82\xC4\xBA\xC4\xBE",
        "\x72\xC5\x95\xC5\x99",
        "\x64\xC4\x8F\xC3\xB0\xC4\x91",
        "\x74\xC5\xA5\xC8\x9B\xC5\xA3\xC3\xBE",
        "\x41\xC3\x84\xC3\x80\xC3\x81\xC3\x82\xC3\x83\xC3\x85\xC7\x8D\xC4\x82\xC4\x84\xC3\x86",
        "\x45\xC3\x8A\xC3\x88\xC3\x89\xC3\x8B\xC4\x9A\xC4\x98\xC4\x94\xC4\x96",
        "\x49\xC3\x8F\xC3\x8C\xC3\x8D\xC3\x8E\xC4\xB0\xC4\xAA",
        "\x4F\xC3\x96\xC3\x92\xC3\x93\xC3\x94\xC3\x95\xC3\x98\xC5\x90\xC5\x92",
        "\x55\xC3\x9C\xC3\x99\xC3\x9A\xC3\x9B\xC5\xAE\xC5\xB0\xC7\x93\xC5\xAA",
        "\x59\xC5\xB8\xE1\xBB\xB2\xC3\x9D\xC5\xB6",
        "\x43\xC3\x87\xC4\x86\xC4\x8C\xC4\x88\xC4\x8A",
        "\x4E\xC3\x91\xC5\x83\xC5\x87",
        "\x53\xC5\x9A\xC5\xA0\xC5\x9E\xC8\x98\xC5\x9C",
        "\x5A\xC5\xB9\xC5\xBD\xC5\xBB",
        "\x4C\xC5\x81\xC4\xB9\xC4\xBD",
        "\x52\xC5\x94\xC5\x98",
        "\x44\xC4\x8E\xC3\x90\xC4\x90",
        "\x54\xC5\xA4\xC8\x9A\xC5\xA2\xC3\x9E",
    };

    for (const char *cycle : cycles) {
        const std::string expectedCycle = cycle;
        lv_textarea_set_text(fixture.textarea, expectedCycle.substr(0, 1).c_str());
        for (size_t offset = 1; offset < expectedCycle.size();) {
            const auto lead = static_cast<unsigned char>(expectedCycle[offset]);
            const size_t length = lead < 0xE0 ? 2 : 3;
            fixture.rotate();
            CHECK(std::string(lv_textarea_get_text(fixture.textarea)) == expectedCycle.substr(offset, length));
            CHECK(lv_textarea_get_cursor_pos(fixture.textarea) == 1u);
            offset += length;
        }
        fixture.rotate();
        CHECK(std::string(lv_textarea_get_text(fixture.textarea)) == expectedCycle.substr(0, 1));
    }
}