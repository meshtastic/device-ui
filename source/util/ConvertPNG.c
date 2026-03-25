#include "core/lv_global.h"
#include "libs/lodepng/lodepng.h"
#include <string.h>

/* ---- stb_image PSRAM arena allocator ----------------------------------------
 * On ESP32-S3, the PSRAM heap gets fragmented by stbi's repeated realloc calls
 * during zlib decompression.  Fix: route all stbi allocations through a single
 * contiguous PSRAM buffer allocated once at boot.
 *
 * Usage per 256x256 tile:  STBI_rgb = 192 KB,  STBI_grey = 64 KB.
 * A 440 KB arena covers both cases with enough headroom for scratch buffers.
 *
 * The arena is reset (used = 0) after every decode; no individual frees needed
 * because decoding is single-threaded and processes one tile at a time.
 * --------------------------------------------------------------------------- */
#ifdef ARDUINO_ARCH_ESP32
#include <esp_heap_caps.h>

#ifndef STBI_ARENA_SIZE
#define STBI_ARENA_SIZE (384u * 1024u) /* NOTE: works only for STBI_rgb */
#endif

static uint8_t *s_stbi_arena = NULL;
static size_t s_stbi_arena_used = 0;

static void stbi_arena_init(void)
{
    if (!s_stbi_arena) {
        s_stbi_arena = (uint8_t *)heap_caps_aligned_alloc(32, STBI_ARENA_SIZE, MALLOC_CAP_SPIRAM);
        if (!s_stbi_arena) {
            LV_LOG_ERROR("stbi_arena_init: (%u bytes at %p) FAILED (largest: %d)", STBI_ARENA_SIZE, s_stbi_arena,
                         heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
        }
    }
}

static void *stbi_arena_malloc(size_t sz)
{
    sz = (sz + 3u) & ~3u; /* 4-byte align */
    if (!s_stbi_arena || s_stbi_arena_used + sz > STBI_ARENA_SIZE)
        return NULL;
    void *p = s_stbi_arena + s_stbi_arena_used;
    s_stbi_arena_used += sz;
    return p;
}

static void *stbi_arena_realloc_sized(void *p, size_t oldsz, size_t newsz)
{
    if (!s_stbi_arena)
        return NULL;

    oldsz = (oldsz + 3u) & ~3u;
    newsz = (newsz + 3u) & ~3u;

    if (!p) {
        return stbi_arena_malloc(newsz);
    }

    /* Defensive: never touch pointers that are not from our arena. */
    uint8_t *pb = (uint8_t *)p;
    uint8_t *arena_begin = s_stbi_arena;
    uint8_t *arena_end = s_stbi_arena + STBI_ARENA_SIZE;
    if (pb < arena_begin || pb >= arena_end)
        return NULL;

    /* In-place growth when p is the top block (common case in stbi's zlib loop) */
    if (pb + oldsz == s_stbi_arena + s_stbi_arena_used) {
        size_t base = s_stbi_arena_used - oldsz;
        if (base + newsz > STBI_ARENA_SIZE)
            return NULL;
        s_stbi_arena_used = base + newsz;
        return p;
    }
    /* Fallback: allocate a fresh block and copy (should not occur in practice) */
    void *np = stbi_arena_malloc(newsz);
    if (np && oldsz > 0)
        memcpy(np, p, oldsz < newsz ? oldsz : newsz);
    return np;
}

static void stbi_arena_free(void *p)
{
    (void)p;
} /* no-op; arena reset between tiles */

static void stbi_arena_reset(void)
{
    s_stbi_arena_used = 0;
}

#define STBI_MALLOC(sz) stbi_arena_malloc(sz)
#define STBI_REALLOC(p, newsz) stbi_arena_realloc_sized(p, 0, newsz)
#define STBI_REALLOC_SIZED(p, oldsz, newsz) stbi_arena_realloc_sized(p, oldsz, newsz)
#define STBI_FREE(p) stbi_arena_free(p)

#else /* non-ESP32 and native linux */

static void stbi_arena_init(void) {}
static void stbi_arena_reset(void) {}

#define STBI_MALLOC(sz) lv_malloc(sz)
#define STBI_REALLOC(p, newsz) lv_realloc(p, newsz)
#define STBI_FREE(p) lv_free(p)

#endif

#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "graphics/map/stb_image.h"

void convertRGB888ToRGB565(uint8_t *src, uint16_t *dst, int width, int height)
{
    uint32_t totalPixels = width * height;
    uint32_t srcIndex = 0;
    for (int i = 0; i < totalPixels; i++) {
        uint8_t r = src[srcIndex];
        uint8_t g = src[srcIndex + 1];
        uint8_t b = src[srcIndex + 2];
        dst[i] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3); // Convert RGB888 to RGB565
        srcIndex += 3;
    }
}

bool decodeImgColor(const void *data, size_t size, lv_img_dsc_t **img)
{
    if (!data || !img)
        return false;
    stbi_arena_init();

    int width, height, channels;
    uint8_t *decodedData = stbi_load_from_memory((stbi_uc *)data, (int)size, &width, &height, &channels, STBI_rgb);
    if (!decodedData) {
        stbi_arena_reset();
        LV_LOG_ERROR("stbi_load_from_memory failed: %s", stbi_failure_reason());
        return false;
    }

    uint16_t *rgb565Data = (uint16_t *)lv_malloc(width * height * sizeof(uint16_t));
    if (!rgb565Data) {
        stbi_image_free(decodedData);
        stbi_arena_reset();
        return false;
    }

    convertRGB888ToRGB565(decodedData, rgb565Data, width, height);
    stbi_image_free(decodedData);
    stbi_arena_reset();

    *img = (lv_img_dsc_t *)lv_malloc_zeroed(sizeof(lv_img_dsc_t));
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
    return true;
}

