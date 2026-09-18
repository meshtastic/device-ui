/**
 * @file  lv_t9_keyboard.c
 * @brief T9-style virtual keyboard for LVGL on ESP32 (480×222 TFT)
 *        — implementation —
 *
 * Compile this file exactly once.  Include lv_t9_keyboard.h wherever
 * you need the public API.
 *
 * Compatibility: LVGL >= 9.0  (tested against 9.3)
 *   v8 → v9 API changes applied:
 *   - lv_mem_alloc / lv_mem_free  → lv_malloc / lv_free
 *   - lv_coord_t                  → int32_t
 *   - lv_btn_create               → lv_button_create
 *   - lv_scr_act()                → lv_screen_active()
 *   - lv_timer_get_user_data(t)   → t->user_data
 *   - lv_timer_del                → lv_timer_delete
 *   - lv_obj_set_style_pad_gap    → lv_obj_set_style_pad_row/column_gap
 *
 * Key label format: single row, space-separated chars, e.g. "a b c 2"
 * This stays readable on small keys regardless of button height.
 *
 * Character commit: emits full UTF-8 byte sequences, so the European
 * supplemental layer (ä, ö, ü, …) works without any extra conversion.
 */

#include "input/lv_t9_keyboard.h"
#include "lvgl_private.h"
#include <string.h>

/* ═══════════════════════════════════════════════════════════════════════════
 * Keymaps
 *
 * *_MAP  strings  →  displayed on the key face, single row, space-separated
 * *_CHARS strings →  UTF-8 chars cycled on each tap, packed one after another
 *                    (each entry is a sequence of UTF-8 codepoints, no spaces)
 *                    NULL  →  purely functional key (⌫, ↵)
 * ═══════════════════════════════════════════════════════════════════════════ */

/* ── Alpha (a-z, digits, basic punctuation) ────────────────────────────── */

const char *const T9_ALPHA_MAP[T9_KEY_ROWS][T9_KEY_COLS] = {
    {". , ! ? ' 1", "a b c 2", "d e f 3"},
    {"g h i 4", "j k l 5", "m n o 6"},
    {"p q r s 7", "t u v 8", "w x y z 9"},
    {LV_SYMBOL_BACKSPACE, "space + 0", LV_SYMBOL_NEW_LINE},
};

const char *const T9_ALPHA_CHARS[T9_KEY_ROWS][T9_KEY_COLS] = {
    {".,!?'1", "abc2", "def3"},
    {"ghi4", "jkl5", "mno6"},
    {"pqrs7", "tuv8", "wxyz9"},
    {NULL, " +0", NULL},
};

/* ── ASCII special characters ──────────────────────────────────────────── */

const char *const T9_SPECIAL_MAP[T9_KEY_ROWS][T9_KEY_COLS] = {
    {"! \" #", "$ % &", "' ( )"},
    {"* + ,", "- . /", ": ; <"},
    {"= > ?", "@ [ \\ ]", "^ _ `"},
    {LV_SYMBOL_BACKSPACE, "{ | } ~", LV_SYMBOL_NEW_LINE},
};

const char *const T9_SPECIAL_CHARS[T9_KEY_ROWS][T9_KEY_COLS] = {
    {"!\"#", "$%&", "'()"},
    {"*+,", "-./", ":;<"},
    {"=>?", "@[\\]", "^_`"},
    {NULL, "{|}~", NULL},
};

