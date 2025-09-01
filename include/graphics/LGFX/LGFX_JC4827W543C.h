
#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/panel/Panel_NV3041A.hpp>

namespace lgfx
{
inline namespace v1
{
constexpr uint8_t Panel_NV3041A::init_cmds[92 * 2];
}
} // namespace lgfx

class LGFX_JC4827W543C : public lgfx::LGFX_Device
{

    lgfx::Touch_GT911 _touch_instance;
    lgfx::Panel_NV3041A _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;

  public:
    const uint32_t screenWidth = 480;
    const uint32_t screenHeight = 272;

    bool hasButton(void) { return false; }

    bool init_impl(bool use_reset, bool use_clear) override
    {
        pinMode(1, OUTPUT);
        digitalWrite(1, HIGH);
        return LGFX_Device::init_impl(use_reset, use_clear);
    }

    LGFX_JC4827W543C(void)
    {
        {
            auto cfg = _bus_instance.config();

            cfg.spi_host = SPI3_HOST;
            cfg.spi_mode = 1;
            cfg.freq_write = 32000000UL; // NV3041A Maximum supported speed is 32MHz
            cfg.freq_read = 16000000;
            cfg.spi_3wire = true;
            cfg.use_lock = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;

            cfg.pin_sclk = GPIO_NUM_47;
            cfg.pin_io0 = GPIO_NUM_21;
            cfg.pin_io1 = GPIO_NUM_48;
            cfg.pin_io2 = GPIO_NUM_40;
            cfg.pin_io3 = GPIO_NUM_39;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {
            auto cfg = _panel_instance.config();

            cfg.pin_cs = GPIO_NUM_45;
            cfg.pin_rst = GPIO_NUM_4;
            cfg.pin_busy = -1;

            cfg.panel_width = screenWidth;
            cfg.panel_height = screenHeight;

            cfg.memory_width = screenWidth;
            cfg.memory_height = screenHeight;

            cfg.offset_x = 0;
            cfg.offset_y = 0;

            cfg.offset_rotation = 0;

            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;

            cfg.readable = true;
            cfg.invert = true;
            cfg.rgb_order = true;
            cfg.dlen_16bit = false;
            cfg.bus_shared = false;

            _panel_instance.config(cfg);
        }

        { // Configure settings for touch screen control.  (delete if not necessary)
            auto cfg = _touch_instance.config();

            cfg.pin_cs = -1;
            cfg.x_min = 0;
            cfg.x_max = screenWidth - 1;
            cfg.y_min = 0;
            cfg.y_max = screenHeight - 1;
            cfg.offset_rotation = 0;

            // For I2C connection
            cfg.i2c_port = 0;
            cfg.i2c_addr = 0x5D;    // 0x14 or 0x5D
            cfg.bus_shared = false; // Set true when using a common bus with the screen
            cfg.freq = 400000;      // I2C Clock frequency
            cfg.pin_int = 3;        // Pin number to which INT is connected (-1 = not connected)
            cfg.pin_rst = 38;       // Pin number to which RST is connected (-1 = not connected)
            cfg.pin_sda = SDA;
            cfg.pin_scl = SCL;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        {
            auto cfg = _light_instance.config();

            cfg.pin_bl = GPIO_NUM_1;
            cfg.invert = false;
            cfg.freq = 44000;
            cfg.pwm_channel = 1;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        setPanel(&_panel_instance);
    }
};
