
#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include <Arduino.h>
#include <esp_log.h>

#if !defined (CONFIG_IDF_TARGET_ESP32P4) && !defined(__INTELLISENSE__)
  #error "CONFIG_IDF_TARGET_ESP32P4 should be set"
#endif

#include "graphics/LGFX/experimental/esp32p4/Bus_RGB_P4.hpp"
#include "graphics/LGFX/experimental/esp32p4/Panel_RGB_P4.hpp"

#define STC8_I2C_SLAVE_DEV_ADDR 0x2F
#define STC8_REG_ADDR_SET_PWM   0x20
#define STC8_I2C_PORT           1
#define STC8_I2C_SDA_PIN        45
#define STC8_I2C_SCL_PIN        46
#define STC8_I2C_FREQ_HZ        400000

namespace lgfx {

class Panel_EK79007 : public lgfx::Panel_RGB_P4
{};

class Elecrow_P4_Light : public ILight
{
  public:
    struct config_t {
        uint8_t brightness = 153; // 60%
    };

    const config_t &config(void) const { return _cfg; }
    void config(const config_t &cfg) { _cfg = cfg; }

    bool init(uint8_t brightness) override {
        auto init_res = lgfx::i2c::init(STC8_I2C_PORT, STC8_I2C_SDA_PIN, STC8_I2C_SCL_PIN);
        if (init_res.has_error()) {
            ESP_LOGE("LGFX", "Elecrow_P4_Light: failed to init i2c port %d", STC8_I2C_PORT);
            _i2cInitialized = false;
            return false;
        }

        _i2cInitialized = true;
        ESP_LOGD("LGFX", "Elecrow_P4_Light: using lgfx::i2c port %d", STC8_I2C_PORT);
        setBrightness(brightness);
        return true;
    }

    void setBrightness(uint8_t brightness) override {
        uint8_t duty = (static_cast<uint16_t>(brightness) * 100 + 127u) / 255u;
        sendI2CCommand(STC8_REG_ADDR_SET_PWM, duty);
        _cfg.brightness = brightness;
    }

    uint8_t getBrightness(void) const { return _cfg.brightness; }

    virtual ~Elecrow_P4_Light(void) = default;

  private:

    uint8_t sendI2CCommand(uint8_t cmd, uint8_t data)
    {
        uint8_t error = 4;
        if (_i2cInitialized) {
            uint8_t payload[2] = {cmd, data};
            auto write_res = lgfx::i2c::transactionWrite(STC8_I2C_PORT,
                                                         STC8_I2C_SLAVE_DEV_ADDR,
                                                         payload,
                                                         sizeof(payload),
                                                         STC8_I2C_FREQ_HZ);
            error = write_res.has_value() ? 0 : 4;
            if (error != 0) {
                ESP_LOGE("LGFX", "Elecrow_P4_Light: failed to send command 0x%02x: %d (I2C error)", (int)cmd, (int)error);
            }
        }
        return error;
    }

    bool _i2cInitialized = false;
    config_t _cfg;
};

}


class LGFX_ELECROW_P4 : public lgfx::LGFX_Device
{
    lgfx::Bus_RGB_P4       _bus_instance;
    lgfx::Elecrow_P4_Light _light_instance;
    lgfx::Panel_EK79007    _panel_instance;
    lgfx::Touch_GT911      _touch_instance;

  public:
    const uint32_t screenWidth = 800;
    const uint32_t screenHeight = 480;
  
    bool hasButton(void) { return false; }
  
    bool init_impl(bool use_reset, bool use_clear) override
    {
#if !CONFIG_SPIRAM
        ESP_LOGE("LGFX", "ELECROW_P4 need PSRAM enabled");
#elif CONFIG_SPIRAM_SPEED <= 80
        ESP_LOGE("LGFX", "ELECROW_P4 need PSRAM SPEED 200MHz");
#endif
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
            cfg.rgb_order = false;
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
            cfg.pin_cs = GPIO_NUM_NC;
            cfg.pin_sclk = GPIO_NUM_NC;
            cfg.pin_mosi = GPIO_NUM_NC;
            cfg.use_psram = 1;
            _panel_instance.config_detail(cfg);
        }

        {
            auto cfg = _bus_instance.config();
            cfg.panel = &_panel_instance;
            cfg.panel_width = screenWidth;
            cfg.panel_height = screenHeight;
            cfg.pin_d0 = GPIO_NUM_8;
            cfg.pin_d1 = GPIO_NUM_7;
            cfg.pin_d2 = GPIO_NUM_6;
            cfg.pin_d3 = GPIO_NUM_5;
            cfg.pin_d4 = GPIO_NUM_4;
            cfg.pin_d5 = GPIO_NUM_14;
            cfg.pin_d6 = GPIO_NUM_13;
            cfg.pin_d7 = GPIO_NUM_12;
            cfg.pin_d8 = GPIO_NUM_11;
            cfg.pin_d9 = GPIO_NUM_10;
            cfg.pin_d10 = GPIO_NUM_9;
            cfg.pin_d11 = GPIO_NUM_19;
            cfg.pin_d12 = GPIO_NUM_18;
            cfg.pin_d13 = GPIO_NUM_17;
            cfg.pin_d14 = GPIO_NUM_16;
            cfg.pin_d15 = GPIO_NUM_15;

            cfg.pin_henable = GPIO_NUM_2;
            cfg.pin_vsync = GPIO_NUM_41;
            cfg.pin_hsync = GPIO_NUM_40;
            cfg.pin_pclk = GPIO_NUM_3;
            cfg.freq_write = 18000000;

            cfg.hsync_polarity = false;
            cfg.hsync_pulse_width = 4;
            cfg.hsync_back_porch = 8;
            cfg.hsync_front_porch = 8;

            cfg.vsync_polarity = false;
            cfg.vsync_pulse_width = 4;
            cfg.vsync_back_porch = 16;
            cfg.vsync_front_porch = 16;

            cfg.pclk_active_neg = true;
            cfg.pclk_idle_high = true;
            cfg.de_idle_high = false;
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
            cfg.i2c_addr = 0x5D;
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
  
