#include "core/lv_global.h"
#include "util/ILog.h"
#include <PNGdec.h>
#include <new> // Required for placement new syntax

// Context structures passed to the row decoder callbacks
struct PNGDrawContext {
    PNG *pngInstance;
    uint16_t *destBuffer;
    int width;
};

struct PNGGreyDrawContext {
    PNG *pngInstance;
    uint8_t *destBuffer;
    int width;
};

PNG *png = nullptr;

#ifdef PNGDEC_SRAM_ALLOC
void initPNGDecoder(void)
{
    static void *pngBuffer = nullptr;
    if (!pngBuffer) {
        // force allocation into zero-wait-state Internal SRAM (aligned to 32-bit words)
        pngBuffer = heap_caps_malloc(sizeof(PNG), MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT);

        if (!pngBuffer) {
            ILOG_ERROR("Failed to allocate PNG context in Internal SRAM!");
            return;
        }
    }
    if (!png) {
        png = ::new (pngBuffer) PNG();
    }
}
#else
void initPNGDecoder(void)
{
    static void *pngBuffer = nullptr;
    if (!pngBuffer)
        pngBuffer = lv_malloc(sizeof(PNG));
    if (!png)
        png = ::new (pngBuffer) PNG();
}
#endif

/**
 * Double-Buffering Scratchpad technique:
 * Decode the row into an ultra-fast Internal SRAM buffer (aligned to 32 bits).
 * Transfer that completed row into PSRAM using 32-bit coalesced burst writes
 * (processing two RGB565 pixels per instruction).
 */
bool decodeImgColor(const void *data, size_t size, lv_image_dsc_t **img)
{
    auto PNGDrawCallback = [](PNGDRAW *pDraw) -> int {
        struct PNGDrawContext *ctx = (struct PNGDrawContext *)pDraw->pUser;

        // 8-bit pointer for the tail loop, and 32-bit pointer for fast PSRAM burst writes
        uint16_t *rowDst16 = ctx->destBuffer + (pDraw->y * ctx->width);
        uint32_t *rowDst32 = (uint32_t *)rowDst16;

        // CRITICAL SPEED FIX: Intermediate scratchpad in ultra-fast Internal SRAM.
        // Aligned to 4 bytes for 32-bit burst transfers.
        // 4096 bytes holds up to a 2048-pixel wide RGB565 row.
        static uint16_t sramScratch[2048] __attribute__((aligned(4)));

        int width = pDraw->iWidth;
        if (width > 2048)
            return 0; // Guard against buffer overflow

        // Decode the line entirely inside Internal SRAM (extremely fast)
        ctx->pngInstance->getLineAsRGB565(pDraw, sramScratch, PNG_RGB565_LITTLE_ENDIAN, 0);

        const uint32_t *src32 = (const uint32_t *)sramScratch;
        int i = 0;

        // Burst Loop: Pack two 16-bit RGB565 pixels into a single 32-bit word
        // to maximize the ESP32-S3 external PSRAM cache line usage.
        for (; i <= width - 2; i += 2) {
            // Read 2 pixels at a time from internal SRAM, write 2 pixels at a time to PSRAM
            *rowDst32++ = *src32++;
        }

        // Tail cleanup loop (if image width is odd)
        if (i < width) {
            uint16_t *dst16 = (uint16_t *)rowDst32;
            const uint16_t *src16 = (const uint16_t *)src32;
            *dst16 = *src16;
        }

        return 1;
    };

    uint32_t start = millis();
    int rc = png->openRAM((uint8_t *)data, size, PNGDrawCallback);
    if (rc != PNG_SUCCESS) {
        ILOG_ERROR("PNGdec failed to open image RAM data: %d", rc);
        return false;
    }

    int width = png->getWidth();
    int height = png->getHeight();

    // Allocate final destination canvas memory in PSRAM
    uint16_t *rgb565Data = (uint16_t *)lv_malloc(width * height * sizeof(uint16_t));
    if (!rgb565Data) {
        png->close();
        return false;
    }

    struct PNGDrawContext ctx {
        png, rgb565Data, width
    };

    // Decode (0 disables CRC checks to speed up runtime)
    rc = png->decode(&ctx, 0);
    png->close();

    if (rc != PNG_SUCCESS) {
        ILOG_ERROR("PNGdec decoding process failed: %d", rc);
        lv_free(rgb565Data);
        return false;
    }

    // Configure the LVGL 9 image descriptor
    *img = (lv_image_dsc_t *)lv_malloc_zeroed(sizeof(lv_image_dsc_t));
    if (!*img) {
        lv_free(rgb565Data);
        return false;
    }

    (*img)->header.magic = LV_IMAGE_HEADER_MAGIC;
    (*img)->header.w = width;
    (*img)->header.h = height;
    (*img)->header.cf = LV_COLOR_FORMAT_RGB565;
    (*img)->header.flags = LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_USER1;
    (*img)->data = (uint8_t *)rgb565Data;
    (*img)->data_size = width * height * sizeof(uint16_t);

    ILOG_DEBUG("FAST decodeImgColor took %d ms", millis() - start);
    return true;
}

