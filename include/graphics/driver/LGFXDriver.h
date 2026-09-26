#pragma once

#include "LovyanGFX.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "graphics/driver/TFTDriver.h"
#include "input/InputDriver.h"
#include "lvgl_private.h"
#include "util/ILog.h"
#include "util/ISpiLock.h"
#include <functional>

// A board whose panel shares its SPI host with the radio or the SD card cannot let a DMA
// transfer run outside the bus lock, so it opts into the serialized flush below.
#if defined(USE_DOUBLE_BUFFER_SHARED_SPI) && !defined(USE_DOUBLE_BUFFER)
#define USE_DOUBLE_BUFFER
#endif
#ifdef USE_DOUBLE_BUFFER
#ifndef LGFX_BUFFER_LINES
#define LGFX_BUFFER_LINES 20
#endif
#endif

constexpr uint32_t defaultLongPressTime = 700;    // ms until long press is detected (lvgl default is 400)
constexpr uint32_t defaultGestureLimit = 10;      // drag threshold in px before scroll starts (lvgl default is 10)
constexpr uint32_t defaultTouchReadPeriodMs = 20; // 50Hz

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
    void forceWakeup(void) override { forcedWakeup = true; }
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
#ifdef USE_FULL_DOUBLE_BUFFER
    static void display_flush_wait(lv_display_t *disp);
#endif
    static void rounder_cb(lv_event_t *e);
    static void touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data);

    uint32_t screenTimeout;
    uint32_t lastBrightness;
    bool powerSaving;
    bool forcedWakeup;

  private:
    void init_lgfx(void);

    static LGFX *lgfx;
    size_t bufsize;
    lv_color_t *buf1;
    lv_color_t *buf2;
    bool calibrating;
#ifdef USE_FULL_DOUBLE_BUFFER
    lv_area_t flushArea = {};
    lv_area_t previousFlushArea = {};
    bool flushAreaValid = false;
    bool previousFlushAreaValid = false;
#endif
};

template <class LGFX> LGFX *LGFXDriver<LGFX>::lgfx = nullptr;

template <class LGFX>
LGFXDriver<LGFX>::LGFXDriver(uint16_t width, uint16_t height)
    : TFTDriver<LGFX>(lgfx ? lgfx : new LGFX, width, height), screenTimeout(defaultScreenTimeout),
      lastBrightness(defaultBrightness), powerSaving(false), forcedWakeup(false), bufsize(0), buf1(nullptr), buf2(nullptr),
      calibrating(false)
{
    lgfx = this->tft;
}

