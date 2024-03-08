#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

const uint32_t screenWidth = 320;
const uint32_t screenHeight = 240;

// ST7789 TFT LCD
#define ST7789_CS 12
#define ST7789_RS 11  // DC
#define ST7789_SDA 41 // MOSI
#define ST7789_SCK 40
#define ST7789_RESET -1
#define ST7789_MISO 38
#define ST7789_BUSY -1
#define ST7789_BL 42
#define ST7789_SPI_HOST SPI2_HOST
#define ST7789_BACKLIGHT_EN 42
#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 16000000

#define TFT_BL ST7789_BL
#define TFT_OFFSET_X 0
#define TFT_OFFSET_Y 0
#define TFT_OFFSET_ROTATION 1

#define HAS_TOUCHSCREEN 1
#define SCREEN_TOUCH_INT 16
#define TOUCH_I2C_PORT 0
#define TOUCH_SLAVE_ADDRESS 0x5D // I2C Touch controller

#define I2C_SDA 18 // I2C pins for this board
#define I2C_SCL 8
#define KB_POWERON 10 // must be set to HIGH
#define KB_SLAVE_ADDRESS 0x55
#define KB_BL_PIN 46 // not used for now

class LGFX_TDECK : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_GT911 _touch_instance;

  public:
    LGFX_TDECK(void)
    {
        {
            auto cfg = _bus_instance.config();

            // SPI
            cfg.spi_host = ST7789_SPI_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = SPI_FREQUENCY;     // SPI clock for transmission (up to 80MHz, rounded to
                                                // the value obtained by dividing 80MHz by an integer)
            cfg.freq_read = SPI_READ_FREQUENCY; // SPI clock when receiving
            cfg.spi_3wire = false;
            cfg.use_lock = true;               // Set to true to use transaction locking
            cfg.dma_channel = SPI_DMA_CH_AUTO; // SPI_DMA_CH_AUTO; // Set DMA channel
                                               // to use (0=not use DMA / 1=1ch / 2=ch
                                               // / SPI_DMA_CH_AUTO=auto setting)
            cfg.pin_sclk = ST7789_SCK;         // Set SPI SCLK pin number
            cfg.pin_mosi = ST7789_SDA;         // Set SPI MOSI pin number
            cfg.pin_miso = ST7789_MISO;        // Set SPI MISO pin number (-1 = disable)
            cfg.pin_dc = ST7789_RS;            // Set SPI DC pin number (-1 = disable)

            _bus_instance.config(cfg);              // applies the set value to the bus.
            _panel_instance.setBus(&_bus_instance); // set the bus on the panel.
        }

        {                                        // Set the display panel control.
            auto cfg = _panel_instance.config(); // Gets a structure for display panel settings.

            cfg.pin_cs = ST7789_CS; // Pin number where CS is connected (-1 = disable)
            cfg.pin_rst = -1;       // Pin number where RST is connected  (-1 = disable)
            cfg.pin_busy = -1;      // Pin number where BUSY is connected (-1 = disable)

            // The following setting values ​​are general initial values ​​for
            // each panel, so please comment out any unknown items and try them.

            cfg.panel_width = screenHeight; // actual displayable width
            cfg.panel_height = screenWidth; // actual displayable height
            cfg.offset_x = TFT_OFFSET_X;    // Panel offset amount in X direction
            cfg.offset_y = TFT_OFFSET_Y;    // Panel offset amount in Y direction
            cfg.offset_rotation = 1;        // Rotation direction value offset 0~7 (4~7 is upside down)
            cfg.dummy_read_pixel = 8;       // Number of bits for dummy read before pixel readout
            cfg.dummy_read_bits = 1;        // Number of bits for dummy read before non-pixel data read
            cfg.readable = true;            // Set to true if data can be read
            cfg.invert = true;              // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = false;          // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = false;         // Set to true for panels that transmit data length in 16-bit
                                            // units with 16-bit parallel or SPI
            cfg.bus_shared = true;          // If the bus is shared with the SD card, set to
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

            cfg.pin_bl = ST7789_BL; // Pin number to which the backlight is connected
            cfg.invert = false;     // true to invert the brightness of the backlight
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

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
            cfg.pin_int = SCREEN_TOUCH_INT;
            cfg.bus_shared = true;
            cfg.offset_rotation = 0;

            // I2C
            cfg.i2c_port = TOUCH_I2C_PORT;
            cfg.i2c_addr = TOUCH_SLAVE_ADDRESS;
            cfg.pin_sda = I2C_SDA;
            cfg.pin_scl = I2C_SCL;
            cfg.freq = 400000;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance); // Sets the panel to use.
    }
};
