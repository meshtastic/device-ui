#include "lvgl.h"

#define LV_COLOR_HEX(C)                                                                                                          \
    {                                                                                                                            \
        .blue = (C >> 0) & 0xff, .green = (C >> 8) & 0xff, .red = (C >> 16) & 0xff                                               \
    }

constexpr lv_color_t colorRed = LV_COLOR_HEX(0xff5555);
constexpr lv_color_t colorDarkRed = LV_COLOR_HEX(0xa70a0a);
constexpr lv_color_t colorOrange = LV_COLOR_HEX(0xff8c04);
constexpr lv_color_t colorYellow = LV_COLOR_HEX(0xdbd251);
constexpr lv_color_t colorBlueGreen = LV_COLOR_HEX(0x05f6cb);
constexpr lv_color_t colorBlue = LV_COLOR_HEX(0x436C70);
constexpr lv_color_t colorGray = LV_COLOR_HEX(0x757575);
constexpr lv_color_t colorLightGray = LV_COLOR_HEX(0xAAFBFF);
constexpr lv_color_t colorMidGray = LV_COLOR_HEX(0x808080);
constexpr lv_color_t colorDarkGray = LV_COLOR_HEX(0x303030);
constexpr lv_color_t colorMesh = LV_COLOR_HEX(0x67ea94);
