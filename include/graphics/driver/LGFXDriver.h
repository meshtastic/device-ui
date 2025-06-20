#pragma once

#include "LovyanGFX.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "graphics/driver/TFTDriver.h"
#include "input/InputDriver.h"
#include "lvgl_private.h"
#include "util/ILog.h"
#include <functional>

constexpr uint32_t defaultLongPressTime = 600; // ms until long press is detected (lvgl default is 400)
constexpr uint32_t defaultGestureLimit = 10;   // x/y diff pixel until a swipe gesture is detected (lvgl default is 50)

constexpr uint32_t defaultScreenTimeout = 30 * 1000;
constexpr uint32_t defaultBrightness = 153;

template <class LGFX> class LGFXDriver : public TFTDriver<LGFX>
{
  public:
    LGFXDriver(uint16_t width, uint16_t height);
    LGFXDriver(const DisplayDriverConfig &cfg);
    void init(DeviceGUI *gui) override;
    bool calibrate(uint16_t parameters[8]) override;
    bool hasTouch(void) override;
    bool hasButton(void) override { return lgfx->hasButton(); }
    bool hasLight(void) override { return lgfx->light(); }
    bool isPowersaving(void) override { return powerSaving; }
    void printConfig(void) override;
    void task_handler(void) override;

    uint8_t getBrightness(void) override { return lgfx->getBrightness(); }
    void setBrightness(uint8_t brightness) override;

    uint16_t getScreenTimeout() override { return screenTimeout / 1000; }
    void setScreenTimeout(uint16_t timeout) override { screenTimeout = timeout * 1000; };

  protected:
    // lvgl callbacks have to be static cause it's a C library, not C++
    static void display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
    static void touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data);

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
    : TFTDriver<LGFX>(lgfx ? lgfx : new LGFX(cfg), cfg.width(), cfg.height()), screenTimeout(defaultScreenTimeout),
      lastBrightness(defaultBrightness), powerSaving(false), bufsize(0), buf1(nullptr), buf2(nullptr), calibrating(false)
{
    lgfx = this->tft;
}

template <class LGFX> bool LGFXDriver<LGFX>::hasTouch(void)
{
#ifdef CUSTOM_TOUCH_DRIVER
    return true;
#else
    return lgfx->touch();
#endif
}