#ifdef USE_FAST_PNGDEC
bool decodeImgGrey(const void *data, size_t size, lv_image_dsc_t **img)
{
    auto PNGGreyDrawCallback = [](PNGDRAW *pDraw) -> int {
        struct PNGGreyDrawContext *ctx = (struct PNGGreyDrawContext *)pDraw->pUser;

        // Direct 8-bit pointer for the tail loop, and a 32-bit pointer for fast PSRAM burst writes
        uint8_t *rowDst8 = ctx->destBuffer + (pDraw->y * ctx->width);
        uint32_t *rowDst32 = (uint32_t *)rowDst8;

        // CRITICAL SPEED FIX: Move the temporary buffer out of the stack and into ultra-fast Internal SRAM.
        // We allocate it statically once. 4096 bytes covers up to a 1365-pixel wide image.
        // Using '__attribute__((aligned(4)))' guarantees the ESP32-S3 can do 32-bit wide burst reads.
        static uint8_t rgbRow[4096] __attribute__((aligned(4)));

        // Ensure we don't overflow our fast static buffer for abnormally large images
        int width = pDraw->iWidth;
        if (width * 3 > 4096)
            return 0;

        // Execute the PNGdec native decoder directly into our fast internal SRAM buffer
        typedef void (*PNG_CONVERT_CALLBACK)(PNGDRAW *, void *, int, int, int, int, void *, int);
        PNG_CONVERT_CALLBACK pfnConvert = (PNG_CONVERT_CALLBACK)pDraw->pPixels;
        pfnConvert(pDraw, rgbRow, 0, width, 0, 0, NULL, 0);

        // Use our 32-bit aligned pointers to extract and process data
        const uint32_t *src32 = (const uint32_t *)rgbRow;
        int i = 0;

        // Vectorized Loop: Process 4 pixels (12 bytes) per iteration using 32-bit register math
        for (; i <= width - 4; i += 4) {
            // High-speed internal SRAM 32-bit reads
            uint32_t w0 = src32[0]; // Contains P0.R, P0.G, P0.B, P1.R
            uint32_t w1 = src32[1]; // Contains P1.G, P1.B, P2.R, P2.G
            uint32_t w2 = src32[2]; // Contains P2.B, P3.R, P3.G, P3.B

            // Pixel 0 Math
            uint32_t r0 = (w0)&0xFF;
            uint32_t g0 = (w0 >> 8) & 0xFF;
            uint32_t b0 = (w0 >> 16) & 0xFF;
            uint32_t gray0 = (r0 * 77 + g0 * 150 + b0 * 29) >> 8;

            // Pixel 1 Math
            uint32_t r1 = (w0 >> 24);
            uint32_t g1 = (w1)&0xFF;
            uint32_t b1 = (w1 >> 8) & 0xFF;
            uint32_t gray1 = (r1 * 77 + g1 * 150 + b1 * 29) >> 8;

            // Pixel 2 Math
            uint32_t r2 = (w1 >> 16) & 0xFF;
            uint32_t g2 = (w1 >> 24);
            uint32_t b2 = (w2)&0xFF;
            uint32_t gray2 = (r2 * 77 + g2 * 150 + b2 * 29) >> 8;

            // Pixel 3 Math
            uint32_t r3 = (w2 >> 8) & 0xFF;
            uint32_t g3 = (w2 >> 16) & 0xFF;
            uint32_t b3 = (w2 >> 24);
            uint32_t gray3 = (r3 * 77 + g3 * 150 + b3 * 29) >> 8;

            // Burst Write: Packs 4 grayscale pixels into one 32-bit word.
            // This fires a single unified write instruction directly into the PSRAM bus.
            *rowDst32++ = (gray3 << 24) | (gray2 << 16) | (gray1 << 8) | gray0;

            src32 += 3; // Advance source by 3 words (12 bytes)
        }

        // Tail loop: Clean up any remaining pixels (if image width isn't a multiple of 4)
        const uint8_t *src8 = (const uint8_t *)src32;
        uint8_t *dst8 = (uint8_t *)rowDst32;
        for (; i < width; i++) {
            uint32_t r = src8[0];
            uint32_t g = src8[1];
            uint32_t b = src8[2];
            *dst8++ = (uint8_t)((r * 77 + g * 150 + b * 29) >> 8);
            src8 += 3;
        }

        return 1;
    };

    uint32_t start = millis();
    int rc = png->openRAM((uint8_t *)data, size, PNGGreyDrawCallback);
    if (rc != PNG_SUCCESS) {
        ILOG_ERROR("PNGdec failed to open greyscale image RAM data: %d", rc);
        return false;
    }

    int width = png->getWidth();
    int height = png->getHeight();

    // Allocate 1-byte per pixel canvas array in PSRAM
    uint8_t *l8Data = (uint8_t *)lv_malloc(width * height * sizeof(uint8_t));
    if (!l8Data) {
        png->close();
        return false;
    }

    struct PNGGreyDrawContext ctx {
        png, l8Data, width
    };

    rc = png->decode(&ctx, 0);
    png->close();

    if (rc != PNG_SUCCESS) {
        ILOG_ERROR("PNGdec greyscale decoding process failed: %d", rc);
        lv_free(l8Data);
        return false;
    }

    *img = (lv_image_dsc_t *)lv_malloc_zeroed(sizeof(lv_image_dsc_t));
    if (!*img) {
        lv_free(l8Data);
        return false;
    }

    (*img)->header.magic = LV_IMAGE_HEADER_MAGIC;
    (*img)->header.w = width;
    (*img)->header.h = height;
    (*img)->header.cf = LV_COLOR_FORMAT_L8;
    (*img)->header.flags = LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_USER1;
    (*img)->data = l8Data;
    (*img)->data_size = width * height * sizeof(uint8_t);

    ILOG_DEBUG("FAST decodeImgGrey took %d ms", millis() - start);
    return true;
}

