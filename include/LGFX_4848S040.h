
#pragma once
#define LGFX_USE_V1

#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>

#ifdef CUSTOM_TOUCH_DRIVER
#include <bb_captouch.h>

#define TOUCH_SDA 19
#define TOUCH_SCL 45
#define TOUCH_INT -1
#define TOUCH_RST -1

// avoid lovyanGFX touch driver: 
// custom class for redirecting getTouch() calls and to alternative implementation
class LGFX_Touch : public lgfx::LGFX_Device
{
  public:
    bool init_impl(bool use_reset, bool use_clear) override
    {
        bool result = LGFX_Device::init_impl(use_reset, use_clear);
        bbct.init(TOUCH_SDA, TOUCH_SCL);
        bbct.setOrientation(0, 480, 480);
        return result;
    }

    int8_t getTouchInt(void) { return TOUCH_INT; }

    // unfortunately not declared as virtual in base class, need to choose a different name
    bool getTouchXY(uint16_t* touchX, uint16_t* touchY) {
        TOUCHINFO ti;
        if (bbct.getSamples(&ti)) {
            *touchX = ti.x[0];
            *touchY = ti.y[0];
            if (*touchX < 480 && *touchY < 480)
                return true;
        }
        return false; 
    };

  private:
    BBCapTouch bbct;
};

class LGFX_4848S040 : public LGFX_Touch
#else
class LGFX_4848S040 : public lgfx::LGFX_Device
#endif
{
    lgfx::Panel_ST7701_guition_esp32_4848S040 _panel_instance;
    lgfx::Bus_RGB _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_GT911 _touch_instance;

  public:
    const uint16_t screenWidth = 480;
    const uint16_t screenHeight = 480;

    bool hasButton(void) { return true; }

    LGFX_4848S040(void)
    {
        {
            auto cfg = _panel_instance.config();
            cfg.memory_width = 480;
            cfg.memory_height = 480;
            cfg.panel_width = screenWidth;
            cfg.panel_height = screenHeight;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            _panel_instance.config(cfg);
        }

        {
            auto cfg = _panel_instance.config_detail();
            cfg.pin_cs = 39;
            cfg.pin_sclk = 48;
            cfg.pin_mosi = 47;
            cfg.use_psram = 1;
            _panel_instance.config_detail(cfg);
        }

        {
            auto cfg = _bus_instance.config();
            cfg.panel = &_panel_instance;

            cfg.pin_d0 = GPIO_NUM_4;   // DB1(B)
            cfg.pin_d1 = GPIO_NUM_5;   // DB2(B)
            cfg.pin_d2 = GPIO_NUM_6;   // DB3(B)
            cfg.pin_d3 = GPIO_NUM_7;   // DB4(B)
            cfg.pin_d4 = GPIO_NUM_15;  // DB5(B)

            cfg.pin_d5 = GPIO_NUM_8;   // DB6(G)
            cfg.pin_d6 = GPIO_NUM_20;  // DB7(G)
            cfg.pin_d7 = GPIO_NUM_3;   // DB8(G)
            cfg.pin_d8 = GPIO_NUM_46;  // DB9(G)
            cfg.pin_d9 = GPIO_NUM_9;   // DB10(G)
            cfg.pin_d10 = GPIO_NUM_10; // DB11(G)

            cfg.pin_d11 = GPIO_NUM_11; // DB13(R)
            cfg.pin_d12 = GPIO_NUM_12; // DB14(R)
            cfg.pin_d13 = GPIO_NUM_13; // DB15(R)
            cfg.pin_d14 = GPIO_NUM_14; // DB16(R)
            cfg.pin_d15 = GPIO_NUM_0;  // DB17(R)

            cfg.pin_henable = GPIO_NUM_18;
            cfg.pin_vsync = GPIO_NUM_17;
            cfg.pin_hsync = GPIO_NUM_16;
            cfg.pin_pclk = GPIO_NUM_21;
            cfg.freq_write = 12000000;

            cfg.hsync_polarity = 1;
            cfg.hsync_front_porch = 10;
            cfg.hsync_pulse_width = 8;
            cfg.hsync_back_porch = 50;

            cfg.vsync_polarity = 1;
            cfg.vsync_front_porch = 10;
            cfg.vsync_pulse_width = 8;
            cfg.vsync_back_porch = 20;

            cfg.pclk_active_neg = 1;
            cfg.de_idle_high = 0;
            cfg.pclk_idle_high = 0;

            _bus_instance.config(cfg);
        }
        _panel_instance.setBus(&_bus_instance);

        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = 38;
            cfg.freq = 2000;
            _light_instance.config(cfg);
        }
        _panel_instance.light(&_light_instance);

        {
            auto cfg = _touch_instance.config();
            cfg.pin_cs = GPIO_NUM_NC;
            cfg.x_min = 0;
            cfg.x_max = 479;
            cfg.y_min = 0;
            cfg.y_max = 479;
            cfg.pin_int = GPIO_NUM_NC;
            cfg.pin_rst = GPIO_NUM_NC;
            cfg.bus_shared = false;
            cfg.offset_rotation = 0;

            cfg.i2c_port = 1;
            cfg.i2c_addr = 0x5D;
            cfg.pin_sda = 19;
            cfg.pin_scl = 45;
            cfg.freq = 400000;
            _touch_instance.config(cfg);
#ifndef CUSTOM_TOUCH_DRIVER
            _panel_instance.setTouch(&_touch_instance);
#endif
        }

        setPanel(&_panel_instance);
    }
};
