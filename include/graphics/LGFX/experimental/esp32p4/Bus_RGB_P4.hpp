#pragma once

#if defined(ESP_PLATFORM) && __has_include(<esp_lcd_panel_rgb.h>) && (defined(CONFIG_IDF_TARGET_ESP32P4) || defined(__INTELLISENSE__))

#include <cstring>

#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "lgfx/v1/Bus.hpp"
#include "lgfx/v1/panel/Panel_FrameBufferBase.hpp"

namespace lgfx
{
inline namespace v1
{

class Bus_RGB_P4 : public IBus
{
  public:
    struct config_t {
        Panel_FrameBufferBase *panel = nullptr;

        int8_t port = 0;
        uint32_t freq_write = 16000000;
        uint16_t panel_width = 800;
        uint16_t panel_height = 480;

        int8_t pin_pclk = -1;
        int8_t pin_vsync = -1;
        int8_t pin_hsync = -1;
        int8_t pin_henable = -1;

        union {
            int8_t pin_data[16];
            struct {
                int8_t pin_d0;
                int8_t pin_d1;
                int8_t pin_d2;
                int8_t pin_d3;
                int8_t pin_d4;
                int8_t pin_d5;
                int8_t pin_d6;
                int8_t pin_d7;
                int8_t pin_d8;
                int8_t pin_d9;
                int8_t pin_d10;
                int8_t pin_d11;
                int8_t pin_d12;
                int8_t pin_d13;
                int8_t pin_d14;
                int8_t pin_d15;
            };
        };

        uint16_t hsync_pulse_width = 4;
        uint16_t hsync_back_porch = 8;
        uint16_t hsync_front_porch = 8;
        uint16_t vsync_pulse_width = 4;
        uint16_t vsync_back_porch = 16;
        uint16_t vsync_front_porch = 16;

        bool hsync_polarity = false;
        bool vsync_polarity = false;
        bool pclk_active_neg = true;
        bool de_idle_high = false;
        bool pclk_idle_high = true;
    };

    const config_t &config(void) const { return _cfg; }
    void config(const config_t &config) { _cfg = config; }

    bus_type_t busType(void) const override { return bus_type_t::bus_unknown; }

    bool init(void) override
    {
        if (_panel_handle != nullptr) {
            return true;
        }
        if (_cfg.panel == nullptr || _cfg.panel_width == 0 || _cfg.panel_height == 0) {
            return false;
        }

        esp_lcd_rgb_panel_config_t panel_config = {};
        panel_config.clk_src = LCD_CLK_SRC_DEFAULT;
        panel_config.timings.pclk_hz = _cfg.freq_write;
        panel_config.timings.h_res = _cfg.panel_width;
        panel_config.timings.v_res = _cfg.panel_height;
        panel_config.timings.hsync_pulse_width = _cfg.hsync_pulse_width;
        panel_config.timings.hsync_back_porch = _cfg.hsync_back_porch;
        panel_config.timings.hsync_front_porch = _cfg.hsync_front_porch;
        panel_config.timings.vsync_pulse_width = _cfg.vsync_pulse_width;
        panel_config.timings.vsync_back_porch = _cfg.vsync_back_porch;
        panel_config.timings.vsync_front_porch = _cfg.vsync_front_porch;
        panel_config.timings.flags.hsync_idle_low = _cfg.hsync_polarity;
        panel_config.timings.flags.vsync_idle_low = _cfg.vsync_polarity;
        panel_config.timings.flags.de_idle_high = _cfg.de_idle_high;
        panel_config.timings.flags.pclk_active_neg = _cfg.pclk_active_neg;
        panel_config.timings.flags.pclk_idle_high = _cfg.pclk_idle_high;
        panel_config.data_width = 16;
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(6, 0, 0)
        panel_config.bits_per_pixel = 16;
#else
        panel_config.in_color_format = LCD_COLOR_FMT_RGB565,  // format stored in the frame buffer
        panel_config.out_color_format = LCD_COLOR_FMT_RGB565, // format expected by the physical LCD screen
#endif
        panel_config.num_fbs = 2;
        panel_config.dma_burst_size = 64;
        panel_config.bounce_buffer_size_px = _cfg.panel_width * 10;
        panel_config.hsync_gpio_num = _cfg.pin_hsync;
        panel_config.vsync_gpio_num = _cfg.pin_vsync;
        panel_config.de_gpio_num = _cfg.pin_henable;
        panel_config.pclk_gpio_num = _cfg.pin_pclk;
        panel_config.disp_gpio_num = -1;
        panel_config.flags.fb_in_psram = true;
        for (int i = 0; i < 16; ++i) {
            panel_config.data_gpio_nums[i] = _cfg.pin_data[i];
        }

        if (ESP_OK != esp_lcd_new_rgb_panel(&panel_config, &_panel_handle)) {
            _panel_handle = nullptr;
            return false;
        }
        _frame_complete = xSemaphoreCreateBinary();
        if (_frame_complete == nullptr) {
            release();
            return false;
        }
        esp_lcd_rgb_panel_event_callbacks_t callbacks = {};
        callbacks.on_frame_buf_complete = frameBufferComplete;
        if (ESP_OK != esp_lcd_rgb_panel_register_event_callbacks(_panel_handle, &callbacks, this)) {
            release();
            return false;
        }
        if (ESP_OK != esp_lcd_panel_reset(_panel_handle)) {
            release();
            return false;
        }
        if (ESP_OK != esp_lcd_panel_init(_panel_handle)) {
            release();
            return false;
        }
        if (ESP_OK != esp_lcd_rgb_panel_get_frame_buffer(_panel_handle, 2, &_frame_buffers[0], &_frame_buffers[1])) {
            release();
            return false;
        }
        return _frame_buffers[0] != nullptr && _frame_buffers[1] != nullptr;
    }

    void release(void) override
    {
        _frame_buffers[0] = nullptr;
        _frame_buffers[1] = nullptr;
        if (_panel_handle != nullptr) {
            esp_lcd_panel_del(_panel_handle);
            _panel_handle = nullptr;
        }
        if (_frame_complete != nullptr) {
            vSemaphoreDelete(_frame_complete);
            _frame_complete = nullptr;
        }
    }

    void beginTransaction(void) override {}
    void endTransaction(void) override {}
    void wait(void) override {}
    bool busy(void) const override { return false; }

    void flush(void) override {}
    bool writeCommand(uint32_t, uint_fast8_t) override { return true; }
    void writeData(uint32_t, uint_fast8_t) override {}
    void writeDataRepeat(uint32_t, uint_fast8_t, uint32_t) override {}
    void writePixels(pixelcopy_t *, uint32_t) override {}
    void writeBytes(const uint8_t *, uint32_t, bool, bool) override {}

    void initDMA(void) override {}
    void addDMAQueue(const uint8_t *, uint32_t) override {}
    void execDMAQueue(void) override {}
    uint8_t *getDMABuffer(uint32_t) override { return static_cast<uint8_t *>(_frame_buffers[0]); }

    void *getFrameBuffer(uint8_t index) const { return index < 2 ? _frame_buffers[index] : nullptr; }

    bool presentFrameBuffer(const void *frame_buffer)
    {
        xSemaphoreTake(_frame_complete, 0);
        _frame_pending = true;
        if (ESP_OK != esp_lcd_panel_draw_bitmap(_panel_handle, 0, 0, _cfg.panel_width, _cfg.panel_height, frame_buffer)) {
            _frame_pending = false;
            return false;
        }
        return true;
    }

    void waitFrameBuffer()
    {
        if (_frame_pending) {
            xSemaphoreTake(_frame_complete, portMAX_DELAY);
            _frame_pending = false;
        }
    }

    void beginRead(void) override {}
    void endRead(void) override {}
    uint32_t readData(uint_fast8_t) override { return 0; }
    bool readBytes(uint8_t *, uint32_t, bool) override { return false; }
    void readPixels(void *, pixelcopy_t *, uint32_t) override {}

  private:
    static bool IRAM_ATTR frameBufferComplete(esp_lcd_panel_handle_t, const esp_lcd_rgb_panel_event_data_t *, void *user_ctx)
    {
        auto bus = static_cast<Bus_RGB_P4 *>(user_ctx);
        BaseType_t task_woken = pdFALSE;
        if (bus->_frame_pending) {
            xSemaphoreGiveFromISR(bus->_frame_complete, &task_woken);
        }
        return task_woken == pdTRUE;
    }

    config_t _cfg;
    esp_lcd_panel_handle_t _panel_handle = nullptr;
    void *_frame_buffers[2] = {};
    SemaphoreHandle_t _frame_complete = nullptr;
    volatile bool _frame_pending = false;
};

} // namespace v1
} // namespace lgfx

#endif
