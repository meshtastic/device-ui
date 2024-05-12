#pragma once

#include "DisplayDriverConfig.h"
#include "ILog.h"
#include "LovyanGFX.h"
#include "TFTDriver.h"
#include <functional>

const uint32_t displayTimeout = 60 * 1000;
const uint32_t defaultBrightness = 128;

template <class LGFX> class LGFXDriver : public TFTDriver<LGFX>
{
  public:
    LGFXDriver(uint16_t width, uint16_t height);
    LGFXDriver(const DisplayDriverConfig &cfg);
    void init(DeviceGUI *gui) override;
    bool hasTouch() override { return lgfx->touch(); }
    bool isPowersaving() override { return powerSaving; }
    void task_handler(void) override;

    virtual bool hasLight(void) { return lgfx->light(); }

  protected:
    // lvgl callbacks have to be static cause it's a C library, not C++
    static void display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
    static void touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data);

    static uint32_t lastTouch;
    bool powerSaving;

  private:
    void init_lgfx(void);

    static LGFX *lgfx;
    size_t bufsize;
    lv_color_t *buf1;
    lv_color_t *buf2;
};

template <class LGFX> LGFX *LGFXDriver<LGFX>::lgfx = nullptr;
template <class LGFX> uint32_t LGFXDriver<LGFX>::lastTouch = 0;

template <class LGFX>
LGFXDriver<LGFX>::LGFXDriver(uint16_t width, uint16_t height)
    : TFTDriver<LGFX>(lgfx ? lgfx : new LGFX, width, height), powerSaving(false), bufsize(0), buf1(nullptr), buf2(nullptr)
{
    lgfx = this->tft;
    lastTouch = millis();
}

template <class LGFX>
LGFXDriver<LGFX>::LGFXDriver(const DisplayDriverConfig &cfg)
    : TFTDriver<LGFX>(lgfx ? lgfx : new LGFX(cfg), cfg.width(), cfg.height()), powerSaving(false)
{
    lgfx = this->tft;
    lastTouch = millis();
}

template <class LGFX> void LGFXDriver<LGFX>::task_handler(void)
{
    if ((hasTouch() /* || hasButton() */) && hasLight()) {
        if (lastTouch + displayTimeout < millis()) {
            if (!powerSaving) {
                // dim display brightness slowly down
                uint32_t brightness = lgfx->getBrightness();
                if (brightness > 1) {
                    lgfx->setBrightness(brightness - 1);
                } else {
                    lgfx->sleep();
                    lgfx->powerSaveOn();
                    powerSaving = true;
                }
            }
        }
        if (powerSaving) {
            if (DisplayDriver::view->sleep(lgfx->touch()->config().pin_int) || lastTouch + displayTimeout > millis()) {
                // woke up by touch or button
                powerSaving = false;
                lastTouch = millis();
                lgfx->powerSaveOff();
                lgfx->wakeup();
                lgfx->setBrightness(defaultBrightness);
            } else {
                // we woke up due to e.g. serial traffic (or sleep() simply not implemented)
                // continue with processing loop and enter sleep() again next round
            }
        }
    }
#ifdef HAS_FREE_RTOS
    lv_tick_set_cb(xTaskGetTickCount);
#endif

    DisplayDriver::task_handler();
}

// Display flushing not using DMA */

template <class LGFX> void LGFXDriver<LGFX>::display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    lgfx->pushImage(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (uint16_t *)px_map);
    lv_display_flush_ready(disp);
}

#if 0
template <class LGFX> void LGFXDriver<LGFX>::display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    lgfx->startWrite();
    lgfx->setAddrWindow(area->x1, area->y1, w, h);
    lgfx->writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
    lgfx->endWrite();
    lv_disp_flush_ready(disp); // TODO put into LGFX callback for DMA
}

