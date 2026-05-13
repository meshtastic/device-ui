#ifndef EEZ_LVGL_UI_FONTS_H
#define EEZ_LVGL_UI_FONTS_H

#include "lvgl.h"

extern const lv_font_t ui_font_montserrat_12;
extern const lv_font_t ui_font_montserrat_14;
extern const lv_font_t ui_font_montserrat_16;
extern const lv_font_t ui_font_montserrat_20;

#ifndef EXT_FONT_DESC_T
#define EXT_FONT_DESC_T
typedef struct _ext_font_desc_t {
    const char *name;
    const void *font_ptr;
} ext_font_desc_t;
#endif

extern ext_font_desc_t fonts[];

#endif /*EEZ_LVGL_UI_FONTS_H*/