#ifndef LGFX_T_WATCH_S3_H
#define LGFX_T_WATCH_S3_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

const uint32_t screenWidth = 240;
const uint32_t screenHeight = 240;

// ST7789 TFT LCD
#define ST7789_CS 12
#define ST7789_RS 38  // DC
#define ST7789_SDA 13 // MOSI
#define ST7789_SCK 18
#define ST7789_RESET -1
#define ST7789_MISO -1
#define ST7789_BUSY -1
#define ST7789_BL 45
#define ST7789_SPI_HOST SPI3_HOST
#define ST7789_BACKLIGHT_EN 45
#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 16000000
#define SPI_TOUCH_FREQUENCY 400000

// #define TFT_BL ST7789_BL  // defined in platform.ini
#define TFT_OFFSET_X 0
#define TFT_OFFSET_Y 0
#define TFT_OFFSET_ROTATION 2 // 0: right arm, 2: left arm

#define HAS_TOUCHSCREEN 1
#define SCREEN_TOUCH_INT 16
#define TOUCH_I2C_PORT 1
#define TOUCH_SLAVE_ADDRESS 0x38

#define I2C_SDA 10 // For QMC6310 sensors and screens
#define I2C_SCL 11 // For QMC6310 sensors and screens

#define I2C_SDA1 39 // I2C pins for this board
#define I2C_SCL1 40

class LGFX_TWATCH_S3 : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_FT5x06 _touch_instance;

  public:
    LGFX_TWATCH_S3(void)
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

            cfg.panel_width = screenWidth;             // actual displayable width
            cfg.panel_height = screenHeight;           // actual displayable height
            cfg.offset_x = TFT_OFFSET_X;               // Panel offset amount in X direction
            cfg.offset_y = TFT_OFFSET_Y;               // Panel offset amount in Y direction
            cfg.offset_rotation = TFT_OFFSET_ROTATION; // Rotation direction value offset 0~7 (4~7 is
                                                       // upside down)
            cfg.dummy_read_pixel = 9;                  // Number of bits for dummy read before pixel readout
            cfg.dummy_read_bits = 1;                   // Number of bits for dummy read before non-pixel data read
            cfg.readable = false;                      // Set to true if data can be read
            cfg.invert = true;                         // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = false;                     // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = false;                    // Set to true for panels that transmit data length in 16-bit
                                                       // units with 16-bit parallel or SPI
            cfg.bus_shared = true;                     // If the bus is shared with the SD card, set to
                                                       // true (bus control with drawJpgFile etc.)

            _panel_instance.config(cfg);
        }

        // Set the backlight control. (delete if not necessary)
        {
            auto cfg = _light_instance.config(); // Gets a structure for backlight settings.

            cfg.pin_bl = ST7789_BL; // Pin number to which the backlight is connected
            cfg.invert = false;     // true to invert the brightness of the backlight
            cfg.freq = 1000;
            cfg.pwm_channel = 3;

            //_light_instance.config(cfg);
            //_panel_instance.setLight(&_light_instance); // Set the backlight on the
            // panel.
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
            cfg.offset_rotation = 2;

            // I2C
            cfg.i2c_port = TOUCH_I2C_PORT;
            cfg.i2c_addr = TOUCH_SLAVE_ADDRESS;
            cfg.pin_sda = I2C_SDA1;
            cfg.pin_scl = I2C_SCL1;
            cfg.freq = SPI_TOUCH_FREQUENCY;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance); // Sets the panel to use.
    }
};

#endif