template <class LGFX>
LGFXDriver<LGFX>::LGFXDriver(const DisplayDriverConfig &cfg)
    : TFTDriver<LGFX>(lgfx ? lgfx : new LGFX(cfg), cfg.width(), cfg.height()), screenTimeout(defaultScreenTimeout),
      lastBrightness(defaultBrightness), powerSaving(false), forcedWakeup(false), bufsize(0), buf1(nullptr), buf2(nullptr),
      calibrating(false)
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
    if ((screenTimeout > 0 && lv_display_get_inactive_time(lv_display_get_default()) > screenTimeout) || powerSaving ||
        (DisplayDriver::view->isScreenLocked())) {
        // sleep screen only if there are means for wakeup
        if (DisplayDriver::view->getInputDriver()->hasPointerDevice() || hasTouch() ||
            DisplayDriver::view->getInputDriver()->hasKeyboardDevice() || hasButton()) {
            if (hasLight()) {
                if (!powerSaving) {
                    // dim display brightness slowly down
                    uint32_t brightness = lgfx->getBrightness();
                    if (brightness > 0) {
                        lgfx->setBrightness(brightness - 1);
                    } else {
                        ILOG_INFO("enter powersave");
                        DisplayDriver::view->screenSaving(true);
                        if (hasTouch() && hasButton()) {
                            ILOG_DEBUG("disable touch, enable button input");
                            lv_indev_enable(DisplayDriver::touch, false);
                            lv_indev_enable(InputDriver::instance()->getButton(), true);
                        }
                        {
                            ISpiLock::Guard bus;
                            lgfx->sleep();
                            lgfx->powerSaveOn();
                        }
                        powerSaving = true;
                        forcedWakeup = false;
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
                    if (forcedWakeup || (pin_int >= 0 && DisplayDriver::view->sleep(pin_int)) ||
                        (screenTimeout + 50 > lv_display_get_inactive_time(lv_display_get_default()) &&
                         !DisplayDriver::view->isScreenLocked())) {
                        delay(2); // let the CPU finish to restore all register in case of light sleep
                        // woke up by touch or button
                        ILOG_INFO("leaving powersave");
                        powerSaving = false;
                        forcedWakeup = false;
                        DisplayDriver::view->triggerHeartbeat();
                        {
                            // Scoped so the bus is not held across view->sleep() above,
                            // which suspends the whole SoC in light sleep.
                            ISpiLock::Guard bus;
                            lgfx->powerSaveOff();
                            lgfx->wakeup();
                            lgfx->setBrightness(lastBrightness);
                        }
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
                    {
                        ISpiLock::Guard bus;
                        lgfx->sleep();
                        lgfx->powerSaveOn();
                    }
                    powerSaving = true;
                }
                if (screenTimeout > lv_display_get_inactive_time(lv_display_get_default())) {
                    DisplayDriver::view->blankScreen(false);
                    {
                        ISpiLock::Guard bus;
                        lgfx->powerSaveOff();
                        lgfx->wakeup();
                    }
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

#if defined(USE_DOUBLE_BUFFER_SHARED_SPI)
// DMA flush, panel sharing its SPI host with another peripheral.
//
// The bus lock has to span the whole transfer, so endWrite() - which waits for the DMA -
// is called before the guard drops. That costs the overlap between transfer and the
// rendering of the next area: the alternative is holding the lock across LVGL's render,
// which is the coarse hold that starves the radio.
template <class LGFX> void LGFXDriver<LGFX>::display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);
    {
        ISpiLock::Guard bus;
        lgfx->startWrite();
        lgfx->setAddrWindow(area->x1, area->y1, w, h);
        lgfx->pushPixelsDMA((uint16_t *)px_map, w * h);
        lgfx->endWrite();
    }

    lv_display_flush_ready(disp);
}
#elif defined(USE_DOUBLE_BUFFER)
// DMA flush, panel owning its SPI host.
//
// No bus lock and no wait: the transfer runs while LVGL renders the next area into the
// other buffer. LovyanGFX stalls on the outstanding DMA itself when the next flush sends
// its address window, and endWrite() collects the final one.
template <class LGFX> void LGFXDriver<LGFX>::display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);
    lgfx->setAddrWindow(area->x1, area->y1, w, h);
    lgfx->pushPixelsDMA((uint16_t *)px_map, w * h);
    lv_display_flush_ready(disp);
}
#elif defined(USE_FULL_DOUBLE_BUFFER)
template <class LGFX> void LGFXDriver<LGFX>::display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    auto driver = static_cast<LGFXDriver *>(lv_display_get_driver_data(disp));
    if (!driver->flushAreaValid) {
        driver->flushArea = *area;
        driver->flushAreaValid = true;
    } else {
        lv_area_join(&driver->flushArea, &driver->flushArea, area);
    }

    if (!lv_display_flush_is_last(disp)) {
        lv_display_flush_ready(disp);
        return;
    }

    lv_area_t flush_area = driver->flushArea;
    if (driver->previousFlushAreaValid) {
        lv_area_join(&flush_area, &flush_area, &driver->previousFlushArea);
    }
    driver->previousFlushArea = driver->flushArea;
    driver->previousFlushAreaValid = true;
    driver->flushAreaValid = false;
    if (!lgfx->presentFrameBuffer(px_map, flush_area.x1, flush_area.y1, lv_area_get_width(&flush_area),
                                  lv_area_get_height(&flush_area))) {
        ILOG_ERROR("LVGL: failed to present RGB frame buffer");
        lv_display_flush_ready(disp);
    }
}

template <class LGFX> void LGFXDriver<LGFX>::display_flush_wait(lv_display_t *)
{
    lgfx->waitFrameBuffer();
}
#else
// Display flushing not using DMA */
template <class LGFX> void LGFXDriver<LGFX>::display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);
    {
        ISpiLock::Guard bus;
        lgfx->pushImage(area->x1, area->y1, w, h, (uint16_t *)px_map);
    }
    lv_display_flush_ready(disp);
}
#endif

