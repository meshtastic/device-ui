#ifndef LGFX_PICO_H
#define LGFX_PICO_H

#define LGFX_USE_V1
#include "Wire.h"
#include "util/ILog.h"
#include <LovyanGFX.hpp>

#ifndef SPI_FREQUENCY
#define SPI_FREQUENCY 75000000
#endif

#define LP5814_I2C_ADDR_DEFAULT 0x2c
#define TCA9535_I2C_ADDR_DEFAULT 0x21

class Wio_Tracker_Light : public lgfx::v1::ILight
{
    // LP5814 register addresses
    static constexpr uint8_t REG_DEVICE_CONFIG0 = 0x00;
    static constexpr uint8_t REG_MAX_CURRENT = 0x01;
    static constexpr uint8_t REG_ENABLE_CONTROL = 0x02;
    static constexpr uint8_t REG_DIM_MODE = 0x04;
    static constexpr uint8_t REG_ENGINE_MODE = 0x05;
    static constexpr uint8_t REG_UPDATE = 0x0F;
    static constexpr uint8_t REG_LED0_DC = 0x14;
    static constexpr uint8_t REG_LED0_PWM = 0x18;

  public:
    struct config_t {
        uint8_t brightness = 153; // 60%
    };

    const config_t &config(void) const { return _cfg; }
    void config(const config_t &cfg) { _cfg = cfg; }

    bool init(uint8_t brightness) override
    {
        // Probe device
        Wire.beginTransmission(LP5814_I2C_ADDR_DEFAULT);
        if (Wire.endTransmission() != 0) {
            ILOG_ERROR("LP5814 not found at 0x%02x", LP5814_I2C_ADDR_DEFAULT);
            return false;
        }

        writeReg(REG_DEVICE_CONFIG0, 0x01); // chip enable
        writeReg(REG_MAX_CURRENT, 0x01);    // 51 mA max current
        writeReg(REG_ENABLE_CONTROL, 0x00); // disable outputs while configuring
        writeReg(REG_DIM_MODE, 0x4E);       // dim mode config
        writeReg(REG_ENGINE_MODE, 0xF0);    // engine mode config
        // Set DC current for all 4 channels (registers 0x14..0x17)
        for (uint8_t i = 0; i < 4; i++) {
            writeReg(REG_LED0_DC + i, 200);
        }
        writeReg(REG_ENABLE_CONTROL, 0x0F); // enable all 4 channels
        writeReg(REG_UPDATE, 0x55);         // latch parameters (LP5814 requires 0x55)
        delay(5);                           // LP5814 engine startup settling time

        setBrightness(brightness);
        return true;
    }

    void setBrightness(uint8_t brightness) override
    {
        // Write PWM to all 4 channels (registers 0x18..0x1B).
        // No UPDATE write needed: after the 0x55 latch in init(), PWM writes take effect immediately.
        for (uint8_t i = 0; i < 4; i++) {
            writeReg(REG_LED0_PWM + i, brightness);
        }
        _cfg.brightness = brightness;
    }

    uint8_t getBrightness(void) const { return _cfg.brightness; }

    virtual ~Wio_Tracker_Light(void) = default;

  private:
    void writeReg(uint8_t reg, uint8_t value)
    {
        Wire.beginTransmission(LP5814_I2C_ADDR_DEFAULT);
        Wire.write(reg);
        Wire.write(value);
        uint8_t error = Wire.endTransmission();
        if (error != 0)
            ILOG_ERROR("LP5814 write reg 0x%02x failed: %d", reg, error);
    }

    config_t _cfg;
};

class LGFX_WIO_TRACKER_L2 : public lgfx::LGFX_Device
{
    lgfx::Panel_NV3031B _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Touch_GT911 _touch_instance;
    Wio_Tracker_Light _light_instance;

  public:
    const uint32_t screenWidth = 320;
    const uint32_t screenHeight = 240;

    bool hasButton(void) { return true; }

