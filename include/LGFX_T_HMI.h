#pragma once
#define LGFX_USE_V1

#include <LovyanGFX.hpp>

class LGFX_T_HMI : public lgfx::LGFX_Device
{

    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_Parallel8 _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_XPT2046 _touch_instance;

  public:
    const uint16_t screenWidth = 320;
    const uint16_t screenHeight = 240;

    bool hasButton(void) { return true; }

    LGFX_T_HMI(void)
    {
        {
            auto cfg = _bus_instance.config();
            cfg.freq_write = 20000000;
#if CONFIG_IDF_TARGET_ESP32S3
            cfg.freq_read = 16000000;
#endif
            cfg.pin_rd = -1;
            cfg.pin_wr = 8;
            cfg.pin_rs = 7;
            cfg.pin_d0 = 48;
            cfg.pin_d1 = 47;
            cfg.pin_d2 = 39;
            cfg.pin_d3 = 40;
            cfg.pin_d4 = 41;
            cfg.pin_d5 = 42;
            cfg.pin_d6 = 45;
            cfg.pin_d7 = 46;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {
            auto cfg = _panel_instance.config();

            cfg.pin_cs = 6;
            cfg.pin_rst = -1;
            cfg.pin_busy = -1;

            cfg.panel_width = screenHeight;
            cfg.panel_height = screenWidth;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 1;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = false;

            _panel_instance.config(cfg);
        }

        {
            auto cfg = _light_instance.config();

            cfg.pin_bl = 38;
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        {
            auto cfg = _touch_instance.config();

            cfg.x_min = 300;
            cfg.x_max = 3900;
            cfg.y_min = 400;
            cfg.y_max = 3900;
            cfg.pin_int = 9;
            cfg.pin_cs = 2;
            cfg.pin_sclk = 1;
            cfg.pin_miso = 4;
            cfg.pin_mosi = 3;
            cfg.spi_host = SPI3_HOST;
            cfg.bus_shared = false;
            cfg.offset_rotation = 0;
            cfg.freq = 2500000;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance);
    }
};