#else

bool decodeImgGrey(const void *data, size_t size, lv_image_dsc_t **img)
{
    auto PNGGreyDrawCallback = [](PNGDRAW *pDraw) -> int {
        struct PNGGreyDrawContext *ctx = (struct PNGGreyDrawContext *)pDraw->pUser;
        uint8_t *rowDst = ctx->destBuffer + (pDraw->y * ctx->width);
        uint8_t rgbRow[pDraw->iWidth * 3];

        typedef void (*PNG_CONVERT_CALLBACK)(PNGDRAW *, void *, int, int, int, int, void *, int);
        PNG_CONVERT_CALLBACK pfnConvert = (PNG_CONVERT_CALLBACK)pDraw->pPixels;
        pfnConvert(pDraw, rgbRow, 0, pDraw->iWidth, 0, 0, NULL, 0);

        for (int i = 0; i < pDraw->iWidth; i++) {
            uint8_t r = rgbRow[i * 3];
            uint8_t g = rgbRow[i * 3 + 1];
            uint8_t b = rgbRow[i * 3 + 2];
            rowDst[i] = (uint8_t)((r * 77 + g * 150 + b * 29) >> 8);
        }
        return 1;
    };

    uint32_t start = millis();
    int rc = png->openRAM((uint8_t *)data, size, PNGGreyDrawCallback);
    if (rc != PNG_SUCCESS) {
        ILOG_ERROR("PNGdec failed to open greyscale image RAM data: %d", rc);
        return false;
    }

    int width = png->getWidth();
    int height = png->getHeight();

    // Allocate 1-byte per pixel canvas array in PSRAM
    uint8_t *l8Data = (uint8_t *)lv_malloc(width * height * sizeof(uint8_t));
    if (!l8Data) {
        png->close();
        return false;
    }

    struct PNGGreyDrawContext ctx {
        png, l8Data, width
    };

    rc = png->decode(&ctx, 0);
    png->close();

    if (rc != PNG_SUCCESS) {
        ILOG_ERROR("PNGdec greyscale decoding process failed: %d", rc);
        lv_free(l8Data);
        return false;
    }

    *img = (lv_image_dsc_t *)lv_malloc_zeroed(sizeof(lv_image_dsc_t));
    if (!*img) {
        lv_free(l8Data);
        return false;
    }

    (*img)->header.magic = LV_IMAGE_HEADER_MAGIC;
    (*img)->header.w = width;
    (*img)->header.h = height;
    (*img)->header.cf = LV_COLOR_FORMAT_L8;
    (*img)->header.flags = LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_USER1;
    (*img)->data = l8Data;
    (*img)->data_size = width * height * sizeof(uint8_t);

    ILOG_DEBUG("decodeImgGrey took %d ms", millis() - start);
    return true;
}
#endif