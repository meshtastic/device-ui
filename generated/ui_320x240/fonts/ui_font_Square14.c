/*******************************************************************************
 * Size: 14 px
 * Bpp: 1
 * Opts: --bpp 1 --size 14 --font /home/manuel/SquareLine/Projects/T_Deck_Meshtastic/assets/square_pixel-7.ttf -o /home/manuel/SquareLine/Projects/T_Deck_Meshtastic/assets/ui_font_Square14.c --format lvgl -r 0x20-0x7f --no-compress --no-prefilter
 ******************************************************************************/

#include "../ui.h"

#ifndef UI_FONT_SQUARE14
#define UI_FONT_SQUARE14 1
#endif

#if UI_FONT_SQUARE14

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xfa,

    /* U+0022 "\"" */
    0x99, 0x90,

    /* U+0023 "#" */
    0x24, 0x4b, 0xf9, 0x2f, 0xe4, 0x89, 0x0,

    /* U+0024 "$" */
    0x11, 0xfe, 0x44, 0x8f, 0xe2, 0x44, 0xff,

    /* U+0025 "%" */
    0xc3, 0x98, 0x70, 0x87, 0xc, 0xe1, 0x80,

    /* U+0026 "&" */
    0xfa, 0x28, 0xbf, 0x8a, 0x2f, 0xc0,

    /* U+0027 "'" */
    0xe0,

    /* U+0028 "(" */
    0x7a, 0x49, 0x98,

    /* U+0029 ")" */
    0xcc, 0x92, 0xf0,

    /* U+002A "*" */
    0xb5, 0xe4, 0xb3, 0x7a, 0xd0,

    /* U+002B "+" */
    0x2f, 0x22,

    /* U+002C "," */
    0xc0,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x2, 0x18, 0x70, 0x87, 0xc, 0x20, 0x0,

    /* U+0030 "0" */
    0xff, 0x1e, 0x7d, 0xdf, 0x3c, 0x7f, 0x80,

    /* U+0031 "1" */
    0xe2, 0x22, 0x22, 0xf0,

    /* U+0032 "2" */
    0xfe, 0x4, 0xf, 0xf8, 0x10, 0x3f, 0x80,

    /* U+0033 "3" */
    0xfe, 0x4, 0xb, 0xf0, 0x20, 0x7f, 0x80,

    /* U+0034 "4" */
    0x83, 0x6, 0xf, 0xf0, 0x20, 0x40, 0x80,

    /* U+0035 "5" */
    0xff, 0x2, 0x7, 0xf0, 0x20, 0x7f, 0x80,

    /* U+0036 "6" */
    0xff, 0x2, 0x7, 0xf8, 0x30, 0x7f, 0x80,

    /* U+0037 "7" */
    0xfe, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,

    /* U+0038 "8" */
    0xff, 0x6, 0xf, 0xf8, 0x30, 0x7f, 0x80,

    /* U+0039 "9" */
    0xff, 0x6, 0xf, 0xf0, 0x20, 0x7f, 0x80,

    /* U+003A ":" */
    0x90,

    /* U+003B ";" */
    0x8c,

    /* U+003C "<" */
    0x12, 0xe6, 0x10,

    /* U+003D "=" */
    0xf0, 0xf,

    /* U+003E ">" */
    0x84, 0x76, 0x80,

    /* U+003F "?" */
    0xfc, 0x10, 0x4f, 0x20, 0x2, 0x0,

    /* U+0040 "@" */
    0xff, 0x6, 0xc, 0xf8, 0x10, 0x3f, 0x80,

    /* U+0041 "A" */
    0xff, 0x6, 0xf, 0xf8, 0x30, 0x60, 0x80,

    /* U+0042 "B" */
    0xff, 0x6, 0xf, 0xf8, 0x30, 0x7f, 0x80,

    /* U+0043 "C" */
    0xff, 0x2, 0x4, 0x8, 0x10, 0x3f, 0x80,

    /* U+0044 "D" */
    0xff, 0x6, 0xc, 0x18, 0x30, 0x7f, 0x80,

    /* U+0045 "E" */
    0xff, 0x2, 0x7, 0xe8, 0x10, 0x3f, 0x80,

    /* U+0046 "F" */
    0xff, 0x2, 0x7, 0xe8, 0x10, 0x20, 0x0,

    /* U+0047 "G" */
    0xff, 0x2, 0x4, 0xf8, 0x30, 0x7f, 0x80,

    /* U+0048 "H" */
    0x83, 0x6, 0xf, 0xf8, 0x30, 0x60, 0x80,

    /* U+0049 "I" */
    0xf2, 0x22, 0x22, 0xf0,

    /* U+004A "J" */
    0xfe, 0x4, 0x8, 0x18, 0x30, 0x7f, 0x80,

    /* U+004B "K" */
    0x87, 0x1a, 0x77, 0xc9, 0xd0, 0xa1, 0x80,

    /* U+004C "L" */
    0x81, 0x2, 0x4, 0x8, 0x10, 0x3f, 0x80,

    /* U+004D "M" */
    0x83, 0xdf, 0xfc, 0x99, 0x30, 0x60, 0x80,

    /* U+004E "N" */
    0x83, 0xc7, 0xcc, 0x99, 0xf1, 0xe0, 0x80,

    /* U+004F "O" */
    0xff, 0x6, 0xc, 0x18, 0x30, 0x7f, 0x80,

    /* U+0050 "P" */
    0xff, 0x6, 0xf, 0xf8, 0x10, 0x20, 0x0,

    /* U+0051 "Q" */
    0xff, 0x6, 0xc, 0x19, 0xf1, 0xfe, 0x80,

    /* U+0052 "R" */
    0xff, 0x6, 0xf, 0xf9, 0xd1, 0xe1, 0x80,

    /* U+0053 "S" */
    0xff, 0x2, 0x7, 0xf0, 0x20, 0x7f, 0x80,

    /* U+0054 "T" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x4, 0x0,

    /* U+0055 "U" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x7f, 0x80,

    /* U+0056 "V" */
    0x83, 0x6, 0xc, 0x1e, 0xef, 0x84, 0x0,

    /* U+0057 "W" */
    0x93, 0x26, 0x4c, 0x99, 0x32, 0x7f, 0x80,

    /* U+0058 "X" */
    0x82, 0xd9, 0xf1, 0xc7, 0xc8, 0xa0, 0x80,

    /* U+0059 "Y" */
    0xc6, 0xd9, 0xf0, 0x81, 0x2, 0x4, 0x0,

    /* U+005A "Z" */
    0xfe, 0x18, 0x71, 0xc7, 0xc, 0x3f, 0x80,

    /* U+005B "[" */
    0xea, 0xac,

    /* U+005C "\\" */
    0x80, 0xc1, 0xc0, 0x81, 0xc1, 0x80, 0x80,

    /* U+005D "]" */
    0xd5, 0x5c,

    /* U+005E "^" */
    0x71, 0xe8, 0xc0,

    /* U+005F "_" */
    0xfe,

    /* U+0060 "`" */
    0xc9, 0x80,

    /* U+0061 "a" */
    0xff, 0x6, 0xf, 0xf8, 0x30, 0x60, 0x80,

    /* U+0062 "b" */
    0xff, 0x6, 0xf, 0xf8, 0x30, 0x7f, 0x80,

    /* U+0063 "c" */
    0xff, 0x2, 0x4, 0x8, 0x10, 0x3f, 0x80,

    /* U+0064 "d" */
    0xff, 0x6, 0xc, 0x18, 0x30, 0x7f, 0x80,

    /* U+0065 "e" */
    0xff, 0x2, 0x7, 0xe8, 0x10, 0x3f, 0x80,

    /* U+0066 "f" */
    0xff, 0x2, 0x7, 0xe8, 0x10, 0x20, 0x0,

    /* U+0067 "g" */
    0xff, 0x2, 0x4, 0xf8, 0x30, 0x7f, 0x80,

    /* U+0068 "h" */
    0x83, 0x6, 0xf, 0xf8, 0x30, 0x60, 0x80,

    /* U+0069 "i" */
    0xf2, 0x22, 0x22, 0xf0,

    /* U+006A "j" */
    0xfe, 0x4, 0x8, 0x18, 0x30, 0x7f, 0x80,

    /* U+006B "k" */
    0x87, 0x1a, 0x77, 0xc9, 0xd0, 0xa1, 0x80,

    /* U+006C "l" */
    0x81, 0x2, 0x4, 0x8, 0x10, 0x3f, 0x80,

    /* U+006D "m" */
    0x83, 0xdf, 0xfc, 0x99, 0x30, 0x60, 0x80,

    /* U+006E "n" */
    0x83, 0xc7, 0xcc, 0x99, 0xf1, 0xe0, 0x80,

    /* U+006F "o" */
    0xff, 0x6, 0xc, 0x18, 0x30, 0x7f, 0x80,

    /* U+0070 "p" */
    0xff, 0x6, 0xf, 0xf8, 0x10, 0x20, 0x0,

    /* U+0071 "q" */
    0xff, 0x6, 0xc, 0x19, 0xf1, 0xfe, 0x80,

    /* U+0072 "r" */
    0xff, 0x6, 0xf, 0xf9, 0xd1, 0xe1, 0x80,

    /* U+0073 "s" */
    0xff, 0x2, 0x7, 0xf0, 0x20, 0x7f, 0x80,

    /* U+0074 "t" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x4, 0x0,

    /* U+0075 "u" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x7f, 0x80,

    /* U+0076 "v" */
    0x83, 0x6, 0xc, 0x1e, 0xef, 0x84, 0x0,

    /* U+0077 "w" */
    0x93, 0x26, 0x4c, 0x99, 0x32, 0x7f, 0x80,

    /* U+0078 "x" */
    0x82, 0xd9, 0xf1, 0xc7, 0xc8, 0xa0, 0x80,

    /* U+0079 "y" */
    0xc6, 0xd9, 0xf0, 0x81, 0x2, 0x4, 0x0,

    /* U+007A "z" */
    0xfe, 0x18, 0x71, 0xc7, 0xc, 0x3f, 0x80,

    /* U+007B "{" */
    0x69, 0x64, 0x98,

    /* U+007C "|" */
    0xfe,

    /* U+007D "}" */
    0xe2, 0x23, 0x22, 0xe0,

    /* U+007E "~" */
    0xf3, 0x26, 0x78
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 112, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 123, .box_w = 1, .box_h = 7, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 123, .box_w = 4, .box_h = 3, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 4, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 11, .adv_w = 123, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 18, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 25, .adv_w = 123, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 31, .adv_w = 123, .box_w = 1, .box_h = 3, .ofs_x = 3, .ofs_y = 4},
    {.bitmap_index = 32, .adv_w = 123, .box_w = 3, .box_h = 7, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 35, .adv_w = 123, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 38, .adv_w = 123, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 43, .adv_w = 123, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 45, .adv_w = 123, .box_w = 1, .box_h = 2, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 46, .adv_w = 123, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 47, .adv_w = 123, .box_w = 1, .box_h = 1, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 48, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 55, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 62, .adv_w = 123, .box_w = 4, .box_h = 7, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 66, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 73, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 80, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 87, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 94, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 101, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 108, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 115, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 122, .adv_w = 123, .box_w = 1, .box_h = 4, .ofs_x = 3, .ofs_y = 2},
    {.bitmap_index = 123, .adv_w = 123, .box_w = 1, .box_h = 6, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 124, .adv_w = 123, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 127, .adv_w = 123, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 129, .adv_w = 123, .box_w = 4, .box_h = 5, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 132, .adv_w = 123, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 138, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 145, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 152, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 166, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 173, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 180, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 187, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 194, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 201, .adv_w = 123, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 205, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 212, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 219, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 226, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 233, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 240, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 247, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 254, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 261, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 268, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 275, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 282, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 289, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 296, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 303, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 310, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 317, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 324, .adv_w = 123, .box_w = 2, .box_h = 7, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 326, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 333, .adv_w = 123, .box_w = 2, .box_h = 7, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 335, .adv_w = 123, .box_w = 6, .box_h = 3, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 338, .adv_w = 123, .box_w = 7, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 339, .adv_w = 123, .box_w = 3, .box_h = 3, .ofs_x = 2, .ofs_y = 4},
    {.bitmap_index = 341, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 348, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 355, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 362, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 369, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 376, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 383, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 390, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 397, .adv_w = 123, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 401, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 408, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 415, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 422, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 429, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 436, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 443, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 450, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 457, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 464, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 471, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 478, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 485, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 492, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 499, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 506, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 513, .adv_w = 123, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 520, .adv_w = 123, .box_w = 3, .box_h = 7, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 523, .adv_w = 123, .box_w = 1, .box_h = 7, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 524, .adv_w = 123, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 528, .adv_w = 123, .box_w = 7, .box_h = 3, .ofs_x = 0, .ofs_y = 4}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t ui_font_Square14 = {
#else
lv_font_t ui_font_Square14 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 9,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if UI_FONT_SQUARE14*/