#ifdef LGFX_AMOLED_ROUNDER
template <class LGFX> void LGFXDriver<LGFX>::rounder_cb(lv_event_t *e)
{
    lv_area_t *area = (lv_area_t *)lv_event_get_param(e);
#if LGFX_AMOLED_ROUNDER == 1
    // force the starting X and Y to be even (round down)
    area->x1 &= ~1;
    area->y1 &= ~1;

    // force the ending X and Y to be odd
    // (an even start + an odd end = an even width/height)
    area->x2 |= 1;
    area->y2 |= 1;
#else
#error "LGFX_AMOLED_AMOLED requires implementation!"
#endif
}
#endif

#if LV_USE_GESTURE_RECOGNITION && !defined(CUSTOM_TOUCH_DRIVER)
#ifdef DEBUG_TOUCH_GESTURE
#include "src/indev/lv_indev_gesture_private.h"
#endif
// Multi-touch read for LVGL's gesture recognizers; needs a touch driver reporting stable point ids.
template <class LGFX> void LGFXDriver<LGFX>::touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
    constexpr uint8_t maxPoints = 2; // LVGL recognizers track two contacts
    static lgfx::touch_point_t prev[maxPoints];
    static uint8_t prevCount = 0;
    static bool multiTouch = false;
    static lv_point_t lastPoint = {0, 0};
    static lv_point_t multiBase = {0, 0};    // pointer position when the second finger landed
    static lv_point_t centroidBase = {0, 0}; // finger centroid at that moment

    lgfx::touch_point_t tp[maxPoints];
    uint8_t count;
    uint8_t lastCount = prevCount;
    {
        ISpiLock::Guard bus;
        count = lgfx->getTouch(tp, maxPoints);
    }

    // recognizers count fingers by id, so a lifted finger must be reported once as released
    lv_indev_touch_data_t events[maxPoints * 2];
    uint16_t n = 0;
    uint32_t now = lv_tick_get();
    for (uint8_t i = 0; i < prevCount; i++) {
        bool lifted = true;
        for (uint8_t j = 0; j < count; j++) {
            if (tp[j].id == prev[i].id)
                lifted = false;
        }
        if (lifted)
            events[n++] = {{prev[i].x, prev[i].y}, LV_INDEV_STATE_RELEASED, (uint8_t)prev[i].id, now};
    }
    for (uint8_t j = 0; j < count; j++) {
        events[n++] = {{tp[j].x, tp[j].y}, LV_INDEV_STATE_PRESSED, (uint8_t)tp[j].id, now};
        prev[j] = tp[j];
    }
    prevCount = count;

    lv_indev_gesture_recognizers_update(indev_driver, events, n);
    lv_indev_gesture_recognizers_set_data(indev_driver, data);

    if (count == 0)
        data->point = lastPoint;
    // while two fingers are down the pointer follows their centroid from where it was, so a pinch can also pan
    // without a jump; once a finger lifts it holds still and stays released until all fingers are up
    if (count >= 2) {
        lv_point_t centroid = {(tp[0].x + tp[1].x) / 2, (tp[0].y + tp[1].y) / 2};
        if (!multiTouch || lastCount < 2) {
            multiBase = lastCount ? lastPoint : centroid;
            centroidBase = centroid;
            multiTouch = true;
        }
        data->point = {multiBase.x + centroid.x - centroidBase.x, multiBase.y + centroid.y - centroidBase.y};
    } else if (multiTouch) {
        data->point = lastPoint;
        data->state = LV_INDEV_STATE_RELEASED;
        if (count == 0)
            multiTouch = false;
    }
    lastPoint = data->point;

#ifdef DEBUG_TOUCH_GESTURE
    if (count || lastCount) {
        const auto &p = indev_driver->recognizers[LV_INDEV_GESTURE_PINCH];
        const auto &r = indev_driver->recognizers[LV_INDEV_GESTURE_ROTATE];
        const auto &s = indev_driver->recognizers[LV_INDEV_GESTURE_TWO_FINGERS_SWIPE];
        ILOG_DEBUG("touch n=%u ev=%u p1(%u)=%d/%d p2(%u)=%d/%d | pinch st=%d fc=%u scale=%.2f | rot st=%d rad=%.2f | "
                   "swipe st=%d dx=%.0f dy=%.0f | out %s %d/%d",
                   count, n, count > 0 ? tp[0].id : 0, count > 0 ? tp[0].x : -1, count > 0 ? tp[0].y : -1,
                   count > 1 ? tp[1].id : 0, count > 1 ? tp[1].x : -1, count > 1 ? tp[1].y : -1, p.state,
                   p.info ? p.info->finger_cnt : 0, p.info ? p.info->scale : 0.0f, r.state,
                   r.info ? r.info->rotation - r.info->p_rotation : 0.0f, s.state, s.info ? s.info->delta_x : 0.0f,
                   s.info ? s.info->delta_y : 0.0f, data->state == LV_INDEV_STATE_PRESSED ? "PR" : "REL", data->point.x,
                   data->point.y);
    }
