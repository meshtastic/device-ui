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
    init_lvgl();
    TFTDriver<LVGL>::init(gui);

    // LVGL: setup display device driver
    ILOG_DEBUG("LVGL display driver create...");
    display = lvgldriver->create(DisplayDriver::screenWidth, DisplayDriver::screenHeight);

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

    if (hasTouch()) {
        lv_indev_t *indev = lv_indev_create();
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, touchpad_read);
        lv_indev_set_display(indev, this->display);
    }
}

template <class LGFX> void LVGLDriver<LGFX>::init_lvgl(void)
{
    lvgldriver->init();
    lvgldriver->setBrightness(defaultBrightness);
    // lvgldriver->fillScreen(LGFX::color565(0x3D, 0xDA, 0x83));
}

template <class LGFX> bool LVGLDriver<LGFX>::hasTouch(void)
{
    return lvgldriver->hasTouch();
}

template <class LGFX> void LVGLDriver<LGFX>::task_handler(void)
{
    DisplayDriver::task_handler();
}
#endif