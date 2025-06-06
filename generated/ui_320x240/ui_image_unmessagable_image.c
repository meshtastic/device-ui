#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_UNMESSAGABLE_IMAGE
#define LV_ATTRIBUTE_IMG_UNMESSAGABLE_IMAGE
#endif

static const
LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_UNMESSAGABLE_IMAGE
uint8_t img_unmessagable_image_map[] = {

    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x79,0xce,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x9a,0xd6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xfb,0xde,0xdb,0xde,0xfb,0xde,0xfb,0xde,0xdb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xdb,0xde,0xfb,0xde,0xff,0xff,0xdb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0xff,0xff,0x9a,0xd6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0x18,0xc6,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xdf,0xff,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xfb,0xde,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0xfb,0xde,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xba,0xd6,0x3c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0xfb,0xde,0xfb,0xde,0x59,0xce,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9a,0xd6,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfb,0xde,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x3c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x00,0x00,0xfb,0xde,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x00,0x00,0x00,0x00,0xfb,0xde,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xff,0xff,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x00,0x00,0x9a,0xd6,0x38,0xc6,0xba,0xd6,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0x7d,0xef,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x9a,0xd6,0x3c,0xe7,0x1c,0xe7,0xff,0xff,0x9a,0xd6,0xb6,0xb5,0x38,0xc6,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0x00,0x00,0xdb,0xde,0xdb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x9a,0xd6,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0xba,0xd6,0x7d,0xef,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x96,0xb5,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0xba,0xd6,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xdb,0xde,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xdb,0xde,0xfb,0xde,0xff,0xff,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xba,0xd6,0xfb,0xde,0xfb,0xde,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9a,0xd6,0xff,0xff,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0xdb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xdb,0xde,0xff,0xff,0xfb,0xde,0xdb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x7d,0xef,0xfb,0xde,0xdb,0xde,0xfb,0xde,0xfb,0xde,0xdb,0xde,0xfb,0xde,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9a,0xd6,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0x1c,0xe7,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x79,0xce,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0xfb,0xde,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x04,0x3d,0x3e,0x03,0x00,0x00,0x00,0x08,0x30,0x53,0x5b,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5b,0x53,0x30,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0xec,0xf0,0x6c,0x02,0x00,0x0b,0x94,0xee,0xfc,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfc,0xec,0xa7,0x2c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3e,0xf0,0xff,0xf3,0x6c,0x01,0x01,0x5d,0xed,0xff,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xff,0xff,0xff,0xd1,0x2c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x6c,0xf3,0xff,0xf3,0x6c,0x01,0x00,0x4d,0x6c,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x6b,0xbf,0xfe,0xff,0xa7,0x07,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x02,0x6c,0xf3,0xff,0xf3,0x6c,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0xbf,0xff,0xec,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x6c,0xf2,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6a,0xff,0xfc,0x53,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x01,0x83,0xff,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x59,0xfe,0xff,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xff,0xff,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfc,0xbc,0xef,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5d,0x67,0xf3,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5a,0x00,0x6c,0xf3,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x02,0x6c,0xf3,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x02,0x6c,0xf3,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x02,0x6c,0xf3,0xff,0xf3,0x6c,0x02,0x00,0x00,0x5c,0xff,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x02,0x6c,0xf3,0xff,0xf3,0x6c,0x02,0x00,0x3c,0xea,0xff,0x5b,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x02,0x6c,0xf3,0xff,0xf3,0x6c,0x01,0x00,0x5a,0xea,0x5d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x6c,0xf3,0xff,0xf3,0x6c,0x01,0x00,0x59,0x47,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x6a,0xf3,0xff,0xf3,0x6c,0x01,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x30,0x98,0xa5,0xa5,0x97,0x31,0x6a,0xf3,0xff,0xf3,0x6c,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0xfe,0xfe,0x5b,0x00,0x00,0x00,0x96,0xff,0xff,0xff,0xff,0x96,0x01,0x6c,0xf3,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5b,0xfe,0xfe,0x5a,0x00,0x00,0x00,0x59,0xd8,0xe1,0xe1,0xd8,0x59,0x00,0x02,0x6c,0xf1,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x53,0xfc,0xff,0x6b,0x00,0x00,0x00,0x02,0x18,0x1f,0x1f,0x18,0x02,0x00,0x00,0x00,0x91,0xff,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0xec,0xff,0xbf,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0xbd,0xff,0xff,0xff,0xf3,0x6c,0x02,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xa7,0xff,0xfe,0xbf,0x6a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x5a,0x6a,0xbe,0xfe,0xff,0xd8,0xf0,0xff,0xf3,0x6c,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2c,0xd1,0xff,0xff,0xff,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xff,0xff,0xff,0xd0,0x31,0x6a,0xf3,0xff,0xf0,0x3e,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2c,0xa7,0xec,0xfc,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfc,0xec,0xa7,0x2c,0x00,0x02,0x6c,0xf0,0xec,0x3e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x30,0x53,0x5b,0x5b,0x5b,0x5b,0x5b,0x5b,0x5b,0x5b,0x53,0x30,0x07,0x00,0x00,0x00,0x03,0x3e,0x3e,0x04,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

};

const lv_image_dsc_t img_unmessagable_image = {
  .header.magic = LV_IMAGE_HEADER_MAGIC,
  .header.cf = LV_COLOR_FORMAT_RGB565A8,
  .header.flags = 0,
  .header.w = 32,
  .header.h = 32,
  .header.stride = 64,
  .data_size = sizeof(img_unmessagable_image_map),
  .data = img_unmessagable_image_map,
};

