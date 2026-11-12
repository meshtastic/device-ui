#include "input/DiacriticRotation.h"

#include <cstring>

namespace
{
constexpr const char *sequences[] = {
    "\x61\xC3\xA4\xC3\xA0\xC3\xA1\xC3\xA2\xC3\xA3\xC3\xA5\xC7\x8E\xC4\x83\xC4\x85\xC3\xA6", // a ä à á â ã å ǎ ă ą æ
    "\x65\xC3\xAB\xC3\xA8\xC3\xA9\xC3\xAA\xC4\x9B\xC4\x99\xC4\x95\xC4\x97",                 // e ë è é ê ě ę ĕ ė
    "\x69\xC3\xAF\xC3\xAC\xC3\xAD\xC3\xAE\xC4\xB1\xC4\xAB",                                 // i ï ì í î ǐ ı ī
    "\x6F\xC3\xB6\xC3\xB2\xC3\xB3\xC3\xB4\xC3\xB5\xC3\xB8\xC5\x91\xC5\x93",                 // o ö ò ó ô õ ø ő œ
    "\x75\xC3\xBC\xC3\xB9\xC3\xBA\xC3\xBB\xC5\xAF\xC5\xB1\xC7\x94\xC5\xAB",                 // u ü ù ú û ů ű ǔ ū
    "\x79\xC3\xBF\xE1\xBB\xB3\xC3\xBD\xC5\xB7",                                             // y ÿ ỳ ý ŷ
    "\x63\xC3\xA7\xC4\x87\xC4\x8D\xC4\x89\xC4\x8B",                                         // c ç ć č ĉ ċ
    "\x6E\xC3\xB1\xC5\x84\xC5\x88",                                                         // n ñ ń ň
    "\x73\xC3\x9F\xC5\x9B\xC5\xA1\xC5\x9F\xC8\x99\xC5\x9D",                                 // s ß ś š ş ș ŝ
    "\x7A\xC5\xBA\xC5\xBE\xC5\xBC",                                                         // z ź ž ż
    "\x6C\xC5\x82\xC4\xBA\xC4\xBE",                                                         // l ł ĺ ľ
    "\x72\xC5\x95\xC5\x99",                                                                 // r ŕ ř
    "\x64\xC4\x8F\xC3\xB0\xC4\x91",                                                         // d ď ð đ
    "\x74\xC5\xA5\xC8\x9B\xC5\xA3\xC3\xBE",                                                 // t ť ț ţ þ
    "\x41\xC3\x84\xC3\x80\xC3\x81\xC3\x82\xC3\x83\xC3\x85\xC7\x8D\xC4\x82\xC4\x84\xC3\x86", // A Ä À Á Â Ã Å Ǎ Ă Ą Æ
    "\x45\xC3\x8A\xC3\x88\xC3\x89\xC3\x8B\xC4\x9A\xC4\x98\xC4\x94\xC4\x96",                 // E Ê È É Ë Ě Ę Ĕ Ė
    "\x49\xC3\x8F\xC3\x8C\xC3\x8D\xC3\x8E\xC4\xB0\xC4\xAA",                                 // I Ï Ì Í Î İ Ī
    "\x4F\xC3\x96\xC3\x92\xC3\x93\xC3\x94\xC3\x95\xC3\x98\xC5\x90\xC5\x92",                 // O Ö Ò Ó Ô Õ Ø Ő Œ
    "\x55\xC3\x9C\xC3\x99\xC3\x9A\xC3\x9B\xC5\xAE\xC5\xB0\xC7\x93\xC5\xAA",                 // U Ü Ù Ú Û Ů Ű Ǔ Ū
    "\x59\xC5\xB8\xE1\xBB\xB2\xC3\x9D\xC5\xB6",                                             // Y Ÿ Ỳ Ý Ŷ
    "\x43\xC3\x87\xC4\x86\xC4\x8C\xC4\x88\xC4\x8A",                                         // C Ç Ć Č Ĉ Ċ
    "\x4E\xC3\x91\xC5\x83\xC5\x87",                                                         // N Ñ Ń Ň
    "\x53\xC5\x9A\xC5\xA0\xC5\x9E\xC8\x98\xC5\x9C",                                         // S Ś Š Ş Ș Ŝ
    "\x5A\xC5\xB9\xC5\xBD\xC5\xBB",                                                         // Z Ź Ž Ż
    "\x4C\xC5\x81\xC4\xB9\xC4\xBD",                                                         // L Ł Ĺ Ľ
    "\x52\xC5\x94\xC5\x98",                                                                 // R Ŕ Ř
    "\x44\xC4\x8E\xC3\x90\xC4\x90",                                                         // D Ď Ð Đ
    "\x54\xC5\xA4\xC8\x9A\xC5\xA2\xC3\x9E",                                                 // T Ť Ț Ţ Þ
};

const char *nextCharacter(const char *text)
{
    ++text;
    while ((static_cast<unsigned char>(*text) & 0xC0) == 0x80)
        ++text;
    return text;
}

void onKey(lv_event_t *event)
{
    if (lv_event_get_key(event) != LV_KEY_CUSTOM_DIACRITIC)
        return;
    lv_event_stop_processing(event);
    DiacriticRotation::rotatePreviousCharacter(lv_event_get_current_target_obj(event));
}
} // namespace

void DiacriticRotation::rotatePreviousCharacter(lv_obj_t *textarea)
{
    const uint32_t cursor = lv_textarea_get_cursor_pos(textarea);
    if (cursor == 0)
        return;

    const char *text = lv_textarea_get_text(textarea);
    const char *previous = text;
    for (uint32_t position = 0; position < cursor; ++position) {
        if (!*text)
            return;
        previous = text;
        text = nextCharacter(text);
    }
    const size_t previousLength = text - previous;

    for (const char *sequence : sequences) {
        for (const char *variant = sequence; *variant;) {
            const char *next = nextCharacter(variant);
            if (static_cast<size_t>(next - variant) == previousLength && std::memcmp(previous, variant, previousLength) == 0) {
                if (!*next)
                    next = sequence;
                const size_t nextLength = nextCharacter(next) - next;
                char replacement[5]{};
                std::memcpy(replacement, next, nextLength);

                const char *accepted = lv_textarea_get_accepted_chars(textarea);
                if (accepted && !std::strstr(accepted, replacement))
                    return;

                lv_textarea_delete_char(textarea);
                lv_textarea_add_text(textarea, replacement);
                return;
            }
            variant = next;
        }
    }
}

void DiacriticRotation::attach(lv_obj_t *root)
{
    lv_obj_tree_walk(
        root,
        [](lv_obj_t *object, void *) -> lv_obj_tree_walk_res_t {
            if (lv_obj_check_type(object, &lv_textarea_class)) {
                lv_obj_remove_event_cb(object, onKey);
                lv_obj_add_event_cb(object, onKey, static_cast<lv_event_code_t>(LV_EVENT_KEY | LV_EVENT_PREPROCESS), nullptr);
            }
            return LV_OBJ_TREE_WALK_NEXT;
        },
        nullptr);
}