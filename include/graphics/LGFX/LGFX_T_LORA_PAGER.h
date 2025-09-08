#pragma once
#define LGFX_USE_V1

#include <LovyanGFX.hpp>

#ifndef SPI_FREQUENCY
#define SPI_FREQUENCY 75000000
#endif

class LGFX_TLORA_PAGER : public lgfx::LGFX_Device
{

    lgfx::Panel_ST7796 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;

  public:
    const uint16_t screenWidth = 480;
    const uint16_t screenHeight = 222;

    bool hasButton(void) { return true; }

    LGFX_TLORA_PAGER(void)
    {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = SPI_FREQUENCY;
            cfg.freq_read = 16000000;
            cfg.spi_3wire = false;
            cfg.use_lock = false;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk = 35;
            cfg.pin_mosi = 34;
            cfg.pin_miso = 33;
            cfg.pin_dc = 37;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {
            auto cfg = _panel_instance.config();

            cfg.pin_cs = 38;
            cfg.pin_rst = -1;
            cfg.pin_busy = -1;

            cfg.panel_width = screenHeight;
            cfg.panel_height = screenWidth;
            cfg.offset_x = 49;
            cfg.offset_y = 0;
            cfg.offset_rotation = 3;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = true;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = false;

            _panel_instance.config(cfg);
        }

        {
            auto cfg = _light_instance.config();

            cfg.pin_bl = 42;
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        setPanel(&_panel_instance);
    }
};