    bool init_impl(bool use_reset, bool use_clear) override
    {
        // Initialize LP5814 before GT911 touch driver runs (bus is clean here).
        _light_instance.init(_light_instance.config().brightness);

        bool result = LGFX_Device::init_impl(use_reset, use_clear);

        // GT911::init() may leave the ESP32 I2C peripheral with BUSY flag stuck
        // (NACK during GT911 probe clears the TX but not the hardware BUSY state).
        // Wire.end() + Wire.begin() resets the peripheral so LP5814 setBrightness()
        // called by init_lgfx() immediately after us doesn't timeout.
        Wire.end();
        Wire.begin(47, 48);

        return result;
    }

    lgfx::ILight *light(void) const
    {
        // pointer is used by LGFXDriver to check for hasLight()
        return (lgfx::ILight *)&_light_instance;
    }

    LGFX_WIO_TRACKER_L2(void)
    {
        {
            auto cfg = _bus_instance.config();

            // SPI
            cfg.spi_host = SPI3_HOST;
            cfg.spi_mode = 3;
            cfg.freq_write = SPI_FREQUENCY; // SPI clock for transmission (up to 80MHz, rounded to
                                            // the value obtained by dividing 80MHz by an integer)
            cfg.freq_read = 16000000;       // SPI clock when receiving
            cfg.pin_sclk = 42;              // Set SPI SCLK pin number
            // quad spi pins
            cfg.pin_io0 = 41;
            cfg.pin_io1 = 40;
            cfg.pin_io2 = 39;
            cfg.pin_io3 = 38;

            _bus_instance.config(cfg);              // applies the set value to the bus.
            _panel_instance.setBus(&_bus_instance); // set the bus on the panel.
        }

        {                                        // Set the display panel control.
            auto cfg = _panel_instance.config(); // Gets a structure for display panel settings.

            cfg.pin_cs = 46;   // Pin number where CS is connected (-1 = disable)
            cfg.pin_rst = -1;  // Pin number where RST is connected  (-1 = disable)
            cfg.pin_busy = -1; // Pin number where BUSY is connected (-1 = disable)

            cfg.panel_width = screenHeight; // actual displayable width
            cfg.panel_height = screenWidth; // actual displayable height
            cfg.memory_width = screenHeight;
            cfg.memory_height = screenWidth;
            cfg.offset_x = 0;        // Panel offset amount in X direction
            cfg.offset_y = 0;        // Panel offset amount in Y direction
            cfg.offset_rotation = 1; // Rotation direction value offset 0~7 (4~7 is
                                     // upside down)
            cfg.invert = true;       // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = true;    // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = false;  // Set to true for panels that transmit data length in 16-bit
                                     // units with 16-bit parallel or SPI
            cfg.bus_shared = false;  // If the bus is shared with the SD card, set to
                                     // true (bus control with drawJpgFile etc.)

            _panel_instance.config(cfg);
        }

        // Configure settings for touch screen control.
        {
            auto cfg = _touch_instance.config();

            cfg.pin_cs = -1;
            cfg.x_min = 0;
            cfg.x_max = screenHeight - 1;
            cfg.y_min = 0;
            cfg.y_max = screenWidth - 1;
            cfg.pin_int = -1;
            cfg.offset_rotation = 2;

            cfg.i2c_port = 0;
            cfg.i2c_addr = 0x5D;
            cfg.pin_sda = 47;
            cfg.pin_scl = 48;
            cfg.bus_shared = false;
            cfg.freq = 400000;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        _panel_instance.setLight(&_light_instance); // Attach LP5814 backlight driver
        setPanel(&_panel_instance);                 // Sets the panel to use.
    }

    void sleep(void)
    {
        _panel->setSleep(true);
        _light_instance.setBrightness(0);
    }

    void wakeup(void)
    {
        _panel->setSleep(false);
        _light_instance.setBrightness(_light_instance.config().brightness);
    }
};

#endif