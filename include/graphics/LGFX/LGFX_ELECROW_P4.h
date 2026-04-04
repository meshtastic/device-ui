
#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#if !defined (CONFIG_IDF_TARGET_ESP32P4)
  #error "CONFIG_IDF_TARGET_ESP32P4 should be set"
#endif

#include "lgfx/v1/platforms/esp32p4/Bus_DSI.hpp"
#include "lgfx/v1/platforms/esp32p4/Panel_DSI.hpp"
#include "Wire.h"

#define STC8_I2C_SLAVE_DEV_ADDR 0x2F
#define STC8_REG_ADDR_SET_PWM   0x20

namespace lgfx {

class Elecrow_P4_Light : public ILight
{
  public:
    struct config_t {
        uint8_t brightness = 153; // 60%
    };

    const config_t &config(void) const { return _cfg; }
    void config(const config_t &cfg) { _cfg = cfg; }

    bool init(uint8_t brightness) override {
        Wire1.begin(I2C_SDA1, I2C_SCL1);
        setBrightness(brightness);
        return true;
    }

    void setBrightness(uint8_t brightness) override {
        sendI2CCommand(STC8_REG_ADDR_SET_PWM, brightness);
        _cfg.brightness = brightness;
    }

    uint8_t getBrightness(void) const { return _cfg.brightness; }

    virtual ~Elecrow_P4_Light(void) = default;

  private:

    uint8_t sendI2CCommand(uint8_t cmd, uint8_t data)
    {
        Wire1.beginTransmission(STC8_I2C_SLAVE_DEV_ADDR);
        Wire1.write(cmd);
        Wire1.write(data);
        uint8_t error = Wire.endTransmission();
        if (error != 0)
            ILOG_ERROR("Elecrow_P4_Light: failed to send command 0x%02x: %d", (int)cmd, (int)error);
        return error;
    }

    config_t _cfg;
};

}


class LGFX_ELECROW_P4 : public lgfx::LGFX_Device
{
    lgfx::Bus_DSI          _bus_instance;
    lgfx::Elecrow_P4_Light _light_instance;
    lgfx::Panel_DSI        _panel_instance;
    lgfx::Touch_GT911      _touch_instance;

  public:
    const uint32_t screenWidth = 800;
    const uint32_t screenHeight = 480;
  
    bool hasButton(void) { return false; }
  
    bool init_impl(bool use_reset, bool use_clear) override
    {
#if !CONFIG_SPIRAM
        ESP_LOGE("LGFX", "ELECROW_P4 need PSRAM enabled");
        if (false)
#elif CONFIG_SPIRAM_SPEED <= 80
        ESP_LOGE("LGFX", "ELECROW_P4 need PSRAM SPEED 200MHz");
#if defined (ESP_ARDUINO_VERSION)
#if ESP_ARDUINO_VERSION == ESP_ARDUINO_VERSION_VAL(3,3,0)
#warning "The Arduino-ESP32 v3.3.0 has a problem that PSRAM does not work at 200MHz. Please use v3.3.1 or later or v3.2.x."
#endif
#endif
#endif
//      if (bus_dsi->init()) {
//        lgfx::delay(80);
//      }
      return lgfx::LGFX_Device::init_impl(use_reset, use_clear);
    }

    LGFX_ELECROW_P4(void) {
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
            cfg.dpi_freq_mhz = 52; // min 27 max. 54, Typ 29
            cfg.hsync_back_porch = 160; // 
            cfg.hsync_pulse_width = 1; // 1..40
            cfg.hsync_front_porch = 160; // 12..40..212
            cfg.vsync_back_porch = 23; // --> 32???
            cfg.vsync_pulse_width = 1; // 1..3
            cfg.vsync_front_porch = 12; // 1..13..88
            _panel_instance.config_detail(cfg);
        }
    
        {
            auto cfg = _bus_instance.config();
            //cfg.panel = &_panel_instance;
            cfg.bus_id = 0;
            cfg.lane_num = 2;
            cfg.lane_mbps = 900; // 960
            cfg.ldo_chan_id = 3;
            cfg.ldo_voltage_mv = 2500; // 1900?
            _bus_instance.config(cfg);
        }
        _panel_instance.setBus(&_bus_instance);
    
        {
            auto cfg = _touch_instance.config();
            cfg.x_min = 0;
            cfg.y_min = 0;
            cfg.x_max = 800;
            cfg.y_max = 480;
            cfg.pin_int = GPIO_NUM_42;
            cfg.pin_rst = GPIO_NUM_36;
            cfg.bus_shared = false;
            cfg.offset_rotation = 0;
    
            cfg.i2c_port = 1;
            cfg.pin_sda = GPIO_NUM_45;
            cfg.pin_scl = GPIO_NUM_46;
            cfg.freq = 400000;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
    
        // Backlight configuration.
        _panel_instance.light(&_light_instance);
    
        setPanel(&_panel_instance);
    }
};
  
