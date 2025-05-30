#pragma once

#ifdef LVGL_DRIVER
#include "drivers/display/lcd/lv_lcd_generic_mipi.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "graphics/driver/TFTDriver.h"
#include "input/InputDriver.h"
#include "util/ILog.h"

/**
 * Base class for all specific LVGL drivers classes
 */
template <class LVGL> class LVGLDriver : public TFTDriver<LVGL>
{
  public:
    const uint32_t defaultScreenTimeout = 60 * 1000;
    const uint32_t defaultScreenLockTimeout = 5 * 1000;
    const uint32_t defaultBrightness = 153;

    LVGLDriver(uint16_t width, uint16_t height);
    LVGLDriver(const DisplayDriverConfig &cfg);
    void init(DeviceGUI *gui) override;
    bool hasTouch(void) override;
    bool hasButton(void) override { return lvgldriver->hasButton(); }
    bool hasLight(void) override { return lvgldriver->light(); }
    bool isPowersaving(void) override { return powerSaving; }
    void task_handler(void) override;

    uint8_t getBrightness(void) override { return lvgldriver->getBrightness(); }
    void setBrightness(uint8_t brightness) override {} // TODO

    uint16_t getScreenTimeout() override { return screenTimeout / 1000; }
    void setScreenTimeout(uint16_t timeout) override { screenTimeout = timeout * 1000; };

    ~LVGLDriver() { delete lvgldriver; }

  protected:
    void init_lvgl(void);
    static void display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
    static void touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data)
    {
        return lvgldriver->touchpad_read(indev_driver, data);
    }

    uint32_t screenTimeout;
    uint32_t lastBrightness;
    bool powerSaving;

  private:
    static LVGL *lvgldriver;
    lv_display_t *display;
    size_t bufsize;
    std::pair<lv_color_t *, lv_color_t *> drawBuffer = {nullptr, nullptr};
};

template <class LVGL> LVGL *LVGLDriver<LVGL>::lvgldriver = nullptr;

template <class LVGL>
LVGLDriver<LVGL>::LVGLDriver(uint16_t width, uint16_t height)
    : TFTDriver<LVGL>(lvgldriver ? lvgldriver : new LVGL, width, height), screenTimeout(defaultScreenTimeout),
      lastBrightness(defaultBrightness), powerSaving(false), display(nullptr), bufsize(0)
{
    lvgldriver = this->tft;
}

template <class LVGL>
LVGLDriver<LVGL>::LVGLDriver(const DisplayDriverConfig &cfg)
    : TFTDriver<LVGL>(lvgldriver ? lvgldriver : new LVGL(cfg), cfg.width(), cfg.height()), screenTimeout(defaultScreenTimeout),
      lastBrightness(defaultBrightness), powerSaving(false), display(nullptr), bufsize(0)
{
    lvgldriver = this->tft;
}

template <class LVGL> void LVGLDriver<LVGL>::init(DeviceGUI *gui)
{
    ILOG_DEBUG("LVGLDriver<LVGL>::init...");
    TFTDriver<LVGL>::init(gui);
    init_lvgl();

    // LVGL: setup display device driver
    ILOG_DEBUG("LVGL display driver create...");
    display = lvgldriver->createDisplay(DisplayDriver::screenWidth, DisplayDriver::screenHeight);

    std::pair<lv_color_t *, lv_color_t *> draw_buffers = {nullptr, nullptr};
    const auto buffer_size =
        DisplayDriver::screenWidth * DisplayDriver::screenHeight * lv_color_format_get_size(lv_display_get_color_format(display));

    ILOG_INFO("allocating %d bytes for LVGL draw buffers", buffer_size);
    // ESP_ERROR_CHECK(esp_dma_malloc(EXAMPLE_LCD_H_RES * 100 * sizeof(lv_color_t), ESP_DMA_MALLOC_FLAG_PSRAM, draw_buffers.first,
    // nullptr)); ESP_ERROR_CHECK(esp_dma_malloc(EXAMPLE_LCD_H_RES * 100 * sizeof(lv_color_t), ESP_DMA_MALLOC_FLAG_PSRAM,
    // draw_buffers.second, nullptr));

    draw_buffers.first = (lv_color_t *)lv_malloc(buffer_size);
    if (draw_buffers.first == nullptr) {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }

    draw_buffers.second = (lv_color_t *)lv_malloc(buffer_size);
    if (draw_buffers.second == nullptr) {
        LV_LOG_ERROR("display buffer malloc failed");
        lv_free(draw_buffers.first);
        return;
    }
    lv_display_set_buffers(display, draw_buffers.first, draw_buffers.second, buffer_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(display, LVGLDriver::display_flush);

    #if defined(DISPLAY_SET_RESOLUTION)
        ILOG_DEBUG("Set display resolution: %dx%d", lvgldriver->screenWidth, lgflvgldriverx->screenHeight);
        lv_display_set_resolution(this->display, lvgldriver->screenWidth, lvgldriver->screenHeight);
    #endif
    
    if (hasTouch()) {
        lv_indev_t *indev = lv_indev_create();
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, touchpad_read);
        lv_indev_set_display(indev, this->display);
    }
}

template <class LVGL> void LVGLDriver<LVGL>::init_lvgl(void)
{
    lvgldriver->init();
    lvgldriver->setBrightness(defaultBrightness);
    // lvgldriver->fillScreen(LVGL::color565(0x3D, 0xDA, 0x83));
}

template <class LVGL> bool LVGLDriver<LVGL>::hasTouch(void)
{
    return lvgldriver->hasTouch();
}

template <class LVGL> void LVGLDriver<LVGL>::task_handler(void)
{
    DisplayDriver::task_handler();
}

// Display flushing not using DMA */
template <class LVGL> void LVGLDriver<LVGL>::display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    lv_draw_sw_rgb565_swap(px_map, w * h);
#if 0 // TODO: //// lvgldriver->pushImage(area->x1, area->y1, w, h, (uint16_t *)px_map) ////

    pixelcopy_t pc(px_map, _write_conv.depth, get_depth<T>::value, hasPalette());
    if (std::is_same<rgb565_t, T>::value || std::is_same<rgb888_t, T>::value || std::is_same<argb8888_t, T>::value || std::is_same<grayscale_t, T>::value)
    {
      pc.no_convert = false;
      pc.fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<T>(_write_conv.depth);
    }
    pixelcopy_t *param = &pc;
    uint32_t x_mask = 7 >> (param->src_bits >> 1);
    param->src_bitwidth = (w + x_mask) & (~x_mask);

    int32_t dx=0, dw=w;
    if (0 < _clip_l - x) { dx = _clip_l - x; dw -= dx; x = _clip_l; }

    if (_adjust_width(x, dx, dw, _clip_l, _clip_r - _clip_l + 1)) return;
    param->src_x32 = param->src_x32_add * dx;

    int32_t dy=0, dh=h;
    if (0 < _clip_t - y) { dy = _clip_t - y; dh -= dy; y = _clip_t; }
    if (_adjust_width(y, dy, dh, _clip_t, _clip_b - _clip_t + 1)) return;
    param->src_y = dy;

    startWrite();
    _panel->writeImage(x, y, dw, dh, param, use_dma);
    endWrite();
#endif

    lv_display_flush_ready(disp);
}

#endif