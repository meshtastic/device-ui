#include "styles.h"
#include "images.h"
#include "fonts.h"

void apply_style_montserrat16(lv_obj_t *obj) {
    lv_obj_set_style_text_color(obj, lv_color_hex(0xffaafbff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
};

