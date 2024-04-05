#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

const uint32_t screenWidth = 320;
const uint32_t screenHeight = 240;

#define ILI9341_CS 15
#define ILI9341_RS 2   // DC
#define ILI9341_SDA 23 // MOSI
#define ILI9341_SCK 18
#define ILI9341_RESET 5 // or connected to 5V
#define ILI9341_MISO 19 // 12
#define ILI9341_BUSY -1
#define ILI9341_BL 25
#define ILI9341_SPI_HOST VSPI_HOST

#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 16000000

// #define TFT_BL ILI9341_BL
#define TFT_OFFSET_X 0
#define TFT_OFFSET_Y 80
#define TFT_ROTATION 4

// #define HAS_TOUCHSCREEN 1
#define XPT2046_IRQ 27
#define XPT2046_MOSI 23
#define XPT2046_MISO 19
#define XPT2046_CLK 18
#define XPT2046_CS 26
#define XPT2046_SPI_HOST VSPI_HOST

class LGFX_ESPILI9341XPT2046 : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_XPT2046 _touch_instance;

  public:
    LGFX_ESPILI9341XPT2046(void)
    {
        {
            auto cfg = _bus_instance.config();

            // SPI
            cfg.spi_host = ILI9341_SPI_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = SPI_FREQUENCY;     // SPI clock for transmission (up to 80MHz, rounded to
                                                // the value obtained by dividing 80MHz by an integer)
            cfg.freq_read = SPI_READ_FREQUENCY; // SPI clock when receiving
            cfg.spi_3wire = false;
            cfg.use_lock = true;               // Set to true to use transaction locking
            cfg.dma_channel = SPI_DMA_CH_AUTO; // SPI_DMA_CH_AUTO; // Set DMA channel
                                               // to use (0=not use DMA / 1=1ch / 2=ch
                                               // / SPI_DMA_CH_AUTO=auto setting)
            cfg.pin_sclk = ILI9341_SCK;        // Set SPI SCLK pin number
            cfg.pin_mosi = ILI9341_SDA;        // Set SPI MOSI pin number
            cfg.pin_miso = ILI9341_MISO;       // Set SPI MISO pin number (-1 = disable)
            cfg.pin_dc = ILI9341_RS;           // Set SPI DC pin number (-1 = disable)

            _bus_instance.config(cfg);              // applies the set value to the bus.
            _panel_instance.setBus(&_bus_instance); // set the bus on the panel.
        }

        {                                        // Set the display panel control.
            auto cfg = _panel_instance.config(); // Gets a structure for display panel settings.

            cfg.pin_cs = ILI9341_CS;     // Pin number where CS is connected (-1 = disable)
            cfg.pin_rst = ILI9341_RESET; // Pin number where RST is connected  (-1 = disable)
            cfg.pin_busy = ILI9341_BUSY; // Pin number where BUSY is connected (-1 = disable)

            // The following setting values ​​are general initial values ​​for
            // each panel, so please comment out any unknown items and try them.

            cfg.panel_width = screenWidth;   // actual displayable width
            cfg.panel_height = screenHeight; // actual displayable height
            cfg.offset_x = TFT_OFFSET_X;     // Panel offset amount in X direction
            cfg.offset_y = TFT_OFFSET_Y;     // Panel offset amount in Y direction
            cfg.offset_rotation = 4;         // Rotation direction value offset 0~7 (4~7 is upside down)
            cfg.dummy_read_pixel = 8;        // Number of bits for dummy read before pixel readout
            cfg.dummy_read_bits = 1;         // Number of bits for dummy read before non-pixel data read
            cfg.readable = true;             // Set to true if data can be read
            cfg.invert = false;              // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = true;            // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = false;          // Set to true for panels that transmit data length in 16-bit
                                             // units with 16-bit parallel or SPI
            cfg.bus_shared = true;           // If the bus is shared with the SD card, set to
                                             // true (bus control with drawJpgFile etc.)

            // Set the following only when the display is shifted with a driver with a
            // variable number of pixels, such as the ST7735 or ILI9163.
            // cfg.memory_width = TFT_WIDTH;   // Maximum width supported by the
            // driver IC cfg.memory_height = TFT_HEIGHT;  // Maximum height supported
            // by the driver IC
            _panel_instance.config(cfg);
        }

        // Set the backlight control. (delete if not necessary)
        {
            auto cfg = _light_instance.config(); // Gets a structure for backlight settings.

            cfg.pin_bl = ILI9341_BL; // Pin number to which the backlight is connected
            cfg.invert = false;      // true to invert the brightness of the backlight
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance); // Set the backlight on the panel.
        }

        // Configure settings for touch screen control.
        {
            auto cfg = _touch_instance.config();

            cfg.freq = 2500000;
            cfg.x_min = 0;
            cfg.x_max = 4095;
            cfg.y_min = 0;
            cfg.y_max = 4095;
            cfg.pin_int = XPT2046_IRQ;
            cfg.pin_rst = -1;
            cfg.offset_rotation = 0;
            cfg.bus_shared = true;

            // SPI
            cfg.spi_host = XPT2046_SPI_HOST;
            cfg.pin_sclk = XPT2046_CLK;
            cfg.pin_miso = XPT2046_MISO;
            cfg.pin_mosi = XPT2046_MOSI;
            cfg.pin_cs = XPT2046_CS;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance); // Sets the panel to use.
    }
};