/* Display flushing using DMA */
template <class LGFX>
void LGFXDriver<LGFX>::display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    lgfx->startWrite();
    lgfx->setAddrWindow(area->x1, area->y1, w, h);
    lgfx->writePixelsDMA((lgfx::rgb565_t *)&color_p->full, w * h);
    lgfx->endWrite();
    lv_disp_flush_ready( disp ); //FIXME must put into some LGFX callback for DMA double-buffering
}

// Display flushing using DMA
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    if (lgfx->getStartCount() == 0)
    {   // Processing if not yet started
        lgfx->startWrite();
    }
    lgfx->pushImageDMA( area->x1
                    , area->y1
                    , area->x2 - area->x1 + 1
                    , area->y2 - area->y1 + 1
                    , ( lgfx::swap565_t* )&color_p->full);
    lv_disp_flush_ready( disp ); //TODO must put into LGFX callback for DMA double-buffering
}
#endif

template <class LGFX> void LGFXDriver<LGFX>::touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;
    bool touched = lgfx->getTouch(&touchX, &touchY);
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        lastTouch = millis();
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;

        ILOG_DEBUG("Touch(%hd/%hd)\n", touchX, touchY);
#if 0
        if (data->point.x < 0)
            data->point.x = 0;
        if (data->point.y < 0)
            data->point.y = 0;
        if (data->point.x >= lgfx->touch()->config()->x_max)
            data->point.x = lgfx->touch()->config()->x_max;
        if (data->point.y >= lgfx->touch()->config()->y_max)
            data->point.y = lgfx->touch()->config()->y_max;
#endif
        // ILOG_DEBUG("touch %d/%d\n", data->point.x, data->point.y);
    }
}

template <class LGFX> void LGFXDriver<LGFX>::init(DeviceGUI *gui)
{
    ILOG_DEBUG("LGFXDriver<LGFX>::init...\n");
    init_lgfx();
    TFTDriver<LGFX>::init(gui);

    // LVGL: setup display device driver
    ILOG_DEBUG("LVGL display driver init...\n");

    DisplayDriver::display = lv_display_create(DisplayDriver::screenWidth, DisplayDriver::screenHeight);

#if defined(USE_DOUBLE_BUFFER) // speedup drawing by using double-buffered DMA mode
    bufsize = screenWidth * screenHeight / 8 * sizeof(lv_color_t);
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
#endif
    assert(buf1 != 0 /* && buf2 != 0 */);
    lv_display_set_buffers(disp, buf1, buf2, bufsize, LV_DISPLAY_RENDER_MODE_DIRECT);
#elif 0 // defined BOARD_HAS_PSRAM
    assert(ESP.getFreePsram());
    bufsize = screenWidth * height / 8 * sizeof(lv_color_t);
    ILOG_DEBUG("LVGL: allocating %u bytes PSRAM for draw buffer\n"), bufsize;
    buf1 = (lv_color_t *)heap_caps_malloc(bufsize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT); // heap_alloc_psram
    assert(buf1 != 0);
    lv_display_set_buffers(display, buf1, buf2, bufsize, LV_DISPLAY_RENDER_MODE_PARTIAL);
#else
    bufsize = this->screenWidth * 10;
    buf1 = new lv_color_t[bufsize];
    assert(buf1 != 0);
    ILOG_DEBUG("LVGL: allocating %u bytes heap memory for draw buffer\n", sizeof(lv_color_t) * bufsize);
    lv_display_set_buffers(this->display, buf1, buf2, sizeof(lv_color_t) * bufsize, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

    lv_display_set_flush_cb(this->display, LGFXDriver::display_flush);

    // TODO lv_display_set_resolution(display, screenWidth, screenHeight);

#if 0
   /* Example 2
     * Two buffers for partial rendering
     * In flush_cb DMA or similar hardware should be used to update the display in the background.*/
    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];
    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];
    lv_display_set_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* Example 3
     * Two buffers screen sized buffer for double buffering.
     * Both LV_DISPLAY_RENDER_MODE_DIRECT and LV_DISPLAY_RENDER_MODE_FULL works, see their comments*/
    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];
    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];
    lv_display_set_buffers(disp, buf_3_1, buf_3_2, sizeof(buf_3_1), LV_DISPLAY_RENDER_MODE_DIRECT);
