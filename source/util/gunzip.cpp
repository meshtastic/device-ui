#include "lvgl.h"
#include "util/ILog.h"
#include <libdeflate.h>
#include <stdint.h>
#include <string>

static libdeflate_decompressor *decompressor = libdeflate_alloc_decompressor();

// Function to decompress GZIP data
std::string decompressGzip(const uint8_t *compressedData, uint32_t length)
{
    // last 4 bytes has uncompressed size
    size_t decompressedSize = *(uint32_t *)&compressedData[length - 4];
    std::string decompressedData(decompressedSize, '\0');

    // Perform GZIP decompression
    size_t actualDecompressedSize = 0;
    libdeflate_result result = libdeflate_gzip_decompress(decompressor, compressedData, length, (void *)decompressedData.data(),
                                                          decompressedSize, &actualDecompressedSize);

    if (result != LIBDEFLATE_SUCCESS) {
        ILOG_ERROR("Decompression failed: %d", result);
        return {};
    }

    return decompressedData;
}

// Function to decompress GZIP data (using PSRAM)
uint8_t *decompressGzip(const uint8_t *compressedData, size_t length, size_t &actualDecompressedSize)
{
    // last 4 bytes has uncompressed size
    size_t decompressedSize = *(uint32_t *)&compressedData[length - 4];
    uint8_t *decompressedData = (uint8_t *)lv_malloc(decompressedSize);
    if (!decompressedData) {
        ILOG_ERROR("Failed to allocate memory for decompressed data");
        return nullptr;
    }

    // Perform GZIP decompression
    libdeflate_result result = libdeflate_gzip_decompress(decompressor, compressedData, length, decompressedData,
                                                          decompressedSize, &actualDecompressedSize);

    if (result != LIBDEFLATE_SUCCESS) {
        ILOG_ERROR("Decompression failed: %d", result);
        lv_free(decompressedData);
        decompressedData = nullptr;
    }

    return decompressedData;
}