template <class LGFX> void LGFXDriver<LGFX>::task_handler(void)
{
    // handle display timeout
    if ((screenTimeout > 0 && lv_display_get_inactive_time(NULL) > screenTimeout) || powerSaving ||
        (DisplayDriver::view->isScreenLocked())) {
        // sleep screen only if there are means for wakeup
        if (DisplayDriver::view->getInputDriver()->hasPointerDevice() || hasTouch() ||
            DisplayDriver::view->getInputDriver()->hasKeyboardDevice() || hasButton()) {
            if (hasLight()) {
                if (!powerSaving) {
                    // dim display brightness slowly down
                    uint32_t brightness = lgfx->getBrightness();
                    if (brightness > 1) {
                        lgfx->setBrightness(brightness - 1);
                    } else {
                        ILOG_INFO("enter powersave");
                        DisplayDriver::view->screenSaving(true);
                        if (hasTouch() && hasButton()) {
                            ILOG_DEBUG("disable touch, enable button input");
                            lv_indev_enable(DisplayDriver::touch, false);
                            lv_indev_enable(InputDriver::instance()->getButton(), true);
                        }
                        lgfx->sleep();
                        lgfx->powerSaveOn();
                        powerSaving = true;
                    }
                }
                if (powerSaving) {
                    int pin_int = -1;
                    if (hasTouch()) {
#ifndef CUSTOM_TOUCH_DRIVER
                        pin_int = lgfx->touch()->config().pin_int;
#else
                        pin_int = lgfx->getTouchInt();
#endif
                    }
                    if (hasButton()) {
#ifdef BUTTON_PIN // only relevant for CYD scenario
                        pin_int = BUTTON_PIN;
#endif
                    }
                    if ((pin_int >= 0 && DisplayDriver::view->sleep(pin_int)) ||
                        (screenTimeout + 50 > lv_display_get_inactive_time(NULL) && !DisplayDriver::view->isScreenLocked())) {
                        delay(2); // let the CPU finish to restore all register in case of light sleep
                        // woke up by touch or button
                        ILOG_INFO("leaving powersave");
                        powerSaving = false;
                        DisplayDriver::view->triggerHeartbeat();
                        lgfx->powerSaveOff();
                        lgfx->wakeup();
                        lgfx->setBrightness(lastBrightness);
                        DisplayDriver::view->screenSaving(false);
                        if (hasTouch() && hasButton()) {
                            ILOG_DEBUG("enable touch, disable button input");
                            lv_indev_enable(DisplayDriver::touch, true);
                            lv_indev_enable(InputDriver::instance()->getButton(), false);
                        }
                        lv_display_trigger_activity(NULL);
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
                    lgfx->sleep();
                    lgfx->powerSaveOn();
                    powerSaving = true;
                }
                if (screenTimeout > lv_display_get_inactive_time(NULL)) {
                    DisplayDriver::view->blankScreen(false);
                    lgfx->powerSaveOff();
                    lgfx->wakeup();
                    powerSaving = false;
                    lv_disp_trig_activity(NULL);
                }
            }
        }
    } else if (lgfx->getBrightness() < lastBrightness) {
        lgfx->setBrightness(lastBrightness);
        lastBrightness = lgfx->getBrightness();
    }

    if (!calibrating) {
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
    uint16_t touchX = 0, touchY = 0;
#ifdef CUSTOM_TOUCH_DRIVER
    bool touched = lgfx->getTouchXY(&touchX, &touchY);
#else
    bool touched = lgfx->getTouch(&touchX, &touchY);
#endif
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;

        // ILOG_DEBUG("Touch(%hd/%hd)", touchX, touchY);
    }
}

template <class LGFX> void LGFXDriver<LGFX>::init(DeviceGUI *gui)
{
    ILOG_DEBUG("LGFXDriver<LGFX>::init...");
    init_lgfx();
    TFTDriver<LGFX>::init(gui);

    // LVGL: setup display device driver
    ILOG_DEBUG("LVGL display driver init...");

    DisplayDriver::display = lv_display_create(DisplayDriver::screenWidth, DisplayDriver::screenHeight);
    lv_display_set_color_format(this->display, LV_COLOR_FORMAT_RGB565);

#if defined(USE_DOUBLE_BUFFER) // speedup drawing by using double-buffered DMA mode
    bufsize = screenWidth * screenHeight / 8 * sizeof(lv_color_t);
#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    ILOG_DEBUG("LVGL: allocating %u bytes PSRAM for double buffering"), bufsize;
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
    ILOG_DEBUG("LVGL: allocating %u bytes heap memory for double buffering"), bufsize;
    buf1 = (lv_color_t *)heap_caps_malloc(bufsize, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA); // heap_alloc_dma
    buf2 = (lv_color_t *)heap_caps_malloc(bufsize, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA); // heap_alloc_dma
#endif
    assert(buf1 != 0 /* && buf2 != 0 */);
    lv_display_set_buffers(disp, buf1, buf2, bufsize, LV_DISPLAY_RENDER_MODE_DIRECT);
#elif 0 // defined BOARD_HAS_PSRAM
    assert(ESP.getFreePsram());
    bufsize = screenWidth * height / 8 * sizeof(lv_color_t);
    ILOG_DEBUG("LVGL: allocating %u bytes PSRAM for draw buffer"), bufsize;
    buf1 = (lv_color_t *)heap_caps_malloc(bufsize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT); // heap_alloc_psram
    assert(buf1 != 0);
    lv_display_set_buffers(display, buf1, buf2, bufsize, LV_DISPLAY_RENDER_MODE_PARTIAL);
#else
    bufsize = lgfx->screenWidth * lgfx->screenHeight;
    buf1 = new lv_color_t[bufsize];
    assert(buf1 != 0);
    ILOG_DEBUG("LVGL: allocating %u bytes heap memory for draw buffer", sizeof(lv_color_t) * bufsize);
    lv_display_set_buffers(this->display, buf1, buf2, sizeof(lv_color_t) * bufsize, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

    lv_display_set_flush_cb(this->display, LGFXDriver::display_flush);

#if defined(DISPLAY_SET_RESOLUTION)
    ILOG_DEBUG("Set display resolution: %dx%d", lgfx->screenWidth, lgfx->screenHeight);
    lv_display_set_resolution(this->display, lgfx->screenWidth, lgfx->screenHeight);
#endif
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
        DisplayDriver::touch = lv_indev_create();
        DisplayDriver::touch->gesture_limit = defaultGestureLimit;
        lv_indev_set_type(DisplayDriver::touch, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(DisplayDriver::touch, touchpad_read);
        lv_indev_set_display(DisplayDriver::touch, this->display);
        lv_indev_set_long_press_time(DisplayDriver::touch, defaultLongPressTime);
#ifdef USE_TOUCH_EVENTS
        if (lgfx->touch()->config()->pin_int > 0) {
            lv_indev_set_mode(DisplayDriver::touch, LV_INDEV_MODE_EVENT);
        }
#else
        lv_timer_t *timer = lv_indev_get_read_timer(DisplayDriver::touch);
        lv_timer_set_period(timer, 10); // 100Hz as I2C touch controllers support
#endif
    }
}

template <class LGFX> void LGFXDriver<LGFX>::init_lgfx(void)
{
    // Initialize LovyanGFX
    ILOG_DEBUG("LGFX init...");
    lgfx->init();
    lgfx->setBrightness(defaultBrightness);
    lgfx->fillScreen(LGFX::color565(0x3D, 0xDA, 0x83));

    if (hasTouch()) {
#ifndef CUSTOM_TOUCH_DRIVER
#ifdef CALIBRATE_TOUCH
        ILOG_INFO("Calibrating touch...");
#ifdef T_DECK
        // FIXME: read calibration data from persistent storage using lfs_file_read
        uint16_t parameters[8] = {0, 2, 0, 314, 223, 5, 224, 314};
#elif defined(WT32_SC01)
        uint16_t parameters[8] = {0, 2, 0, 479, 319, 0, 319, 479};
#elif defined(T_HMI)
        uint16_t parameters[8] = {399, 293, 309, 3701, 3649, 266, 3678, 3689};
#elif defined(ESP32_2432S022)
        uint16_t parameters[8] = {1, 2, 69, 313, 187, 5, 239, 314};
#elif defined(ESP32_2432S028RV1)
        uint16_t parameters[8] = {278, 3651, 228, 173, 3819, 3648, 3815, 179};
#elif defined(UNPHONE)
        uint16_t parameters[8] = {222, 146, 241, 3812, 3860, 131, 3857, 3813};
#elif defined(NODEMCU_32S) || defined(ARCH_PORTDUINO)
        uint16_t parameters[8] = {255, 3691, 203, 198, 3836, 3659, 3795, 162};
#elif defined(SENSECAP_INDICATOR)
        uint16_t parameters[8] = {23, 3, 0, 479, 476, 2, 475, 479};
#else
        uint16_t parameters[8] = {0, 0, 0, 319, 239, 0, 239, 319};
        ILOG_WARN("Touch screen has no calibration data!!!");
#endif

#if CALIBRATE_TOUCH
        calibrate(parameters);
#else
        lgfx->setTouchCalibrate(parameters);
#endif
#endif
#endif
    }
}

template <class LGFX> bool LGFXDriver<LGFX>::calibrate(uint16_t parameters[8])
{
#ifndef CUSTOM_TOUCH_DRIVER
    if (parameters[0] || parameters[7]) {
        ILOG_DEBUG("setting touch screen calibration data");
        lgfx->setTouchCalibrate(parameters);
    } else {
        calibrating = true;
        std::uint16_t fg = TFT_BLUE;
        std::uint16_t bg = LGFX::color565(0x67, 0xEA, 0x94);
        lgfx->clearDisplay();
        lgfx->fillScreen(LGFX::color565(0x67, 0xEA, 0x94));
        lgfx->setTextSize(1);
        lgfx->setTextDatum(textdatum_t::middle_center);
        lgfx->setTextColor(fg, bg);
        lgfx->drawString("Tap the tip of the arrow marker.", lgfx->width() >> 1, lgfx->height() >> 1);
        lgfx->setTextDatum(textdatum_t::top_left);
        if (lgfx->isEPD())
            std::swap(fg, bg);
        lgfx->calibrateTouch(parameters, fg, bg, std::max(lgfx->width(), lgfx->height()) >> 3);
        calibrating = false;
    }
    ILOG_DEBUG("Touchscreen calibration parameters: {%d, %d, %d, %d, %d, %d, %d, %d}", parameters[0], parameters[1],
               parameters[2], parameters[3], parameters[4], parameters[5], parameters[6], parameters[7]);
#endif
    return true;
}

template <class LGFX> void LGFXDriver<LGFX>::setBrightness(uint8_t brightness)
{
    lgfx->setBrightness(brightness);
    lastBrightness = brightness;
}

template <class LGFX> void LGFXDriver<LGFX>::printConfig(void)
{
    if (lgfx->panel()) {
        auto p = lgfx->panel();
        auto cfg = p->config();
        uint32_t id = p->readCommand(0x04, 0, 4);
        ILOG_DEBUG("Panel id=0x%08x (%dx%d): rst:%d, busy:%d, offX:%d, offY:%d invert:%d, RGB:%d, rotation:%d, offR:%d, read:%d, "
                   "readP:%d, readB:%d, dlen:%d, colordepth:%d",
                   id, p->width(), p->height(), cfg.pin_rst, cfg.pin_busy, cfg.offset_x, cfg.offset_y, p->getInvert(),
                   cfg.rgb_order, (int)p->getRotation(), cfg.offset_rotation, cfg.readable, cfg.dummy_read_pixel,
                   cfg.dummy_read_bits, cfg.dlen_16bit, LV_COLOR_DEPTH);
    }
    if (lgfx->panel() && lgfx->panel()->getBus()) {
        lgfx::v1::bus_type_t type = lgfx->panel()->getBus()->busType();
        switch (type) {
        case lgfx::v1::bus_unknown:
            ILOG_DEBUG("Bus (unknown)");
            break;
        case lgfx::v1::bus_spi: {
            auto cfg = static_cast<lgfx::Bus_SPI *>(lgfx->panel()->getBus())->config();
#ifdef ARCH_PORTDUINO
            auto p = lgfx->panel();
            ILOG_DEBUG("Bus_SPI(%d): cs:%d, clk:%d, miso:%d, mosi:%d, dc:%d", cfg.spi_host, p->config().pin_cs, cfg.pin_sclk,
                       cfg.pin_miso, cfg.pin_mosi, cfg.pin_dc);
#else
            ILOG_DEBUG("Bus_SPI(%d): clk:%d, miso:%d, mosi:%d, dc:%d, 3wire:%d, dma:%d", cfg.spi_host, cfg.pin_sclk, cfg.pin_miso,
                       cfg.pin_mosi, cfg.pin_dc, cfg.spi_3wire, cfg.dma_channel);
#endif
            break;
        }
        case lgfx::v1::bus_i2c: {
            // auto bus = static_cast<lgfx::Bus_I2C*>(lgfx->panel()->getBus());
            ILOG_DEBUG("Bus_I2C");
            break;
        }
#ifndef ARCH_PORTDUINO
        case lgfx::v1::bus_parallel8: {
            // auto bus = static_cast<lgfx::v1::Bus_Parallel8 *>(lgfx->panel()->getBus());
            ILOG_DEBUG("Bus_Parallel8");
            break;
        }
        case lgfx::v1::bus_parallel16: {
            // auto bus = static_cast<lgfx::v1::Bus_Parallel16 *>(lgfx->panel()->getBus());
            ILOG_DEBUG("Bus_Parallel16");
            break;
        }
#endif
        case lgfx::v1::bus_stream:
            ILOG_DEBUG("Bus (Stream)");
            break;
        case lgfx::v1::bus_image_push:
            ILOG_DEBUG("Bus (ImagePush)");
            break;
        default:
            break;
        }
    }

#ifndef CUSTOM_TOUCH_DRIVER
    if (lgfx->touch()) {
        auto cfg = lgfx->touch()->config();
        ILOG_DEBUG("Touch int:%d, rst:%d, rotation:%d, (%d/%d)-(%d/%d) ", cfg.pin_int, cfg.pin_rst, cfg.offset_rotation,
                   cfg.x_min, cfg.y_min, cfg.x_max, cfg.y_max);
        if (cfg.i2c_addr > 0 && cfg.pin_cs == -1) {
            ILOG_DEBUG("Touch I2C(%d:0x%02x): SCL:%d, SCA:%d, freq:%d ", (int)cfg.i2c_port, cfg.i2c_addr, cfg.pin_scl,
                       cfg.pin_sda, cfg.freq);
        } else {
            if (cfg.pin_cs == -1) {
                ILOG_DEBUG("Touch SPI(spidev%d.%d), clk:%d, mosi:%d, miso:%d ", (int)(cfg.spi_host & 0x0f),
                           (int)((cfg.spi_host & 0xf0) >> 4), cfg.pin_cs, cfg.pin_sclk, cfg.pin_mosi, cfg.pin_miso);
            } else {
                ILOG_DEBUG("Touch SPI(%d): cs:%d, clk:%d, mosi:%d, miso:%d ", (int)cfg.spi_host, cfg.pin_cs, cfg.pin_sclk,
                           cfg.pin_mosi, cfg.pin_miso);
            }
        }
    }
#endif
    if (lgfx->light()) {
        // auto cfg = lgfx->light();
        ILOG_DEBUG("BL pin assigned");
    }
}