#endif

#if 0    
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = this->screenWidth;
    disp_drv.ver_res = this->screenHeight;
    disp_drv.flush_cb = &LGFXDriver::display_flush;
    disp_drv.draw_buf = DisplayDriver::lvgl.get_draw_buf();
#ifdef BOARD_HAS_PSRAM
    // disp_drv.full_refresh = 1; //needs allocate entire draw_buf width*height in
    // PSRAM
#endif

    lv_disp_drv_register(&disp_drv);

    if (hasTouch()) {
        // LVGL: setup input device driver
        ILOG_DEBUG("LVGL input driver init...\n");
        static lv_indev_drv_t indev_drv;
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.read_cb = &touchpad_read;
        lv_indev_drv_register(&indev_drv);
    }
#endif
}

template <class LGFX> void LGFXDriver<LGFX>::init_lgfx(void)
{
    // Initialize LovyanGFX
    ILOG_DEBUG("LGFX init...\n");
    TFTDriver<LGFX>::tft->init();
    TFTDriver<LGFX>::tft->setBrightness(defaultBrightness);
    TFTDriver<LGFX>::tft->fillScreen(LGFX::color565(0x3D, 0xDA, 0x83));

    if (hasTouch()) {
#ifdef CALIBRATE_TOUCH
        ILOG_INFO("Calibrating touch...\n");
#ifdef T_DECK
        // FIXME: read calibration data from persistent storage using lfs_file_read
        // uint16_t parameters[8] = {3, 13, 1, 316, 227, 19, 231, 311};
        uint16_t parameters[8] = {11, 19, 6, 314, 218, 15, 229, 313};
#elif defined(T_HMI)
        uint16_t parameters[8] = {399, 293, 309, 3701, 3649, 266, 3678, 3689};
#elif defined(ESP32_2432S022)
        uint16_t parameters[8] = {1, 2, 69, 313, 187, 5, 239, 314};
#elif defined(ESP32_2432S028RV1)
        uint16_t parameters[8] = {278, 3651, 228, 173, 3819, 3648, 3815, 179};
#elif defined(NODEMCU_32S) || defined(PORTDUINO)
        uint16_t parameters[8] = {255, 3691, 203, 198, 3836, 3659, 3795, 162};
#else
        uint16_t parameters[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        ILOG_WARN("Touch screen has no calibration data!!!\n");
#endif

#if !CALIBRATE_TOUCH
        TFTDriver<LGFX>::tft->setTouchCalibrate(parameters);
#else
        TFTDriver<LGFX>::tft->setTextSize(2);
        TFTDriver<LGFX>::tft->setTextDatum(textdatum_t::middle_center);
        TFTDriver<LGFX>::tft->drawString("touch the arrow marker.", TFTDriver<LGFX>::tft->width() >> 1,
                                         TFTDriver<LGFX>::tft->height() >> 1);
        TFTDriver<LGFX>::tft->setTextDatum(textdatum_t::top_left);
        std::uint16_t fg = TFT_BLUE;
        std::uint16_t bg = LGFX::color565(0x40, 0xFF, 0x72);
        if (TFTDriver<LGFX>::tft->isEPD())
            std::swap(fg, bg);
        TFTDriver<LGFX>::tft->calibrateTouch(parameters, fg, bg,
                                             std::max(TFTDriver<LGFX>::tft->width(), TFTDriver<LGFX>::tft->height()) >> 3);

#endif
        // FIXME: store parameters[] using lfs_file_write
        ILOG_DEBUG("Touchscreen calibration parameters: {%d, %d, %d, %d, %d, %d, %d, %d}\n", parameters[0], parameters[1],
                   parameters[2], parameters[3], parameters[4], parameters[5], parameters[6], parameters[7]);
#endif
    }
}
