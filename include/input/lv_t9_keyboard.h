/**
 * @file  lv_t9_keyboard.h
 * @brief T9-style virtual keyboard for LVGL on ESP32 (480×222 TFT)
 *        — interface / header —
 *
 * Include this file in every translation unit that uses the keyboard.
 * Compile lv_t9_keyboard.c exactly once and link it into your project.
 *
 * Compatibility: LVGL >= 9.0  (tested against 9.3)
 *
 * Keyboard modes (cycled by the mode button):
 *   T9_MODE_ALPHA   — Latin a-z / digits / basic punctuation  (multi-tap)
 *   T9_MODE_SPECIAL — ASCII special characters  (!@#$ …)
 *   T9_MODE_LATIN   — European supplemental chars  (Ä Ö Ü á é … multi-tap)
 *
 * Key label format: all characters for a key on one line, space-separated,
 * e.g. "2 a b c" — readable even on small keys.
 */

#ifndef LV_T9_KEYBOARD_H
#define LV_T9_KEYBOARD_H

#include "lvgl.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── Configuration ─────────────────────────────────────────────────────── */
#define T9_MAX_INPUT_LEN 256     /* max bytes in the input buffer         */
#define T9_MULTI_TAP_TIMEOUT 800 /* ms idle before multi-tap char commits */
#define T9_KEY_ROWS 4
#define T9_KEY_COLS 3

/* ── Keyboard mode ─────────────────────────────────────────────────────── */
typedef enum {
    T9_MODE_ALPHA = 0,   /* a-z, digits, basic punctuation  */
    T9_MODE_SPECIAL = 1, /* ASCII special chars             */
    T9_MODE_LATIN = 2,   /* European supplemental chars     */
    T9_MODE_COUNT = 3
} t9_mode_t;

/* ── State struct ──────────────────────────────────────────────────────── */
typedef struct {
    lv_obj_t *container;                      /* Root container widget   */
    lv_obj_t *text_area;                      /* Bound textarea (or NULL)*/
    lv_obj_t *keys[T9_KEY_ROWS][T9_KEY_COLS]; /* Key buttons             */
    lv_obj_t *mode_btn;                       /* Mode cycle button       */
    lv_obj_t *shift_btn;                      /* Uppercase toggle        */
    lv_timer_t *tap_timer;                    /* Multi-tap commit timer  */
    lv_timer_t *enter_timer;                  /* Enter multi-tap timer   */
    int enter_taps;                           /* Enter tap count (0/1)   */

    t9_mode_t mode;
    int last_row;  /* Row of last tapped key  */
    int last_col;  /* Col of last tapped key  */
    int tap_count; /* Taps on current key     */
    bool shift;    /* Uppercase active        */

    char input[T9_MAX_INPUT_LEN];
    int input_len;
} t9_kb_t;

/* ── Public API ────────────────────────────────────────────────────────── */

/**
 * @brief  Create a T9 keyboard and attach it to a parent widget.
 *
 * @param  parent  LVGL parent object (e.g. lv_screen_active()).
 * @param  ta      Optional lv_textarea to type into; pass NULL to poll
 *                 t9_kb_get_input() yourself.
 * @return Heap-allocated keyboard state.  Destroy with t9_kb_del().
 *
 * The keyboard occupies the bottom 73 % of the parent (100 % wide).
 * Cell sizes are derived automatically from the resolved pixel dimensions.
 */
t9_kb_t *t9_kb_create(lv_obj_t *parent, lv_obj_t *ta);

/**
 * @brief  Destroy the keyboard and free all resources.
 */
void t9_kb_del(t9_kb_t *kb);

/**
 * @brief  Return the current accumulated input string (NUL-terminated UTF-8).
 */
const char *t9_kb_get_input(const t9_kb_t *kb);

/**
 * @brief  Clear the input buffer and (if bound) the attached textarea.
 *         Any pending multi-tap character is committed first.
 */
void t9_kb_clear(t9_kb_t *kb);

#ifdef __cplusplus
}
#endif
#endif /* LV_T9_KEYBOARD_H */
