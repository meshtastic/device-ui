#pragma once

#include "DisplayDriverConfig.h"
#include "ILog.h"
#include "InputDriver.h"
#include "LovyanGFX.h"
#include "TFTDriver.h"
#include <functional>

const uint32_t defaultScreenTimeout = 60 * 1000;
const uint32_t defaultBrightness = 153;

template <class LGFX> class LGFXDriver : public TFTDriver<LGFX>
{
  public:
    LGFXDriver(uint16_t width, uint16_t height);
    LGFXDriver(const DisplayDriverConfig &cfg);
    void init(DeviceGUI *gui) override;
    bool calibrate(void) override;
    bool hasTouch() override { return lgfx->touch(); }
    bool hasLight(void) override { return lgfx->light(); }
    bool isPowersaving() override { return powerSaving; }
    void task_handler(void) override;

    uint8_t getBrightness(void) override { return lgfx->getBrightness(); }
    void setBrightness(uint8_t brightness) override;

    uint8_t getScreenTimeout() override { return screenTimeout / 1000; }
    void setScreenTimeout(uint8_t timeout) override { screenTimeout = timeout * 1000; };

  protected:
    // lvgl callbacks have to be static cause it's a C library, not C++
    static void display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
    static void touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data);
    static uint32_t my_tick_get_cb(void) { return millis(); }

    uint32_t screenTimeout;
    uint32_t lastBrightness;
    bool powerSaving;

  private:
    void init_lgfx(void);

    static LGFX *lgfx;
    size_t bufsize;
    lv_color_t *buf1;
    lv_color_t *buf2;
    bool calibrating;
};

template <class LGFX> LGFX *LGFXDriver<LGFX>::lgfx = nullptr;

template <class LGFX>
LGFXDriver<LGFX>::LGFXDriver(uint16_t width, uint16_t height)
    : TFTDriver<LGFX>(lgfx ? lgfx : new LGFX, width, height), screenTimeout(defaultScreenTimeout),
      lastBrightness(defaultBrightness), powerSaving(false), bufsize(0), buf1(nullptr), buf2(nullptr), calibrating(false)
{
    lgfx = this->tft;
}

template <class LGFX>
LGFXDriver<LGFX>::LGFXDriver(const DisplayDriverConfig &cfg)
    : TFTDriver<LGFX>(lgfx ? lgfx : new LGFX(cfg), cfg.width(), cfg.height()), powerSaving(false), bufsize(0), buf1(nullptr),
      buf2(nullptr), calibrating(false)
{
    lgfx = this->tft;
}

template <class LGFX> void LGFXDriver<LGFX>::task_handler(void)
{
    // handle display timeout
    if ((screenTimeout > 0 && screenTimeout < lv_display_get_inactive_time(NULL)) || powerSaving) {
        // sleep screen only if there are means for wakeup
        if (DisplayDriver::view->getInputDriver()->hasPointerDevice() || hasTouch() ||
            DisplayDriver::view->getInputDriver()->hasKeyboardDevice() /* || hasButton() */) {
            if (hasLight()) {
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
                if (powerSaving) {
                    if (DisplayDriver::view->sleep(lgfx->touch()->config().pin_int) ||
                        screenTimeout > lv_display_get_inactive_time(NULL)) {
                        // woke up by touch or button
                        powerSaving = false;
                        lgfx->powerSaveOff();
                        lgfx->wakeup();
                        lgfx->setBrightness(lastBrightness);
                    } else {
                        // we woke up due to e.g. serial traffic (or sleep() simply not implemented)
                        // continue with processing loop and enter sleep() again next round
                    }
                }
            }
            // no BL pin defined to control brightness, so show blank screen instead
            else {
                if (!powerSaving) {
                    DisplayDriver::view->blankScreen(true);
                    powerSaving = true;
                }
                if (screenTimeout > lv_display_get_inactive_time(NULL)) {
                    DisplayDriver::view->blankScreen(false);
                    powerSaving = false;
                }
            }
        }
    }

    if (!calibrating) {
#ifdef HAS_FREE_RTOS
        lv_tick_set_cb(xTaskGetTickCount);
#else
        lv_tick_set_cb(my_tick_get_cb);
#endif
        DisplayDriver::task_handler();
    }
}

#if 1
// Display flushing not using DMA */
template <class LGFX> void LGFXDriver<LGFX>::display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);
    lv_draw_sw_rgb565_swap(px_map, w * h);
    lgfx->pushImage(area->x1, area->y1, w, h, (uint16_t *)px_map);
    lv_display_flush_ready(disp);
}
#else
// Display flushing using DMA
template <class LGFX> void LGFXDriver<LGFX>::display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);
    lv_draw_sw_rgb565_swap(px_map, w * h);
    if (lgfx->getStartCount() == 0) { // Processing if not yet started
        lgfx->startWrite();
    }
    lgfx->pushImageDMA(area->x1, area->y1, w, h, (uint16_t *)px_map);
    lv_disp_flush_ready(disp); // TODO must put into LGFX callback for DMA double-buffering
}
#endif