/* ── Latin extended — 30 most common European supplemental characters ───
 *
 * Selection rationale: covers the full umlaut sets (German, Swedish, Finnish,
 * Hungarian), accented vowels (French, Spanish, Portuguese, Italian, Danish,
 * Norwegian), c-cedilla, n-tilde, and the ß ligature — together these handle
 * the vast majority of Western and Central European text.
 *
 * Each CHARS entry is a sequence of UTF-8 encoded codepoints packed together.
 * The cycle index is tracked as a UTF-8 character index, not a byte index.
 *
 * Lowercase / uppercase split — because the 3×4 grid has only 9 data keys:
 *   Rows 0-2 lowercase  (ä ö ü á é í ó ú ý ñ ç ß à è ì ò ù â ê î ô û)
 *   Shift active      →  uppercase variants emitted for the same keys
 *
 * Layout (shift off = lowercase):
 *   Key 1 (r0c0): ä  ö  ü
 *   Key 2 (r0c1): á  é  í
 *   Key 3 (r0c2): ó  ú  ý
 *   Key 4 (r1c0): ñ  ç  ß   (ß has no uppercase that differs visually in
 *                             common use; we cycle to SS on shift)
 *   Key 5 (r1c1): à  è  ì
 *   Key 6 (r1c2): ò  ù  å
 *   Key 7 (r2c0): â  ê  î
 *   Key 8 (r2c1): ô  û  ã
 *   Key 9 (r2c2): õ  æ  ø
 *
 * UTF-8 sequences used (all are 2-byte, U+00C0–U+00FF range):
 *   ä=\xC3\xA4  ö=\xC3\xB6  ü=\xC3\xBC
 *   Ä=\xC3\x84  Ö=\xC3\x96  Ü=\xC3\x9C
 *   á=\xC3\xA1  é=\xC3\xA9  í=\xC3\xAD
 *   Á=\xC3\x81  É=\xC3\x89  Í=\xC3\x8D
 *   ó=\xC3\xB3  ú=\xC3\xBA  ý=\xC3\xBD
 *   Ó=\xC3\x93  Ú=\xC3\x9A  Ý=\xC3\x9D
 *   ñ=\xC3\xB1  ç=\xC3\xA7  ß=\xC3\x9F
 *   Ñ=\xC3\x91  Ç=\xC3\x87
 *   à=\xC3\xA0  è=\xC3\xA8  ì=\xC3\xAC
 *   À=\xC3\x80  È=\xC3\x88  Ì=\xC3\x8C
 *   ò=\xC3\xB2  ù=\xC3\xB9  å=\xC3\xA5
 *   Ò=\xC3\x92  Ù=\xC3\x99  Å=\xC3\x85
 *   â=\xC3\xA2  ê=\xC3\xAA  î=\xC3\xAE
 *   Â=\xC3\x82  Ê=\xC3\x8A  Î=\xC3\x8E
 *   ô=\xC3\xB4  û=\xC3\xBB  ã=\xC3\xA3
 *   Ô=\xC3\x94  Û=\xC3\x9B  Ã=\xC3\x83
 *   õ=\xC3\xB5  æ=\xC3\xA6  ø=\xC3\xB8
 *   Õ=\xC3\x95  Æ=\xC3\x86  Ø=\xC3\x98
 * ─────────────────────────────────────────────────────────────────────── */

const char *const T9_LATIN_MAP[T9_KEY_ROWS][T9_KEY_COLS] = {
    {"\xC3\xA4 \xC3\xB6 \xC3\xBC", /* ä ö ü */
     "\xC3\xA1 \xC3\xA9 \xC3\xAD", /* á é í */
     "\xC3\xB3 \xC3\xBA \xC3\xBD" /* ó ú ý */},

    {"\xC3\xB1 \xC3\xA7 \xC3\x9F", /* ñ ç ß */
     "\xC3\xA0 \xC3\xA8 \xC3\xAC", /* à è ì */
     "\xC3\xB2 \xC3\xB9 \xC3\xA5" /* ò ù å */},

    {"\xC3\xA2 \xC3\xAA \xC3\xAE", /* â ê î */
     "\xC3\xB4 \xC3\xBB \xC3\xA3", /* ô û ã */
     "\xC3\xB5 \xC3\xA6 \xC3\xB8" /* õ æ ø */},

    {LV_SYMBOL_BACKSPACE, "0 + space", LV_SYMBOL_NEW_LINE},
};

/*
 * T9_LATIN_CHARS: lowercase cycle strings (UTF-8, 2 bytes per codepoint).
 * The shift path uses T9_LATIN_UPPER_CHARS defined below.
 */
const char *const T9_LATIN_CHARS[T9_KEY_ROWS][T9_KEY_COLS] = {
    {"\xC3\xA4\xC3\xB6\xC3\xBC", /* ä ö ü */
     "\xC3\xA1\xC3\xA9\xC3\xAD", /* á é í */
     "\xC3\xB3\xC3\xBA\xC3\xBD" /* ó ú ý */},

    {"\xC3\xB1\xC3\xA7\xC3\x9F", /* ñ ç ß */
     "\xC3\xA0\xC3\xA8\xC3\xAC", /* à è ì */
     "\xC3\xB2\xC3\xB9\xC3\xA5" /* ò ù å */},

    {"\xC3\xA2\xC3\xAA\xC3\xAE", /* â ê î */
     "\xC3\xB4\xC3\xBB\xC3\xA3", /* ô û ã */
     "\xC3\xB5\xC3\xA6\xC3\xB8" /* õ æ ø */},

    {NULL, " +0", NULL},
};

