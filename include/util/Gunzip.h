#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * Inflate a gzip member into a freshly lv_malloc'ed buffer (PSRAM).
 * Caller owns the result and frees it with lv_free().
 */
extern uint8_t *decompressGzip(const uint8_t *compressedData, size_t length, size_t &decompressedSize);
