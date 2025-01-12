#ifndef LGFX_PICO_H
#define LGFX_PICO_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#ifndef SPI_FREQUENCY
#define SPI_FREQUENCY 80000000
#endif
#ifndef HSPI_HOST
#define HSPI_HOST 0
#endif
#ifndef VSPI_HOST
#define VSPI_HOST 1
#endif

class LGFX_PICO_TFT_240x135 : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_GT911 _touch_instance;

  public:
    const uint32_t screenWidth = 240;
    const uint32_t screenHeight = 135;

    bool hasButton(void) { return true; }

    LGFX_PICO_TFT_240x135(void)
    {
        {
            auto cfg = _bus_instance.config();

            // SPI
            cfg.spi_host = (spi_host_device_t)VSPI_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = SPI_FREQUENCY; // SPI clock for transmission (up to 80MHz, rounded to
                                            // the value obtained by dividing 80MHz by an integer)
            cfg.freq_read = 16000000;       // SPI clock when receiving
            cfg.pin_sclk = 10;              // Set SPI SCLK pin number
            cfg.pin_mosi = 11;              // Set SPI MOSI pin number
            cfg.pin_miso = -1;              // Set SPI MISO pin number (-1 = disable)
            cfg.pin_dc = 8;                 // Set SPI DC pin number (-1 = disable)

            _bus_instance.config(cfg);              // applies the set value to the bus.
            _panel_instance.setBus(&_bus_instance); // set the bus on the panel.
        }

        {                                        // Set the display panel control.
            auto cfg = _panel_instance.config(); // Gets a structure for display panel settings.

            cfg.pin_cs = 9;    // Pin number where CS is connected (-1 = disable)
            cfg.pin_rst = 12;  // Pin number where RST is connected  (-1 = disable)
            cfg.pin_busy = -1; // Pin number where BUSY is connected (-1 = disable)

            // The following setting values ​​are general initial values ​​for
            // each panel, so please comment out any unknown items and try them.
            cfg.panel_width = screenHeight; // actual displayable width
            cfg.panel_height = screenWidth; // actual displayable height
            cfg.offset_x = 52;              // Panel offset amount in X direction
            cfg.offset_y = 40;              // Panel offset amount in Y direction
            cfg.offset_rotation = 1;        // Rotation direction value offset 0~7 (4~7 is
                                            // upside down)
            cfg.invert = true;              // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = false;          // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = false;         // Set to true for panels that transmit data length in 16-bit
                                            // units with 16-bit parallel or SPI
            cfg.bus_shared = true;          // If the bus is shared with the SD card, set to
                                            // true (bus control with drawJpgFile etc.)

            _panel_instance.config(cfg);
        }

#if 1 // got linker errors for pico when using standard framework and platform
      // (because code is not included due to missing pico/stdio.h !?) therefore
      // included a dummy in pico folder Set the backlight control. (delete if
      // not necessary)
        {
            auto cfg = _light_instance.config(); // Gets a structure for backlight settings.

            cfg.pin_bl = 13;   // Pin number to which the backlight is connected
            cfg.invert = true; // true to invert the brightness of the backlight
            cfg.freq = 44100;
            cfg.pwm_channel = 1;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance); // Set the backlight on the panel.
        }
#endif
        //_panel_instance.light(_create_pwm_backlight(TFT_BL, 0, 12000));

        setPanel(&_panel_instance); // Sets the panel to use.
    }
};

#endif