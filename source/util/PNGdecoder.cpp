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

// ------------------------------------------------------------------------------

struct PNGGreyDrawContext {
    PNG *pngInstance;
    uint8_t *destBuffer;
    int width;
};

static inline uint8_t rgb565ToGrey(uint16_t pixel)
{
    uint32_t r = (pixel >> 8) & 0xF8;
    uint32_t g = (pixel >> 3) & 0xFC;
    uint32_t b = (pixel << 3) & 0xF8;

    r |= r >> 5;
    g |= g >> 6;
    b |= b >> 5;

    return (uint8_t)((r * 77 + g * 150 + b * 29) >> 8);
}

bool decodeImgGrey(const void *data, size_t size, lv_image_dsc_t **img)
{
    auto PNGGreyDrawCallback = [](PNGDRAW *pDraw) -> int {
        struct PNGGreyDrawContext *ctx = (struct PNGGreyDrawContext *)pDraw->pUser;
        uint8_t *rowDst = ctx->destBuffer + (pDraw->y * ctx->width);
        static uint16_t rgb565Row[2048];
        int width = pDraw->iWidth;

        if (width > 2048)
            return 0;

        ctx->pngInstance->getLineAsRGB565(pDraw, rgb565Row, PNG_RGB565_LITTLE_ENDIAN, 0);

        for (int i = 0; i < width; i++) {
            rowDst[i] = rgb565ToGrey(rgb565Row[i]);
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
