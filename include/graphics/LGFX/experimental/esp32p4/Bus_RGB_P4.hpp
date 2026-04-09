#pragma once

#if defined(ESP_PLATFORM) && __has_include(<esp_lcd_panel_rgb.h>) && (defined(CONFIG_IDF_TARGET_ESP32P4) || defined(__INTELLISENSE__))

#include <cstring>

#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>

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
        panel_config.bits_per_pixel = 16;
        panel_config.num_fbs = 2;
        panel_config.dma_burst_size = 64;
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
        if (ESP_OK != esp_lcd_panel_reset(_panel_handle)) {
            release();
            return false;
        }
        if (ESP_OK != esp_lcd_panel_init(_panel_handle)) {
            release();
            return false;
        }
        void *frame_buffer = nullptr;
        if (ESP_OK != esp_lcd_rgb_panel_get_frame_buffer(_panel_handle, 1, &frame_buffer)) {
            release();
            return false;
        }
        _frame_buffer = static_cast<uint8_t *>(frame_buffer);
        return _frame_buffer != nullptr;
    }

    void release(void) override
    {
        _frame_buffer = nullptr;
        if (_panel_handle != nullptr) {
            esp_lcd_panel_del(_panel_handle);
            _panel_handle = nullptr;
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
    uint8_t *getDMABuffer(uint32_t) override { return _frame_buffer; }

    void beginRead(void) override {}
    void endRead(void) override {}
    uint32_t readData(uint_fast8_t) override { return 0; }
    bool readBytes(uint8_t *, uint32_t, bool) override { return false; }
    void readPixels(void *, pixelcopy_t *, uint32_t) override {}

  private:
    config_t _cfg;
    esp_lcd_panel_handle_t _panel_handle = nullptr;
    uint8_t *_frame_buffer = nullptr;
};

} // namespace v1
} // namespace lgfx

#endif
