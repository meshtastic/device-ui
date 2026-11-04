#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <TCA9534.h>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>

#ifndef FREQ_WRITE
#define FREQ_WRITE 14000000
#endif

// Backlight / buzzer / speaker / touch control on CrowPanel Advance 4.3"/5.0"/7.0":
//   - HW rev V1.0/V1.1: TCA9534 I/O expander at I2C 0x18 (pin 1 gates backlight).
//   - HW rev V1.2+ (incl. V1.3/V1.4): STC8H1K28 microcontroller at I2C 0x30.
//     Writing a single byte selects the function, per Elecrow's reference code:
//       V1.2:  0x05..0x10  (0x05 = off, 0x10 = max)
//       V1.3+: 0..245      (0 = max, 244 = min, 245 = off)
//       Both:  246/247 buzzer on/off, 248/249 speaker on/off, 250 activate touch
//   Sending a value in the V1.3+ scale to a V1.2 board is a no-op (undefined
//   commands are ignored), so we send the V1.3+ "max brightness" byte and let
//   V1.2 hardware stay dark rather than risk flipping buzzer/speaker pins by
//   reusing bytes with overlapping meanings.
#define CROW_MCU_I2C_ADDR 0x30
#define CROW_MCU_BL_MAX 0
#define CROW_MCU_BL_OFF 245
#define CROW_MCU_TOUCH_ACTIVATE 250

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
        // Distinguish hardware revisions by probing for the TCA9534 I/O
        // expander at 0x18: present on V1.0/V1.1, absent on V1.2+ where
        // its role has been taken over by the STC8H1K28 microcontroller
        // at 0x30. Probing 0x18 is reliable even on cold boot — the
        // TCA9534 is passive and responds immediately, unlike the MCU
        // which needs a wake sequence before it ACKs.
        Wire.beginTransmission(0x18);
        bool hasExpander = (Wire.endTransmission() == 0);
        hasMcu = !hasExpander;

        if (hasMcu) {
            // V1.2+ wake sequence, mirrors Elecrow's CrowPanel-Advance-7
            // reference code. The first write may NACK because the MCU
            // is still booting; the GPIO1 pulse then latches/wakes it,
            // and subsequent writes set the desired state. Repeat the
            // command a few times so we don't rely on any single
            // transmission succeeding.
            for (int attempt = 0; attempt < 5; ++attempt) {
                writeMcu(CROW_MCU_TOUCH_ACTIVATE);
                pinMode(1, OUTPUT);
                digitalWrite(1, LOW);
                delay(120);
                pinMode(1, INPUT);
                delay(100);
                writeMcu(CROW_MCU_BL_MAX);
                delay(20);
            }
        } else {
            // V1.0/V1.1: TCA9534 I/O expander at 0x18.
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
        }

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
        if (hasMcu) {
            writeMcu(CROW_MCU_BL_OFF);
        } else {
            ioex.output(1, TCA9534::Level::L);
        }
        _panel->setSleep(true);
    }
    void wakeup(void)
    {
        if (hasMcu) {
            writeMcu(CROW_MCU_BL_MAX);
        } else {
            ioex.output(1, TCA9534::Level::H);
        }
        _panel->setSleep(false);
    }

  private:
    void writeMcu(uint8_t cmd)
    {
        Wire.beginTransmission(CROW_MCU_I2C_ADDR);
        Wire.write(cmd);
        Wire.endTransmission();
    }

    TCA9534 ioex;
    bool hasMcu = false;
};
