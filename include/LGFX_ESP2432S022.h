#pragma once

#define LGFX_USE_V1
#include "Touch_CST820.h"
#include <LovyanGFX.hpp>

#ifndef SPI_FREQUENCY
#define SPI_FREQUENCY 25000000
#endif

class LGFX_ESP2432S022 : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_Parallel8 _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_CST820 _touch_instance;

  public:
    const uint32_t screenWidth = 320;
    const uint32_t screenHeight = 240;

    bool hasButton(void) { return true; }

    LGFX_ESP2432S022(void)
    {
        {
            auto cfg = _bus_instance.config();
            cfg.freq_write = SPI_FREQUENCY;
            cfg.pin_rd = 2;
            cfg.pin_wr = 4;
            cfg.pin_rs = 16;
            cfg.pin_d0 = 15;
            cfg.pin_d1 = 13;
            cfg.pin_d2 = 12;
            cfg.pin_d3 = 14;
            cfg.pin_d4 = 27;
            cfg.pin_d5 = 25;
            cfg.pin_d6 = 33;
            cfg.pin_d7 = 32;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {                                        // Set the display panel control.
            auto cfg = _panel_instance.config(); // Gets a structure for display panel settings.

            cfg.pin_cs = 17;   // Pin number where CS is connected (-1 = disable)
            cfg.pin_rst = -1;  // Pin number where RST is connected  (-1 = disable)
            cfg.pin_busy = -1; // Pin number where BUSY is connected (-1 = disable)

            // The following setting values ​​are general initial values ​​for
            // each panel, so please comment out any unknown items and try them.

            cfg.panel_width = screenHeight; // actual displayable width
            cfg.panel_height = screenWidth; // actual displayable height
            cfg.offset_x = 0;               // Panel offset amount in X direction
            cfg.offset_y = 0;               // Panel offset amount in Y direction
            cfg.offset_rotation = 1;        // Rotation direction value offset 0~7 (4~7 is upside down)
            // cfg.dummy_read_pixel = 8;     // Number of bits for dummy read before pixel readout
            // cfg.dummy_read_bits = 1;      // Number of bits for dummy read before non-pixel data read
            cfg.readable = false;   // Set to true if data can be read
            cfg.invert = false;     // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = false;  // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = false; // Set to true for panels that transmit data length in 16-bit
                                    // units with 16-bit parallel or SPI
            cfg.bus_shared = false; // If the bus is shared with the SD card, set to
                                    // true (bus control with drawJpgFile etc.)

            _panel_instance.config(cfg);
        }

        // Set the backlight control.
        {
            auto cfg = _light_instance.config(); // Gets a structure for backlight settings.

            cfg.pin_bl = 0;     // Pin number to which the backlight is connected
            cfg.invert = false; // true to invert the brightness of the backlight
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance); // Set the backlight on the panel.
        }
#if 1
        // Configure settings for touch screen control.
        {
            auto cfg = _touch_instance.config();

            cfg.pin_cs = -1;
            cfg.x_min = 0;
            // cfg.x_max = screenWidth - 1;
            cfg.y_min = 0;
            // cfg.y_max = screenHeight - 1;
            cfg.pin_int = -1;
            cfg.bus_shared = false;
            cfg.offset_rotation = 0;

            // I2C
            cfg.i2c_port = 0;
            cfg.pin_sda = 21;
            cfg.pin_scl = 22;
            cfg.freq = 100000;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
#endif
        setPanel(&_panel_instance); // Sets the panel to use.
    }
};
