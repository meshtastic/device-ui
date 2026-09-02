#include "util/Gunzip.h"
#include "lvgl.h"
#include "util/ILog.h"
#include <libdeflate.h>
#include <string.h>

// gzip trailer: CRC32 (4 bytes) + ISIZE (4 bytes, uncompressed size mod 2^32)
static constexpr size_t GZIP_MIN_LENGTH = 18;

static libdeflate_decompressor *decompressor()
{
    static libdeflate_decompressor *d = libdeflate_alloc_decompressor();
    return d;
}

uint8_t *decompressGzip(const uint8_t *compressedData, size_t length, size_t &decompressedSize)
{
    decompressedSize = 0;
    if (!compressedData || length < GZIP_MIN_LENGTH) {
        ILOG_ERROR("gunzip: input too short (%u bytes)", (unsigned int)length);
        return nullptr;
    }

    libdeflate_decompressor *d = decompressor();
    if (!d) {
        ILOG_ERROR("gunzip: failed to allocate decompressor");
        return nullptr;
    }

    uint32_t isize = 0;
    memcpy(&isize, &compressedData[length - 4], sizeof(isize)); // trailer is not guaranteed to be aligned

    uint8_t *decompressedData = (uint8_t *)lv_malloc(isize);
    if (!decompressedData) {
        ILOG_ERROR("gunzip: failed to allocate %u bytes", (unsigned int)isize);
        return nullptr;
    }

    size_t actualSize = 0;
    libdeflate_result result = libdeflate_gzip_decompress(d, compressedData, length, decompressedData, isize, &actualSize);
    if (result != LIBDEFLATE_SUCCESS) {
        ILOG_ERROR("gunzip: decompression failed: %d", result);
        lv_free(decompressedData);
        return nullptr;
    }

    decompressedSize = actualSize;
    return decompressedData;
}