#endif
}
#else
template <class LGFX> void LGFXDriver<LGFX>::touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX = 0, touchY = 0;
#ifdef CUSTOM_TOUCH_DRIVER
    bool touched = lgfx->getTouchXY(&touchX, &touchY); // I2C, no bus guard needed
#else
    // XPT2046/STMPE610 sit on SPI, and on several boards on the panel's own host.
    bool touched;
    {
        ISpiLock::Guard bus;
        touched = lgfx->getTouch(&touchX, &touchY);
    }
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
#endif

template <class LGFX> void LGFXDriver<LGFX>::init(DeviceGUI *gui)
{
    ILOG_DEBUG("LGFXDriver<LGFX>::init...");
    init_lgfx();
    TFTDriver<LGFX>::init(gui);

    // LVGL: setup display device driver
    ILOG_DEBUG("LVGL display driver init...");

    DisplayDriver::display = lv_display_create(DisplayDriver::screenWidth, DisplayDriver::screenHeight);
    lv_display_set_driver_data(this->display, this);
#ifndef LV_COLOR_FORMAT_NO_RGB_SWAP
    lv_display_set_color_format(this->display, LV_COLOR_FORMAT_RGB565_SWAPPED);
#else
    lv_display_set_color_format(this->display, LV_COLOR_FORMAT_RGB565);
#endif
#if defined(USE_DOUBLE_BUFFER) // speedup drawing by using heap-based double-buffered DMA mode
    bufsize = lgfx->screenWidth * LGFX_BUFFER_LINES * sizeof(lv_color_t);
    ILOG_DEBUG("LVGL: allocating %u bytes DRAM memory for double buffering (%d lines)", bufsize * 2, LGFX_BUFFER_LINES);
    buf1 = (lv_color_t *)heap_caps_aligned_alloc(64, bufsize, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    buf2 = (lv_color_t *)heap_caps_aligned_alloc(64, bufsize, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    if (buf1 == nullptr || buf2 == nullptr) {
        ILOG_CRIT("LVGL: failed to allocate DMA buffers (%u bytes each, internal SRAM free: %u)", bufsize,
                  heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA));
        abort();
    }
    lv_display_set_buffers(this->display, buf1, buf2, bufsize, LV_DISPLAY_RENDER_MODE_PARTIAL);
#elif defined(USE_FULL_DOUBLE_BUFFER) // speedup drawing by using PSRAM based double-buffered P4 PPA mode
    bufsize = lgfx->screenWidth * lgfx->screenHeight * sizeof(lv_color_t);
    buf1 = static_cast<lv_color_t *>(lgfx->getFrameBuffer(1));
    buf2 = static_cast<lv_color_t *>(lgfx->getFrameBuffer(0));
    if (buf1 == nullptr || buf2 == nullptr) {
        ILOG_CRIT("LVGL: failed to acquire RGB frame buffers");
        abort();
    }
    ILOG_DEBUG("LVGL: using two %u-byte RGB frame buffers", bufsize);
    lv_display_set_buffers(this->display, buf1, buf2, bufsize, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_wait_cb(this->display, LGFXDriver::display_flush_wait);

#elif defined(BOARD_HAS_PSRAM)
    assert(ESP.getFreePsram());
#ifdef LGFX_BUFSIZE
    bufsize = LGFX_BUFSIZE;
#else
    bufsize = lgfx->screenWidth * lgfx->screenHeight * sizeof(lv_color_t) / 4;
#endif
    ILOG_DEBUG("LVGL: allocating %u bytes PSRAM for draw buffer (max free PSRAM: %u)", bufsize, ESP.getMaxAllocPsram());
    buf1 = (lv_color_t *)LV_MEM_POOL_ALLOC(bufsize);
    assert(buf1 != 0);
    lv_display_set_buffers(this->display, buf1, buf2, bufsize, LV_DISPLAY_RENDER_MODE_PARTIAL);
#else
#ifdef LGFX_BUFSIZE
    bufsize = LGFX_BUFSIZE;
#else
    bufsize = lgfx->screenWidth * lgfx->screenHeight / 8;
#endif
    ILOG_DEBUG("LVGL: allocating %u bytes heap memory for draw buffer", sizeof(lv_color_t) * bufsize);
    buf1 = new lv_color_t[bufsize];
    assert(buf1 != 0);
    lv_display_set_buffers(this->display, buf1, buf2, sizeof(lv_color_t) * bufsize, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

    lv_display_set_flush_cb(this->display, LGFXDriver::display_flush);
#ifdef LGFX_AMOLED_ROUNDER
    lv_display_add_event_cb(this->display, rounder_cb, LV_EVENT_INVALIDATE_AREA, this->display);
#endif

#if defined(DISPLAY_SET_RESOLUTION)
    ILOG_DEBUG("Set display resolution: %dx%d", lgfx->screenWidth, lgfx->screenHeight);
    lv_display_set_resolution(this->display, lgfx->screenWidth, lgfx->screenHeight);
#endif
    // lv_display_set_physical_resolution(this->display, this->screenWidth, this->screenHeight);
    // lv_display_set_rotation(this->display, LV_DISPLAY_ROTATION_90);

    if (hasTouch()) {
        DisplayDriver::touch = lv_indev_create();
        lv_indev_set_scroll_limit(DisplayDriver::touch, defaultGestureLimit);
        lv_indev_set_type(DisplayDriver::touch, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(DisplayDriver::touch, touchpad_read);
        lv_indev_set_display(DisplayDriver::touch, this->display);
        lv_indev_set_long_press_time(DisplayDriver::touch, defaultLongPressTime);
#if LV_USE_GESTURE_RECOGNITION
        lv_indev_set_pinch_up_threshold(DisplayDriver::touch, PINCH_ZOOM_STEP);
        lv_indev_set_pinch_down_threshold(DisplayDriver::touch, 1.0f / PINCH_ZOOM_STEP);
        // LVGL 9.3 defaults the rotate threshold to 0, so rotation wins instantly and locks out pinch
        lv_indev_set_rotation_rad_threshold(DisplayDriver::touch, 0.8f);
        // a recognized two-finger swipe would lock out pinch; panning uses the centroid pointer instead
        DisplayDriver::touch->recognizers[LV_INDEV_GESTURE_TWO_FINGERS_SWIPE].recog_fn = nullptr;
#endif
#ifdef USE_TOUCH_EVENTS
        if (lgfx->touch()->config()->pin_int > 0) {
            lv_indev_set_mode(DisplayDriver::touch, LV_INDEV_MODE_EVENT);
        }
#else
        lv_timer_t *timer = lv_indev_get_read_timer(DisplayDriver::touch);
        if (timer) {
            lv_timer_set_period(timer, defaultTouchReadPeriodMs);
        }
#endif
    }
}

template <class LGFX> void LGFXDriver<LGFX>::init_lgfx(void)
{
    // Initialize LovyanGFX
    ILOG_DEBUG("LGFX init...");
    {
        ISpiLock::Guard bus;
        lgfx->init();
        lgfx->setBrightness(defaultBrightness);
        lgfx->fillScreen(LGFX::color565(0x3D, 0xDA, 0x83));
    }

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
#elif defined(ESP32_NM_CYD_C5)
        uint16_t parameters[8] = {3787, 226, 3857, 3833, 309, 238, 309, 3803};
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
        // calibrateTouch() blocks until the user has tapped every marker, and the bus
        // stays held throughout. Deliberate: LovyanGFX runs its own draw/read loop with
        // no hook to release in the middle, and dropping the guard would leave this the
        // one unprotected SPI path now that the host no longer holds a coarse lock. It
        // is also not a regression - the host used to hold that lock across this same
        // call. Note the branch is only reached when a board ships no stored calibration
        // parameters; every board in-tree supplies them, so in practice we take the
        // non-blocking setTouchCalibrate() path above.
        ISpiLock::Guard bus;
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
        uint32_t id;
        {
            ISpiLock::Guard bus; // readCommand is a real panel transaction
            id = p->readCommand(0x04, 0, 4);
        }
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
