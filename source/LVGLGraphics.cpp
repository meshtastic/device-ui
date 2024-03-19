#include "LVGLGraphics.h"
#include "ILog.h"
#include "assert.h"

LVGLGraphics::LVGLGraphics(uint16_t width, uint16_t height) : screenWidth(width), screenHeight(height)
{

#if defined(USE_DOUBLE_BUFFER) // speedup drawing by using double-buffered DMA mode
    bufsize = width * height / 8 * sizeof(lv_color_t);
#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    ILOG_DEBUG("LVGL: allocating %u bytes PSRAM for double buffering\n"), bufsize;
    assert(ESP.getFreePsram());
    // buf1 = (lv_color_t*)heap_caps_malloc(bufsize, MALLOC_CAP_INTERNAL |
    // MALLOC_CAP_DMA);  //assert failed: block_trim_free heap_tlsf.c:371 buf2 =
    // (lv_color_t*)heap_caps_malloc(bufsize, MALLOC_CAP_INTERNAL |
    // MALLOC_CAP_DMA); buf1 = (lv_color_t*)heap_caps_malloc((bufsize + 3) & ~3,
    // MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT); // crash buf1 =
    // (lv_color_t*)heap_caps_malloc(bufsize, MALLOC_CAP_SPIRAM); // crash buf1 =
    // (lv_color_t*)ps_malloc(bufsize); // crash
    buf1 = (lv_color_t *)heap_caps_aligned_alloc(32, (bufsize + 3) & ~3, MALLOC_CAP_SPIRAM);
    // buf2 = (lv_color_t*)heap_caps_aligned_alloc(16, (bufsize + 3) & ~3,
    // MALLOC_CAP_SPIRAM);
    draw_buf = (lv_disp_draw_buf_t *)heap_caps_aligned_alloc(32, sizeof(lv_disp_draw_buf_t), MALLOC_CAP_SPIRAM);
#else
    ILOG_DEBUG("LVGL: allocating %u bytes heap memory for double buffering\n"), bufsize;
    buf1 = (lv_color_t *)heap_caps_malloc(bufsize, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA); // heap_alloc_dma
    buf2 = (lv_color_t *)heap_caps_malloc(bufsize, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA); // heap_alloc_dma
    draw_buf = (lv_disp_draw_buf_t *)heap_caps_malloc(sizeof(lv_disp_draw_buf_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
#endif
    assert(buf1 != 0 /* && buf2 != 0 */);
#elif 0 // defined BOARD_HAS_PSRAM
    assert(ESP.getFreePsram());
    bufsize = width * height / 8 * sizeof(lv_color_t);
    ILOG_DEBUG("LVGL: allocating %u bytes PSRAM for draw buffer\n"), bufsize;
    buf1 = (lv_color_t *)heap_caps_malloc(bufsize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT); // heap_alloc_psram
    draw_buf = (lv_disp_draw_buf_t *)heap_caps_aligned_alloc(16, sizeof(lv_disp_draw_buf_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(buf1 != 0);
#else
    bufsize = width * 10;
    ILOG_DEBUG("LVGL: allocating %u bytes heap memory for draw buffer\n", bufsize);
    draw_buf = new lv_disp_draw_buf_t;
    buf1 = new lv_color_t[bufsize]; // OK
    assert(buf1 != 0);
#endif
}

void LVGLGraphics::init(void)
{
    ILOG_DEBUG("LV init...\n");
    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb(lv_debug); // register print function for debugging
#endif

    // LVGL: Setting up buffer to use for display
    lv_disp_draw_buf_init(draw_buf, buf1, buf2, bufsize);
}

// debugging callback
void LVGLGraphics::lv_debug(const char *buf)
{
    ILOG_DEBUG("%s", buf);
}
