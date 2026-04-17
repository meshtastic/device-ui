
#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include <Arduino.h>
#include <esp_log.h>

#if !defined(CONFIG_IDF_TARGET_ESP32P4)
#error "CONFIG_IDF_TARGET_ESP32P4 should be set"
#endif

#include "graphics/LGFX/experimental/esp32p4/Panel_EK79007D.hpp"
#include "lgfx/v1/platforms/esp32p4/Bus_DSI.hpp"
#include "lgfx/v1/platforms/esp32p4/Panel_DSI.hpp"

class LGFX_ELECROW_P4_70_90_101 : public lgfx::LGFX_Device
{
    lgfx::Bus_DSI _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Panel_EK79007D _panel_instance;
    lgfx::Touch_GT911 _touch_instance;

  public:
    const uint32_t screenWidth = 1024;
    const uint32_t screenHeight = 600;

    bool hasButton(void) { return false; }

    bool init_impl(bool use_reset, bool use_clear) override
    {
#if !CONFIG_SPIRAM
        ESP_LOGE("LGFX", "ELECROW_P4 need PSRAM enabled");
        if (false)
#elif CONFIG_SPIRAM_SPEED <= 80
        ESP_LOGE("LGFX", "ELECROW_P4 need PSRAM SPEED 200MHz");
#endif
            if (_bus_instance.init()) {
                // EK79007 power-up can be timing-sensitive; keep a conservative settle time
                lgfx::delay(250);
            }
        return lgfx::LGFX_Device::init_impl(use_reset, use_clear);
    }

    LGFX_ELECROW_P4_70_90_101(void)
    {
        {
            auto cfg = _panel_instance.config();

            cfg.memory_width = screenWidth;
            cfg.memory_height = screenHeight;
            cfg.panel_width = screenWidth;
            cfg.panel_height = screenHeight;
            cfg.readable = true;
            cfg.rgb_order = true;
            cfg.bus_shared = false;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.pin_cs = GPIO_NUM_NC;
            cfg.pin_rst = GPIO_NUM_NC;
            _panel_instance.config(cfg);
        }

        {
            auto cfg = _panel_instance.config_detail();
            cfg.dpi_freq_mhz = 51;
            cfg.hsync_back_porch = 160;
            cfg.hsync_pulse_width = 70;
            cfg.hsync_front_porch = 160;
            cfg.vsync_back_porch = 23;
            cfg.vsync_pulse_width = 10;
            cfg.vsync_front_porch = 12;
            _panel_instance.config_detail(cfg);
        }

        {
            auto cfg = _bus_instance.config();
            // cfg.panel = &_panel_instance;
            cfg.bus_id = 0;
            cfg.lane_num = 2;
            cfg.lane_mbps = 1000;
            cfg.ldo_chan_id = 3;
            cfg.ldo_voltage_mv = 2500; // 1900?
            _bus_instance.config(cfg);
        }
        _panel_instance.setBus(&_bus_instance);

        {
            auto cfg = _touch_instance.config();
            cfg.x_min = 0;
            cfg.y_min = 0;
            cfg.x_max = screenWidth;
            cfg.y_max = screenHeight;
            cfg.pin_int = GPIO_NUM_42;
            cfg.pin_rst = GPIO_NUM_40;
            cfg.bus_shared = false;
            cfg.offset_rotation = 0;

            cfg.i2c_port = 1;
            cfg.pin_sda = GPIO_NUM_45;
            cfg.pin_scl = GPIO_NUM_46;
            cfg.freq = 400000;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = GPIO_NUM_31;
            cfg.invert = false;
            cfg.freq = 30000;
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        setPanel(&_panel_instance);
    }
};
