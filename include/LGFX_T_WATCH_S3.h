#ifndef LGFX_T_WATCH_S3_H
#define LGFX_T_WATCH_S3_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#ifndef SPI_FREQUENCY
#define SPI_FREQUENCY 80000000
#endif

class LGFX_TWATCH_S3 : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_FT5x06 _touch_instance;

  public:
    const uint32_t screenWidth = 240;
    const uint32_t screenHeight = 240;

    LGFX_TWATCH_S3(void)
    {
        {
            auto cfg = _bus_instance.config();

            // SPI
            cfg.spi_host = SPI3_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = SPI_FREQUENCY; // SPI clock for transmission (up to 80MHz, rounded to
                                            // the value obtained by dividing 80MHz by an integer)
            cfg.freq_read = 16000000;       // SPI clock when receiving
            cfg.spi_3wire = false;
            cfg.use_lock = true;               // Set to true to use transaction locking
            cfg.dma_channel = SPI_DMA_CH_AUTO; // SPI_DMA_CH_AUTO; // Set DMA channel
                                               // to use (0=not use DMA / 1=1ch / 2=ch
                                               // / SPI_DMA_CH_AUTO=auto setting)
            cfg.pin_sclk = 18;                 // Set SPI SCLK pin number
            cfg.pin_mosi = 13;                 // Set SPI MOSI pin number
            cfg.pin_miso = -1;                 // Set SPI MISO pin number (-1 = disable)
            cfg.pin_dc = 38;                   // Set SPI DC pin number (-1 = disable)

            _bus_instance.config(cfg);              // applies the set value to the bus.
            _panel_instance.setBus(&_bus_instance); // set the bus on the panel.
        }

        {                                        // Set the display panel control.
            auto cfg = _panel_instance.config(); // Gets a structure for display panel settings.

            cfg.pin_cs = 12;   // Pin number where CS is connected (-1 = disable)
            cfg.pin_rst = -1;  // Pin number where RST is connected  (-1 = disable)
            cfg.pin_busy = -1; // Pin number where BUSY is connected (-1 = disable)

            // The following setting values ​​are general initial values ​​for
            // each panel, so please comment out any unknown items and try them.

            cfg.panel_width = screenWidth;   // actual displayable width
            cfg.panel_height = screenHeight; // actual displayable height
            cfg.offset_x = 0;                // Panel offset amount in X direction
            cfg.offset_y = 0;                // Panel offset amount in Y direction
            cfg.offset_rotation = 2;         // Rotation direction value offset 0~7 (4~7 is upside down)
            cfg.dummy_read_pixel = 9;        // Number of bits for dummy read before pixel readout
            cfg.dummy_read_bits = 1;         // Number of bits for dummy read before non-pixel data read
            cfg.readable = false;            // Set to true if data can be read
            cfg.invert = true;               // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = false;           // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = false;          // Set to true for panels that transmit data length in 16-bit
                                             // units with 16-bit parallel or SPI
            cfg.bus_shared = true;           // If the bus is shared with the SD card, set to
                                             // true (bus control with drawJpgFile etc.)

            _panel_instance.config(cfg);
        }

        // Set the backlight control. (delete if not necessary)
        {
            auto cfg = _light_instance.config(); // Gets a structure for backlight settings.

            cfg.pin_bl = 45;    // Pin number to which the backlight is connected
            cfg.invert = false; // true to invert the brightness of the backlight
            cfg.freq = 1000;
            cfg.pwm_channel = 3;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance); // Set the backlight on the panel.
        }

        // Configure settings for touch screen control.
        {
            auto cfg = _touch_instance.config();

            cfg.pin_cs = -1;
            cfg.x_min = 0;
            cfg.x_max = screenWidth - 1;
            cfg.y_min = 0;
            cfg.y_max = screenHeight - 1;
            cfg.pin_int = 16;
            cfg.bus_shared = true;
            cfg.offset_rotation = 2;

            // I2C
            cfg.i2c_port = 1;
            cfg.i2c_addr = 0x38;
            cfg.pin_sda = 39;
            cfg.pin_scl = 40;
            cfg.freq = 400000;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance); // Sets the panel to use.
    }
};

#endif