/* Uppercase variants for Latin mode + shift */
static const char *const T9_LATIN_UPPER_CHARS[T9_KEY_ROWS][T9_KEY_COLS] = {
    {"\xC3\x84\xC3\x96\xC3\x9C", /* Ä Ö Ü */
     "\xC3\x81\xC3\x89\xC3\x8D", /* Á É Í */
     "\xC3\x93\xC3\x9A\xC3\x9D" /* Ó Ú Ý */},

    {"\xC3\x91\xC3\x87SS",       /* Ñ Ç SS (ß→SS) */
     "\xC3\x80\xC3\x88\xC3\x8C", /* À È Ì */
     "\xC3\x92\xC3\x99\xC3\x85" /* Ò Ù Å */},

    {"\xC3\x82\xC3\x8A\xC3\x8E", /* Â Ê Î */
     "\xC3\x94\xC3\x9B\xC3\x83", /* Ô Û Ã */
     "\xC3\x95\xC3\x86\xC3\x98" /* Õ Æ Ø */},

    {NULL, " +0", NULL},
};

/* Uppercase display labels for Latin+shift mode */
static const char *const T9_LATIN_UPPER_MAP[T9_KEY_ROWS][T9_KEY_COLS] = {
    {"\xC3\x84 \xC3\x96 \xC3\x9C", /* Ä Ö Ü */
     "\xC3\x81 \xC3\x89 \xC3\x8D", /* Á É Í */
     "\xC3\x93 \xC3\x9A \xC3\x9D" /* Ó Ú Ý */},

    {"\xC3\x91 \xC3\x87 SS",       /* Ñ Ç SS */
     "\xC3\x80 \xC3\x88 \xC3\x8C", /* À È Ì */
     "\xC3\x92 \xC3\x99 \xC3\x85" /* Ò Ù Å */},

    {"\xC3\x82 \xC3\x8A \xC3\x8E", /* Â Ê Î */
     "\xC3\x94 \xC3\x9B \xC3\x83", /* Ô Û Ã */
     "\xC3\x95 \xC3\x86 \xC3\x98" /* Õ Æ Ø */},

    {LV_SYMBOL_BACKSPACE, "0 + space", LV_SYMBOL_NEW_LINE},
};

/* ═══════════════════════════════════════════════════════════════════════════
 * Styles (file-scope, initialised once)
 * ═══════════════════════════════════════════════════════════════════════════ */

static lv_style_t s_style_key;
static lv_style_t s_style_key_pressed;
static lv_style_t s_style_fn_key;
static lv_style_t s_style_mode_btn;
static bool s_styles_inited = false;

