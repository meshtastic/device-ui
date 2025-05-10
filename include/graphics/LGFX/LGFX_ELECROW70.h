#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <TCA9534.h>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>

#ifndef FREQ_WRITE
#define FREQ_WRITE 14000000
#endif

class LGFX_ELECROW70 : public lgfx::LGFX_Device
{
    lgfx::Bus_RGB _bus_instance;
    lgfx::Panel_RGB _panel_instance;
    lgfx::Touch_GT911 _touch_instance;

  public:
    const uint16_t screenWidth = 800;
    const uint16_t screenHeight = 480;

    bool hasButton(void) { return false; }

    bool init_impl(bool use_reset, bool use_clear) override
    {
        ioex.attach(Wire);
        ioex.setDeviceAddress(0x18);
        ioex.config(1, TCA9534::Config::OUT);
        ioex.config(2, TCA9534::Config::OUT);
        ioex.config(3, TCA9534::Config::OUT);
        ioex.config(4, TCA9534::Config::OUT);

        ioex.output(1, TCA9534::Level::H);
        ioex.output(3, TCA9534::Level::L);
        ioex.output(4, TCA9534::Level::H);

        pinMode(1, OUTPUT);
        digitalWrite(1, LOW);
        ioex.output(2, TCA9534::Level::L);
        delay(20);
        ioex.output(2, TCA9534::Level::H);
        delay(100);
        pinMode(1, INPUT);

        return LGFX_Device::init_impl(use_reset, use_clear);
    }

    LGFX_ELECROW70(void)
    {
        {
            auto cfg = _panel_instance.config();

            cfg.memory_width = screenWidth;
            cfg.memory_height = screenHeight;
            cfg.panel_width = screenWidth;
            cfg.panel_height = screenHeight;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            _panel_instance.config(cfg);
        }

        {
            auto cfg = _panel_instance.config_detail();
            cfg.use_psram = 1;
            _panel_instance.config_detail(cfg);
        }

        {
            auto cfg = _bus_instance.config();
            cfg.panel = &_panel_instance;
            cfg.pin_d0 = GPIO_NUM_21;  // B0
            cfg.pin_d1 = GPIO_NUM_47;  // B1
            cfg.pin_d2 = GPIO_NUM_48;  // B2
            cfg.pin_d3 = GPIO_NUM_45;  // B3
            cfg.pin_d4 = GPIO_NUM_38;  // B4
            cfg.pin_d5 = GPIO_NUM_9;   // G0
            cfg.pin_d6 = GPIO_NUM_10;  // G1
            cfg.pin_d7 = GPIO_NUM_11;  // G2
            cfg.pin_d8 = GPIO_NUM_12;  // G3
            cfg.pin_d9 = GPIO_NUM_13;  // G4
            cfg.pin_d10 = GPIO_NUM_14; // G5
            cfg.pin_d11 = GPIO_NUM_7;  // R0
            cfg.pin_d12 = GPIO_NUM_17; // R1
            cfg.pin_d13 = GPIO_NUM_18; // R2
            cfg.pin_d14 = GPIO_NUM_3;  // R3
            cfg.pin_d15 = GPIO_NUM_46; // R4

            cfg.pin_henable = GPIO_NUM_42;
            cfg.pin_vsync = GPIO_NUM_41;
            cfg.pin_hsync = GPIO_NUM_40;
            cfg.pin_pclk = GPIO_NUM_39;
            cfg.freq_write = FREQ_WRITE;

            cfg.hsync_polarity = 0;
            cfg.hsync_front_porch = 8;
            cfg.hsync_pulse_width = 4;
            cfg.hsync_back_porch = 8;

            cfg.vsync_polarity = 0;
            cfg.vsync_front_porch = 8;
            cfg.vsync_pulse_width = 4;
            cfg.vsync_back_porch = 8;

            cfg.pclk_idle_high = 1;
            // cfg.pclk_active_neg = 0;
            // cfg.pclk_idle_high = 0;
            // cfg.de_idle_high = 1;

            _bus_instance.config(cfg);
        }
        _panel_instance.setBus(&_bus_instance);

        {
            auto cfg = _touch_instance.config();
            cfg.x_min = 0;
            cfg.x_max = 800;
            cfg.y_min = 0;
            cfg.y_max = 480;
            cfg.pin_int = -1;
            cfg.pin_rst = -1;
            cfg.bus_shared = true;
            cfg.offset_rotation = 0;

            cfg.i2c_port = 0;
            cfg.i2c_addr = 0x5D;
            cfg.pin_sda = GPIO_NUM_15;
            cfg.pin_scl = GPIO_NUM_16;
            cfg.freq = 400000;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance);
    }

    void sleep(void)
    {
        ioex.output(1, TCA9534::Level::L);
        _panel->setSleep(true);
    }
    void wakeup(void)
    {
        ioex.output(1, TCA9534::Level::H);
        _panel->setSleep(false);
    }

  private:
    TCA9534 ioex;
};