bool decodeImgGrey(const void *data, size_t size, lv_img_dsc_t **img)
{
    if (!data || !img)
        return false;
    stbi_arena_init();

    int width, height, channels;
    uint8_t *decodedData = stbi_load_from_memory((stbi_uc *)data, (int)size, &width, &height, &channels, STBI_grey);
    if (!decodedData) {
        LV_LOG_ERROR("stbi_load_from_memory failed: %s", stbi_failure_reason());
        stbi_arena_reset();
        return false;
    }

    /* copy image data into lv_malloc to free stbi arena */
    size_t dataSize = (size_t)width * (size_t)height;
    uint8_t *l8Data = (uint8_t *)lv_malloc(dataSize);
    if (!l8Data) {
        stbi_image_free(decodedData);
        stbi_arena_reset();
        return false;
    }
    memcpy(l8Data, decodedData, dataSize);
    stbi_image_free(decodedData);
    stbi_arena_reset();

    *img = (lv_img_dsc_t *)lv_malloc_zeroed(sizeof(lv_img_dsc_t));
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
    (*img)->data_size = dataSize;
    return true;
}

/* lodepng decoders (requires lodepng patch) */

#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

lv_draw_buf_t *convert_rgb24_to_l8(const lv_draw_buf_t *src)
{
    if (!src || !src->data)
        return NULL;

    const unsigned w = src->header.w;
    const unsigned h = src->header.h;

    lv_draw_buf_t *dst =
        lv_draw_buf_create_ex(image_cache_draw_buf_handlers, w, h, LV_COLOR_FORMAT_L8, LV_DRAW_BUF_STRIDE(w, LV_COLOR_FORMAT_L8));
    if (!dst)
        return NULL;

    const uint8_t *srcp = (const uint8_t *)src->data; /* RGB888 packed */
    uint8_t *dstp = (uint8_t *)dst->data;
    const uint32_t px_cnt = (uint32_t)w * (uint32_t)h;

    for (uint32_t i = 0; i < px_cnt; i++) {
        const uint8_t r = srcp[0];
        const uint8_t g = srcp[1];
        const uint8_t b = srcp[2];

        /* gray-scale integer approximation of 0.299*R + 0.587*G + 0.114*B */
        *dstp = (uint8_t)(((uint16_t)r * 77u + (uint16_t)g * 150u + (uint16_t)b * 29u) >> 8);

        srcp += 3;
        dstp += 1;
    }

    return dst;
}

lv_draw_buf_t *convert_rgb24_to_rgb565(const lv_draw_buf_t *src)
{
    if (!src || !src->data)
        return NULL;

    const unsigned w = src->header.w;
    const unsigned h = src->header.h;

    lv_draw_buf_t *dst = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, w, h, LV_COLOR_FORMAT_RGB565,
                                               LV_DRAW_BUF_STRIDE(w, LV_COLOR_FORMAT_RGB565));
    if (!dst)
        return NULL;

    const uint8_t *srcp = (const uint8_t *)src->data; /* RGB888 packed */
    uint8_t *dstp = (uint8_t *)dst->data;
    const uint32_t px_cnt = (uint32_t)w * (uint32_t)h;

    for (uint32_t i = 0; i < px_cnt; i++) {
        const uint8_t r = srcp[0];
        const uint8_t g = srcp[1];
        const uint8_t b = srcp[2];

        const uint16_t rgb565 = ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | ((uint16_t)(b >> 3));

#if LV_COLOR_16_SWAP
        dstp[0] = (uint8_t)(rgb565 >> 8);
        dstp[1] = (uint8_t)(rgb565 & 0xFF);
#else
        dstp[0] = (uint8_t)(rgb565 & 0xFF);
        dstp[1] = (uint8_t)(rgb565 >> 8);
#endif

        srcp += 3;
        dstp += 2;
    }

    return dst;
}

lv_draw_buf_t *decode_png24_data(bool color, const void *png_data, size_t png_data_size)
{
    unsigned error = 0;
    unsigned png_width = 0;
    unsigned png_height = 0;
    unsigned char *decoded_raw = NULL;
    lv_draw_buf_t *decoded = NULL;

    if (color) {
        unsigned error = lodepng_decode_memory(&decoded_raw, &png_width, &png_height, png_data, png_data_size, LCT_RGB, 8);
        if (error || !decoded_raw) {
            LV_LOG_ERROR("lodepng RGB decode failed: %d", error);
            return NULL;
        }

        decoded = convert_rgb24_to_rgb565((const lv_draw_buf_t *)decoded_raw);
        lv_draw_buf_destroy((lv_draw_buf_t *)decoded_raw);
        return decoded;
    } else {
        /* Decode directly to 8-bit grayscale to reduce peak allocation pressure. */
        error = lodepng_decode_memory(&decoded_raw, &png_width, &png_height, png_data, png_data_size, LCT_GREY, 8);
        if (error || !decoded_raw) {
            LV_LOG_ERROR("lodepng grayscale decode failed: %d", error);
            return NULL;
        }

        return (lv_draw_buf_t *)decoded_raw;
    }
}
