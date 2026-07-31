#pragma once

extern void initPNGDecoder(void);
extern bool decodeImgGrey(const void *data, size_t size, lv_img_dsc_t **img);
extern bool decodeImgColor(const void *data, size_t size, lv_img_dsc_t **img);
