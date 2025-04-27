#include <libdeflate.h>
#include "util/ILog.h"
#include <string>
#include <stdint.h>

static libdeflate_decompressor* decompressor = libdeflate_alloc_decompressor();


// Function to decompress GZIP data
std::string decompressGzip(const std::string& compressedData) {
    // last 4 bytes has uncompressed size
    size_t decompressedSize = *(uint32_t *)&compressedData[compressedData.size()-4];
    std::string decompressedData(decompressedSize, '\0');

    // Perform GZIP decompression
    size_t actualDecompressedSize = 0;
    libdeflate_result result = libdeflate_gzip_decompress(
        decompressor,
        compressedData.data(), compressedData.size(),
        (void*)decompressedData.data(), decompressedSize,
        &actualDecompressedSize
    );

    if (result != LIBDEFLATE_SUCCESS) {
        ILOG_ERROR("Decompression failed: %d", result);
        return {};
    }

    return decompressedData;
}