template <class LGFX> void LGFXDriver<LGFX>::touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;
    bool touched = lgfx->getTouch(&touchX, &touchY);
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;

        ILOG_DEBUG("Touch(%hd/%hd)\n", touchX, touchY);
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
    lv_display_set_color_format(this->display, LV_COLOR_FORMAT_RGB565);

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
    bufsize = lgfx->screenWidth * 10;
    buf1 = new lv_color_t[bufsize];
    assert(buf1 != 0);
    ILOG_DEBUG("LVGL: allocating %u bytes heap memory for draw buffer\n", sizeof(lv_color_t) * bufsize);
    lv_display_set_buffers(this->display, buf1, buf2, sizeof(lv_color_t) * bufsize, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

    lv_display_set_flush_cb(this->display, LGFXDriver::display_flush);

    ILOG_DEBUG("Set display resolution: %dx%d\n", lgfx->screenWidth, lgfx->screenHeight);
    // lv_display_set_resolution(this->display, lgfx->screenWidth, lgfx->screenHeight);
    // lv_display_set_physical_resolution(this->display, this->screenWidth, this->screenHeight);
    // lv_display_set_rotation(this->display, LV_DISPLAY_ROTATION_90);

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

    if (hasTouch()) {
        lv_indev_t *indev = lv_indev_create();
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, touchpad_read);
        lv_indev_set_display(indev, this->display);
    }
}

template <class LGFX> void LGFXDriver<LGFX>::init_lgfx(void)
{
    // Initialize LovyanGFX
    ILOG_DEBUG("LGFX init...\n");
    lgfx->init();
    lgfx->setBrightness(defaultBrightness);
    lgfx->fillScreen(LGFX::color565(0x3D, 0xDA, 0x83));

    if (hasTouch()) {
#ifdef CALIBRATE_TOUCH
        ILOG_INFO("Calibrating touch...\n");
#ifdef T_DECK
        // FIXME: read calibration data from persistent storage using lfs_file_read
        // uint16_t parameters[8] = {3, 13, 1, 316, 227, 19, 231, 311};
        uint16_t parameters[8] = {11, 19, 6, 314, 218, 15, 229, 313};
#elif defined(WT32_SC01)
        uint16_t parameters[8] = {0, 2, 0, 479, 319, 0, 319, 479};
#elif defined(T_HMI)
        uint16_t parameters[8] = {399, 293, 309, 3701, 3649, 266, 3678, 3689};
#elif defined(ESP32_2432S022)
        uint16_t parameters[8] = {1, 2, 69, 313, 187, 5, 239, 314};
#elif defined(ESP32_2432S028RV1)
        uint16_t parameters[8] = {278, 3651, 228, 173, 3819, 3648, 3815, 179};
#elif defined(NODEMCU_32S) || defined(ARCH_PORTDUINO)
        uint16_t parameters[8] = {255, 3691, 203, 198, 3836, 3659, 3795, 162};
#else
        uint16_t parameters[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        ILOG_WARN("Touch screen has no calibration data!!!\n");
#endif

#if !CALIBRATE_TOUCH
        lgfx->setTouchCalibrate(parameters);
#else
        lgfx->setTextSize(2);
        lgfx->setTextDatum(textdatum_t::middle_center);
        lgfx->drawString("touch the arrow marker.", lgfx->width() >> 1, lgfx->height() >> 1);
        lgfx->setTextDatum(textdatum_t::top_left);
        std::uint16_t fg = TFT_BLUE;
        std::uint16_t bg = LGFX::color565(0x67, 0xEA, 0x94);
        if (lgfx->isEPD())
            std::swap(fg, bg);
        lgfx->calibrateTouch(parameters, fg, bg, std::max(lgfx->width(), lgfx->height()) >> 3);

#endif
        // FIXME: store parameters[] using lfs_file_write
        ILOG_DEBUG("Touchscreen calibration parameters: {%d, %d, %d, %d, %d, %d, %d, %d}\n", parameters[0], parameters[1],
                   parameters[2], parameters[3], parameters[4], parameters[5], parameters[6], parameters[7]);
#endif
    }
}

template <class LGFX> bool LGFXDriver<LGFX>::calibrate(void)
{
    calibrating = true;
    lgfx->clearDisplay();
    lgfx->setTextSize(1);
    lgfx->setTextDatum(textdatum_t::middle_center);
    lgfx->drawString("Tap the tip of the arrow marker.", lgfx->width() >> 1, lgfx->height() >> 1);
    lgfx->setTextDatum(textdatum_t::top_left);
    std::uint16_t fg = TFT_BLUE;
    std::uint16_t bg = LGFX::color565(0x67, 0xEA, 0x94);
    if (lgfx->isEPD())
        std::swap(fg, bg);
    uint16_t parameters[8];
    lgfx->calibrateTouch(parameters, fg, bg, std::max(lgfx->width(), lgfx->height()) >> 3);
    ILOG_DEBUG("Touchscreen calibration parameters: {%d, %d, %d, %d, %d, %d, %d, %d}\n", parameters[0], parameters[1],
               parameters[2], parameters[3], parameters[4], parameters[5], parameters[6], parameters[7]);

    calibrating = false;
    return true;
}

template <class LGFX> void LGFXDriver<LGFX>::setBrightness(uint8_t brightness)
{
    lgfx->setBrightness(brightness);
    lastBrightness = brightness;
}