static void _init_styles(void)
{
    if (s_styles_inited)
        return;
    s_styles_inited = true;

    /* Normal alpha / special key */
    lv_style_init(&s_style_key);
    lv_style_set_bg_color(&s_style_key, lv_color_hex(0x2C3E50));
    lv_style_set_bg_opa(&s_style_key, LV_OPA_COVER);
    lv_style_set_border_color(&s_style_key, lv_color_hex(0x1A252F));
    lv_style_set_border_width(&s_style_key, 1);
    lv_style_set_radius(&s_style_key, 4);
    lv_style_set_text_color(&s_style_key, lv_color_white());
    lv_style_set_text_font(&s_style_key, &ui_font_montserrat_14);
    lv_style_set_text_align(&s_style_key, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_all(&s_style_key, 2);

    /* Pressed / active-tap highlight */
    lv_style_init(&s_style_key_pressed);
    lv_style_set_bg_color(&s_style_key_pressed, lv_color_hex(0x3498DB));
    lv_style_set_bg_opa(&s_style_key_pressed, LV_OPA_COVER);

    /* Functional keys (⌫ ↵) */
    lv_style_init(&s_style_fn_key);
    lv_style_set_bg_color(&s_style_fn_key, lv_color_hex(0x1A252F));
    lv_style_set_bg_opa(&s_style_fn_key, LV_OPA_COVER);
    lv_style_set_border_color(&s_style_fn_key, lv_color_hex(0x0D1520));
    lv_style_set_border_width(&s_style_fn_key, 1);
    lv_style_set_radius(&s_style_fn_key, 4);
    lv_style_set_text_color(&s_style_fn_key, lv_color_hex(0xECF0F1));
    lv_style_set_text_font(&s_style_fn_key, &ui_font_montserrat_14);
    lv_style_set_text_align(&s_style_fn_key, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_all(&s_style_fn_key, 2);

    /* Mode-cycle button (purple) */
    lv_style_init(&s_style_mode_btn);
    lv_style_set_bg_color(&s_style_mode_btn, lv_color_hex(0x8E44AD));
    lv_style_set_bg_opa(&s_style_mode_btn, LV_OPA_COVER);
    lv_style_set_border_color(&s_style_mode_btn, lv_color_hex(0x6C3483));
    lv_style_set_border_width(&s_style_mode_btn, 1);
    lv_style_set_radius(&s_style_mode_btn, 4);
    lv_style_set_text_color(&s_style_mode_btn, lv_color_white());
    lv_style_set_text_font(&s_style_mode_btn, &ui_font_montserrat_12);
    lv_style_set_text_align(&s_style_mode_btn, LV_TEXT_ALIGN_CENTER);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Internal helpers — forward declarations
 * ═══════════════════════════════════════════════════════════════════════════ */

static void _key_cb(lv_event_t *e);
static void _tap_timer_cb(lv_timer_t *t);
static void _enter_timer_cb(lv_timer_t *t);
static void _mode_btn_cb(lv_event_t *e);
static void _shift_btn_cb(lv_event_t *e);
static void _commit_char(t9_kb_t *kb);
static void _update_labels(t9_kb_t *kb);
static void _do_backspace(t9_kb_t *kb);
static void _do_enter(t9_kb_t *kb, int tap_count);

/* ── Return cycle-char table for current mode ──────────────────────────── */
static const char *const (*_cur_chars(t9_kb_t *kb))[T9_KEY_COLS]
{
    switch (kb->mode) {
    case T9_MODE_SPECIAL:
        return T9_SPECIAL_CHARS;
    case T9_MODE_LATIN:
        return kb->shift ? T9_LATIN_UPPER_CHARS : T9_LATIN_CHARS;
    default:
        return T9_ALPHA_CHARS;
    }
}

/* ── Return display-label table for current mode ───────────────────────── */
static const char *const (*_cur_map(t9_kb_t *kb))[T9_KEY_COLS]
{
    switch (kb->mode) {
    case T9_MODE_SPECIAL:
        return T9_SPECIAL_MAP;
    case T9_MODE_LATIN:
        return kb->shift ? T9_LATIN_UPPER_MAP : T9_LATIN_MAP;
    default:
        return T9_ALPHA_MAP;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 * UTF-8 helpers
 *
 * Characters in the cycle strings may be multi-byte (e.g. 2-byte for all
 * Latin-extended codepoints U+0080..U+07FF).  We therefore track the cycle
 * index as a codepoint index, not a byte index, and emit whole codepoints.
 * ═══════════════════════════════════════════════════════════════════════════ */

/** Return byte-length of the UTF-8 sequence starting at *p (1–4). */
static int _utf8_seqlen(unsigned char c)
{
    if (c < 0x80)
        return 1;
    if ((c & 0xE0) == 0xC0)
        return 2;
    if ((c & 0xF0) == 0xE0)
        return 3;
    return 4;
}

/** Count the number of UTF-8 codepoints in a NUL-terminated string. */
static int _utf8_len(const char *s)
{
    int n = 0;
    while (*s) {
        s += _utf8_seqlen((unsigned char)*s);
        n++;
    }
    return n;
}

/**
 * Return a pointer to the start of the idx-th UTF-8 codepoint in s,
 * and set *seq_bytes to the byte length of that codepoint.
 */
static const char *_utf8_at(const char *s, int idx, int *seq_bytes)
{
    int i = 0;
    while (*s && i < idx) {
        s += _utf8_seqlen((unsigned char)*s);
        i++;
    }
    *seq_bytes = *s ? _utf8_seqlen((unsigned char)*s) : 0;
    return s;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Core logic
 * ═══════════════════════════════════════════════════════════════════════════ */

/* ── _commit_char ──────────────────────────────────────────────────────── */
static void _commit_char(t9_kb_t *kb)
{
    if (kb->last_row < 0 || kb->last_col < 0)
        goto reset;

    {
        const char *chars = _cur_chars(kb)[kb->last_row][kb->last_col];
        if (!chars)
            goto reset;

        int cp_count = _utf8_len(chars);
        int cp_idx = (kb->tap_count - 1) % cp_count;
        int seq_len = 0;
        const char *cp = _utf8_at(chars, cp_idx, &seq_len);

        /* ASCII single-byte: apply shift for a-z */
        char shifted_buf[2];
        if (seq_len == 1 && kb->shift && cp[0] >= 'a' && cp[0] <= 'z') {
            shifted_buf[0] = (char)(cp[0] - 'a' + 'A');
            shifted_buf[1] = '\0';
            cp = shifted_buf;
            seq_len = 1;
        }

        if (kb->text_area) {
            /* lv_textarea_add_text expects a NUL-terminated UTF-8 string */
            char buf[8];
            if (seq_len > 0 && seq_len < (int)sizeof(buf)) {
                memcpy(buf, cp, (size_t)seq_len);
                buf[seq_len] = '\0';
                lv_textarea_add_text(kb->text_area, buf);
            }
        }
        if (kb->input_len + seq_len < T9_MAX_INPUT_LEN) {
            memcpy(kb->input + kb->input_len, cp, (size_t)seq_len);
            kb->input_len += seq_len;
            kb->input[kb->input_len] = '\0';
        }
    }

reset:
    kb->last_row = -1;
    kb->last_col = -1;
    kb->tap_count = 0;
    if (kb->tap_timer) {
        lv_timer_delete(kb->tap_timer);
        kb->tap_timer = NULL;
    }
}

/* ── _tap_timer_cb ─────────────────────────────────────────────────────── */
static void _tap_timer_cb(lv_timer_t *t)
{
    t9_kb_t *kb = (t9_kb_t *)t->user_data;
    kb->tap_timer = NULL; /* timer already fired; don't delete again */
    _commit_char(kb);
}

/* ── _enter_timer_cb ───────────────────────────────────────────────────── */
/* Fires when the enter key was tapped exactly once and the timeout elapsed
 * without a second tap → emit the CR "ready" signal.                       */
static void _enter_timer_cb(lv_timer_t *t)
{
    t9_kb_t *kb = (t9_kb_t *)t->user_data;
    kb->enter_timer = NULL;
    int taps = kb->enter_taps;
    kb->enter_taps = 0;
    _do_enter(kb, taps);
}

/* ── _update_labels ────────────────────────────────────────────────────── */
static void _update_labels(t9_kb_t *kb)
{
    const char *const(*map)[T9_KEY_COLS] = _cur_map(kb);

    for (int r = 0; r < T9_KEY_ROWS; r++) {
        for (int c = 0; c < T9_KEY_COLS; c++) {
            lv_obj_t *lbl = lv_obj_get_child(kb->keys[r][c], 0);
            if (!lbl)
                continue;

            /*
             * Row-3 functional keys: always fixed symbols regardless of mode.
             * col 0 = backspace, col 2 = enter — the map already contains the
             * correct LV_SYMBOL_* string, so we just fall through to the
             * generic lv_label_set_text(lbl, map[r][c]) below.
             *
             * For alpha mode + shift: rebuild a single-row uppercase label
             * because T9_ALPHA_MAP always shows lowercase.
             */
            if (kb->shift && kb->mode == T9_MODE_ALPHA && r < 3) {
                const char *chars = T9_ALPHA_CHARS[r][c];
                if (chars) {
                    /* Build "U U U D" — uppercase letters first, digit last */
                    char buf[32];
                    int len = 0;
                    int ncp = (int)strlen(chars); /* all ASCII here */
                    /* letters (all but the last char which is the digit) */
                    for (int i = 0; i < ncp - 1; i++) {
                        if (i > 0)
                            buf[len++] = ' ';
                        char ch = chars[i];
                        buf[len++] = (ch >= 'a' && ch <= 'z') ? (char)(ch - 'a' + 'A') : ch;
                    }
                    /* digit at the end */
                    buf[len++] = ' ';
                    buf[len++] = chars[ncp - 1];
                    buf[len] = '\0';
                    lv_label_set_text(lbl, buf);
                    continue;
                }
            }

            /* All other cases: use the map string directly */
            lv_label_set_text(lbl, map[r][c]);
        }
    }

    /* Mode button: show current mode name and next mode as hint */
    lv_obj_t *mode_lbl = lv_obj_get_child(kb->mode_btn, 0);
    if (mode_lbl) {
        const char *txt;
        switch (kb->mode) {
        case T9_MODE_SPECIAL:
            txt = "?!@"
                  "\n" LV_SYMBOL_RIGHT "\n"
                  "\xC3\xA4\xC3\xB6\xC3\xBC";
            break;
        case T9_MODE_LATIN:
            txt = "\xC3\xA4\xC3\xB6\xC3\xBC"
                  "\n" LV_SYMBOL_RIGHT "\n"
                  "ABC";
            break;
        default:
            txt = "ABC"
                  "\n" LV_SYMBOL_RIGHT "\n"
                  "?!@";
            break;
        }
        lv_label_set_text(mode_lbl, txt);
    }

    /* Shift button: green when active */
    lv_obj_set_style_bg_color(kb->shift_btn, kb->shift ? lv_color_hex(0x27AE60) : lv_color_hex(0x1A252F), 0);
}

/* ── _do_backspace ─────────────────────────────────────────────────────── */
static void _do_backspace(t9_kb_t *kb)
{
    /* Cancel any in-flight multi-tap without deleting a committed char */
    if (kb->last_row >= 0) {
        kb->last_row = -1;
        kb->last_col = -1;
        kb->tap_count = 0;
        if (kb->tap_timer) {
            lv_timer_delete(kb->tap_timer);
            kb->tap_timer = NULL;
        }
        return;
    }

    /* Remove the last UTF-8 codepoint from the input buffer */
    if (kb->input_len > 0) {
        /* Walk back to the start of the last codepoint */
        int i = kb->input_len - 1;
        while (i > 0 && ((unsigned char)kb->input[i] & 0xC0) == 0x80)
            i--; /* skip UTF-8 continuation bytes */
        kb->input_len = i;
        kb->input[kb->input_len] = '\0';
    }
    if (kb->text_area)
        lv_textarea_delete_char(kb->text_area);
}

/* ── _do_enter ─────────────────────────────────────────────────────────── */
/*
 * Enter key behaviour (T9 multi-tap on the ↵ key):
 *
 *   1 tap  →  fires LV_EVENT_READY on the textarea (same signal the built-in
 *             LVGL keyboard sends when its OK/Enter button is pressed).
 *             Catch it in your textarea event handler with:
 *               if (lv_event_get_code(e) == LV_EVENT_READY) { ... }
 *             Nothing is inserted into the text buffer.
 *
 *   2 taps →  inserts a real newline '\n' (LF, 0x0A) so the user can enter
 *             multi-line text.
 *
 * The tap count is resolved by the same T9_MULTI_TAP_TIMEOUT timer used for
 * all other keys, so a second tap within the timeout window upgrades to newline.
 */
static void _do_enter(t9_kb_t *kb, int tap_count)
{
    if (tap_count == 1) {
        /* Single tap → "ready / submit" signal, nothing inserted */
        if (kb->text_area)
            lv_obj_send_event(kb->text_area, LV_EVENT_READY, NULL);
    } else {
        /* Two (or more) taps → real newline (LF = 0x0A) */
        if (kb->text_area)
            lv_textarea_add_char(kb->text_area, '\n');
        if (kb->input_len < T9_MAX_INPUT_LEN - 1) {
            kb->input[kb->input_len++] = '\n';
            kb->input[kb->input_len] = '\0';
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Event callbacks
 * ═══════════════════════════════════════════════════════════════════════════ */

static void _key_cb(lv_event_t *e)
{
    t9_kb_t *kb = (t9_kb_t *)lv_event_get_user_data(e);
    lv_obj_t *btn = lv_event_get_target(e);
    uintptr_t rc = (uintptr_t)lv_obj_get_user_data(btn);
    int row = (int)(rc >> 4) & 0xF;
    int col = (int)(rc & 0xF);

    /* Functional row-3 keys */
    if (row == 3) {
        if (col == 0) {
            /* Backspace: always immediate; commits any pending char first */
            _commit_char(kb);
            _do_backspace(kb);
            return;
        }
        if (col == 2) {
            /*
             * Enter key — multi-tap:
             *   tap 1 (within timeout): arms timer, waits for possible tap 2
             *   tap 2 (before timeout): upgrades to newline
             *   timer fires after tap 1 alone: commits as CR (ready signal)
             *
             * We reuse the existing last_row/last_col/tap_count state but
             * mark col with a sentinel so _commit_char skips char emission
             * (chars[row][col] is NULL for this key).  The timer callback
             * calls _commit_char which hits the NULL guard and resets state,
             * so we fire _do_enter from here directly on tap 2, and from a
             * dedicated enter-commit path in the timer callback instead.
             *
             * Simpler approach used here: maintain a separate enter_taps
             * counter and always handle enter inline.
             */
            _commit_char(kb); /* flush any pending non-enter key first */

            if (kb->enter_taps == 0) {
                /* First tap: start timer */
                kb->enter_taps = 1;
                if (kb->enter_timer) {
                    lv_timer_delete(kb->enter_timer);
                }
                kb->enter_timer = lv_timer_create(_enter_timer_cb, T9_MULTI_TAP_TIMEOUT, kb);
                lv_timer_set_repeat_count(kb->enter_timer, 1);
            } else {
                /* Second tap within timeout: upgrade to newline */
                if (kb->enter_timer) {
                    lv_timer_delete(kb->enter_timer);
                    kb->enter_timer = NULL;
                }
                kb->enter_taps = 0;
                _do_enter(kb, 2);
            }
            return;
        }
        /* col 1 (space / +) falls through to multi-tap */
    }

    const char *chars = _cur_chars(kb)[row][col];
    if (!chars)
        return;

    /* Multi-tap: same key again → advance cycle */
    if (row == kb->last_row && col == kb->last_col) {
        kb->tap_count++;
        if (kb->tap_timer) {
            lv_timer_delete(kb->tap_timer);
            kb->tap_timer = NULL;
        }
    } else {
        _commit_char(kb); /* commit previous key first */
        kb->last_row = row;
        kb->last_col = col;
        kb->tap_count = 1;
    }

    /* Highlight active key */
    for (int r = 0; r < T9_KEY_ROWS; r++)
        for (int c = 0; c < T9_KEY_COLS; c++)
            lv_obj_clear_state(kb->keys[r][c], LV_STATE_CHECKED);
    lv_obj_add_state(kb->keys[row][col], LV_STATE_CHECKED);

    /* (Re-)arm auto-commit timer */
    kb->tap_timer = lv_timer_create(_tap_timer_cb, T9_MULTI_TAP_TIMEOUT, kb);
    lv_timer_set_repeat_count(kb->tap_timer, 1);
}

static void _mode_btn_cb(lv_event_t *e)
{
    t9_kb_t *kb = (t9_kb_t *)lv_event_get_user_data(e);
    _commit_char(kb);
    kb->mode = (t9_mode_t)((kb->mode + 1) % T9_MODE_COUNT);
    _update_labels(kb);
}

static void _shift_btn_cb(lv_event_t *e)
{
    t9_kb_t *kb = (t9_kb_t *)lv_event_get_user_data(e);
    _commit_char(kb);
    kb->shift = !kb->shift;
    _update_labels(kb);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Public API
 * ═══════════════════════════════════════════════════════════════════════════ */

t9_kb_t *t9_kb_create(lv_obj_t *parent, lv_obj_t *ta)
{
    _init_styles();

    t9_kb_t *kb = (t9_kb_t *)lv_malloc(sizeof(t9_kb_t));
    LV_ASSERT_MALLOC(kb);
    memset(kb, 0, sizeof(t9_kb_t));

    kb->text_area = ta;
    kb->last_row = -1;
    kb->last_col = -1;
    kb->mode = T9_MODE_ALPHA;
    kb->shift = false;

    /* ── Root container ─────────────────────────────────────────────────
     * 100 % wide × 73 % tall of the parent.  On 480×222 → 480×162 px.
     * lv_obj_update_layout() is called immediately so we can read back the
     * resolved pixel size and compute cell dimensions from it.
     * ─────────────────────────────────────────────────────────────────── */
    kb->container = lv_obj_create(parent);
    lv_obj_set_size(kb->container, lv_pct(100), lv_pct(100));
    lv_obj_align(kb->container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(kb->container, lv_color_hex(0x17202A), 0);
    lv_obj_set_style_bg_opa(kb->container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(kb->container, 0, 0);
    lv_obj_set_style_pad_all(kb->container, 0, 0);
    lv_obj_set_style_pad_row(kb->container, 0, 0);
    lv_obj_set_style_pad_column(kb->container, 0, 0);
    lv_obj_clear_flag(kb->container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_update_layout(kb->container);

    /* ── Cell geometry ──────────────────────────────────────────────────
     *
     * gap  = 1 % of the shorter dimension, clamped to [2, 6] px
     * grid = 75 % of inner width  (3 cols)
     * side = 25 % of inner width  (mode + shift panel)
     * ─────────────────────────────────────────────────────────────────── */
    const int32_t W = lv_obj_get_width(kb->container);
    const int32_t H = lv_obj_get_height(kb->container);
    const int32_t gap = LV_CLAMP(2, LV_MIN(W, H) / 100, 6);
    const int32_t inner_w = W - 2 * gap;
    const int32_t inner_h = H - 2 * gap;
    const int32_t cell_w = (inner_w * 75 / 100 - (T9_KEY_COLS + 1) * gap) / T9_KEY_COLS;
    const int32_t side_w = inner_w - T9_KEY_COLS * cell_w - (T9_KEY_COLS + 2) * gap;
    const int32_t cell_h = (inner_h - (T9_KEY_ROWS + 1) * gap) / T9_KEY_ROWS;
    const int32_t grid_x = gap;
    const int32_t grid_y = gap;
    const int32_t side_x = grid_x + T9_KEY_COLS * (cell_w + gap) + gap;

    /* ── T9 key grid ────────────────────────────────────────────────── */
    for (int r = 0; r < T9_KEY_ROWS; r++) {
        for (int c = 0; c < T9_KEY_COLS; c++) {
            lv_obj_t *btn = lv_button_create(kb->container);
            lv_obj_set_size(btn, cell_w, cell_h);
            lv_obj_set_pos(btn, grid_x + c * (cell_w + gap), grid_y + r * (cell_h + gap));

            bool is_fn = (r == 3 && (c == 0 || c == 2));
            lv_obj_add_style(btn, is_fn ? &s_style_fn_key : &s_style_key, 0);
            lv_obj_add_style(btn, &s_style_key_pressed, LV_STATE_PRESSED);
            lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

            lv_obj_t *lbl = lv_label_create(btn);
            lv_obj_center(lbl);
            lv_label_set_long_mode(lbl, LV_LABEL_LONG_CLIP);
            lv_obj_set_size(lbl, cell_w - 4, cell_h - 4);
            lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);

            lv_obj_set_user_data(btn, (void *)(uintptr_t)((r << 4) | c));
            lv_obj_add_event_cb(btn, _key_cb, LV_EVENT_CLICKED, kb);
            kb->keys[r][c] = btn;
        }
    }

    /* ── Side panel button geometry ─────────────────────────────────── */
    const int32_t side_btn_h = cell_h * 2 + gap;

    /* Mode button (top half of side panel) */
    kb->mode_btn = lv_button_create(kb->container);
    lv_obj_set_size(kb->mode_btn, side_w, side_btn_h);
    lv_obj_set_pos(kb->mode_btn, side_x, grid_y);
    lv_obj_add_style(kb->mode_btn, &s_style_mode_btn, 0);
    lv_obj_add_style(kb->mode_btn, &s_style_key_pressed, LV_STATE_PRESSED);
    lv_obj_clear_flag(kb->mode_btn, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *lbl = lv_label_create(kb->mode_btn);
        lv_label_set_text(lbl, "ABC\n" LV_SYMBOL_RIGHT "\n?!@");
        lv_obj_center(lbl);
        lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
    }
    lv_obj_add_event_cb(kb->mode_btn, _mode_btn_cb, LV_EVENT_CLICKED, kb);

    /* Shift button (bottom half of side panel) */
    kb->shift_btn = lv_button_create(kb->container);
    lv_obj_set_size(kb->shift_btn, side_w, side_btn_h);
    lv_obj_set_pos(kb->shift_btn, side_x, grid_y + 2 * (cell_h + gap));
    lv_obj_add_style(kb->shift_btn, &s_style_fn_key, 0);
    lv_obj_add_style(kb->shift_btn, &s_style_key_pressed, LV_STATE_PRESSED);
    lv_obj_clear_flag(kb->shift_btn, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *lbl = lv_label_create(kb->shift_btn);
        lv_label_set_text(lbl, LV_SYMBOL_UP "\nShift");
        lv_obj_center(lbl);
        lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
    }
    lv_obj_add_event_cb(kb->shift_btn, _shift_btn_cb, LV_EVENT_CLICKED, kb);

    _update_labels(kb);
    return kb;
}

void t9_kb_del(t9_kb_t *kb)
{
    if (!kb)
        return;
    if (kb->tap_timer) {
        lv_timer_delete(kb->tap_timer);
        kb->tap_timer = NULL;
    }
    if (kb->enter_timer) {
        lv_timer_delete(kb->enter_timer);
        kb->enter_timer = NULL;
    }
    lv_obj_delete(kb->container);
    lv_free(kb);
}

const char *t9_kb_get_input(const t9_kb_t *kb)
{
    return kb ? kb->input : "";
}

void t9_kb_clear(t9_kb_t *kb)
{
    if (!kb)
        return;
    _commit_char(kb);
    kb->input_len = 0;
    kb->input[0] = '\0';
    if (kb->text_area)
        lv_textarea_set_text(kb->text_area, "");
}
