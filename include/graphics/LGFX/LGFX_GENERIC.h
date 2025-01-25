#pragma once

/**
 * @file LGFX_GENERIC.h
 * @author mverch67
 * @brief Generic LGFX class that works with compiler defines
 * @version 0.1
 * @date 2024-06-24
 *
 * Usage example for https://www.aliexpress.com/item/1005006323532762.html (2.4inch ST7789 touch)
 * Add to platformio.ini (pins for T3S3 v1.1 or v1.2):
 * build_flags =
 *  -D LGFX_DRIVER_TEMPLATE
 *  -D LGFX_DRIVER=LGFX_GENERIC
 *  -D GFX_DRIVER_INC=\"graphics/LGFX/LGFX_GENERIC.h\"
 *  -D LGFX_PANEL=ST7789
 *  -D LGFX_TOUCH=XPT2046
 *  -D LGFX_INVERT_COLOR=true
 *  -D LGFX_PIN_SCK=12
 *  -D LGFX_PIN_MOSI=15
 *  -D LGFX_PIN_MISO=16
 *  -D LGFX_PIN_DC=47
 *  -D LGFX_PIN_CS=48
 *  -D LGFX_PIN_RST=34
 *  -D LGFX_PIN_BL=35
 *  -D LGFX_TOUCH_INT=38
 *  -D LGFX_TOUCH_CS=39
 *  -D LGFX_TOUCH_CLK=12
 *  -D LGFX_TOUCH_DO=15
 *  -D LGFX_TOUCH_DIN=16
 */

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#ifndef SPI_FREQUENCY
#define SPI_FREQUENCY 20000000
#endif

#ifndef LGFX_CFG_HOST
#define LGFX_CFG_HOST SPI3_HOST
#endif

#ifndef LGFX_SPI_3WIRE
#define LGFX_SPI_3WIRE false
#endif

#ifndef LGFX_PIN_SCK
#define LGFX_PIN_SCK -1
#endif

#ifndef LGFX_PIN_MOSI
#define LGFX_PIN_MOSI -1
#endif

#ifndef LGFX_PIN_MISO
#define LGFX_PIN_MISO -1
#endif

#ifndef LGFX_PIN_DC
#define LGFX_PIN_DC -1
#endif

#ifndef LGFX_PIN_CS
#define LGFX_PIN_CS -1
#endif

#ifndef LGFX_PIN_RST
#define LGFX_PIN_RST -1
#endif

#ifndef LGFX_PIN_BUSY
#define LGFX_PIN_BUSY -1
#endif

#ifndef LGFX_SCREEN_WIDTH
#define LGFX_SCREEN_WIDTH 320
#endif

#ifndef LGFX_SCREEN_HEIGHT
#define LGFX_SCREEN_HEIGHT 240
#endif

#ifndef LGFX_OFFSET_X
#define LGFX_OFFSET_X 0
#endif

#ifndef LGFX_OFFSET_Y
#define LGFX_OFFSET_Y 0
#endif

#ifndef LGFX_ROTATION
#define LGFX_ROTATION 0
#endif

#ifndef LGFX_READ_PIXEL
#define LGFX_READ_PIXEL 9
#endif

#ifndef LGFX_READ_BITS
#define LGFX_READ_BITS 1
#endif

#ifndef LGFX_READABLE
#define LGFX_READABLE true
#endif

#ifndef LGFX_INVERT_COLOR
#define LGFX_INVERT_COLOR true
#endif

#ifndef LGFX_RGB_ORDER
#define LGFX_RGB_ORDER false
#endif

#ifndef LGFX_DLEN_16BITS
#define LGFX_DLEN_16BITS false
#endif

#ifndef LGFX_INVERT_LIGHT
#define LGFX_INVERT_LIGHT false
#endif

#ifndef LGFX_PWM_FREQ
#define LGFX_PWM_FREQ 44000
#endif

#ifndef LGFX_PWM_CHANNEL
#define LGFX_PWM_CHANNEL 7
#endif

#ifndef LGFX_TOUCH_SPI_HOST
#define LGFX_TOUCH_SPI_HOST 3
#endif

#ifndef LGFX_TOUCH_SPI_FREQ
#define LGFX_TOUCH_SPI_FREQ 1000000
#endif

#ifndef LGFX_TOUCH_CS
#define LGFX_TOUCH_CS -1
#endif

#ifndef LGFX_TOUCH_X_MIN
#define LGFX_TOUCH_X_MIN 0
#endif

#ifndef LGFX_TOUCH_X_MAX
#define LGFX_TOUCH_X_MAX LGFX_SCREEN_WIDTH - 1
#endif

#ifndef LGFX_TOUCH_Y_MIN
#define LGFX_TOUCH_Y_MIN 0
#endif

#ifndef LGFX_TOUCH_Y_MAX
#define LGFX_TOUCH_Y_MAX LGFX_SCREEN_HEIGHT - 1
#endif

#ifndef LGFX_TOUCH_INT
#define LGFX_TOUCH_INT -1
#endif

#ifndef LGFX_TOUCH_RST
#define LGFX_TOUCH_RST -1
#endif

#ifndef LGFX_TOUCH_ROTATION
#define LGFX_TOUCH_ROTATION 0
#endif

#ifndef LGFX_TOUCH_I2C_PORT
#define LGFX_TOUCH_I2C_PORT 0
#endif

#ifndef LGFX_TOUCH_I2C_FREQ
#define LGFX_TOUCH_I2C_FREQ 400000
#endif

#define LCONCAT(A, B) A##B
#define LCONCAT_EXPAND(A, B) LCONCAT(A, B)

#ifndef LGFX_PANEL
#error "LGFX_PANEL must be defined!"
#else
#define LGFX_PANEL_INSTANCE LCONCAT_EXPAND(lgfx::Panel_, LGFX_PANEL)
#endif

#ifdef LGFX_TOUCH
#define LGFX_TOUCH_INSTANCE LCONCAT_EXPAND(lgfx::Touch_, LGFX_TOUCH)
#endif

class LGFX_GENERIC : public lgfx::LGFX_Device
{
    LGFX_PANEL_INSTANCE _panel_instance;
    lgfx::Bus_SPI _bus_instance;
#ifdef LGFX_PIN_BL
    lgfx::Light_PWM _light_instance;
#endif
#ifdef LGFX_TOUCH
    LGFX_TOUCH_INSTANCE _touch_instance;
#endif

  public:
    const uint32_t screenWidth = LGFX_SCREEN_WIDTH;
    const uint32_t screenHeight = LGFX_SCREEN_HEIGHT;

    bool hasButton(void) { return false; }

    LGFX_GENERIC(void)
    {
        {
            auto cfg = _bus_instance.config();

            // SPI
            cfg.spi_host = LGFX_CFG_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = SPI_FREQUENCY; // SPI clock for transmission (up to 80MHz, rounded to
                                            // the value obtained by dividing 80MHz by an integer)
            cfg.freq_read = 16000000;       // SPI clock when receiving
            cfg.spi_3wire = LGFX_SPI_3WIRE;
            cfg.use_lock = true;               // Set to true to use transaction locking
            cfg.dma_channel = SPI_DMA_CH_AUTO; // SPI_DMA_CH_AUTO; // Set DMA channel
                                               // to use (0=not use DMA / 1=1ch / 2=ch
                                               // / SPI_DMA_CH_AUTO=auto setting)
            cfg.pin_sclk = LGFX_PIN_SCK;       // Set SPI SCLK pin number
            cfg.pin_mosi = LGFX_PIN_MOSI;      // Set SPI MOSI pin number
            cfg.pin_miso = LGFX_PIN_MISO;      // Set SPI MISO pin number (-1 = disable)
            cfg.pin_dc = LGFX_PIN_DC;          // Set SPI DC pin number (-1 = disable)

            _bus_instance.config(cfg);              // applies the set value to the bus.
            _panel_instance.setBus(&_bus_instance); // set the bus on the panel.
        }

        {                                        // Set the display panel control.
            auto cfg = _panel_instance.config(); // Gets a structure for display panel settings.

            cfg.pin_cs = LGFX_PIN_CS;     // Pin number where CS is connected (-1 = disable)
            cfg.pin_rst = LGFX_PIN_RST;   // Pin number where RST is connected  (-1 = disable)
            cfg.pin_busy = LGFX_PIN_BUSY; // Pin number where BUSY is connected (-1 = disable)

            // The following setting values ​​are general initial values ​​for
            // each panel, so please comment out any unknown items and try them.

            cfg.panel_width = LGFX_SCREEN_WIDTH;    // actual displayable width
            cfg.panel_height = LGFX_SCREEN_HEIGHT;  // actual displayable height
            cfg.offset_x = LGFX_OFFSET_X;           // Panel offset amount in X direction
            cfg.offset_y = LGFX_OFFSET_Y;           // Panel offset amount in Y direction
            cfg.offset_rotation = LGFX_ROTATION;    // Rotation direction value offset 0~7 (4~7 is upside down)
            cfg.dummy_read_pixel = LGFX_READ_PIXEL; // Number of bits for dummy read before pixel readout
            cfg.dummy_read_bits = LGFX_READ_BITS;   // Number of bits for dummy read before non-pixel data read
            cfg.readable = LGFX_READABLE;           // Set to true if data can be read
            cfg.invert = LGFX_INVERT_COLOR;         // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = LGFX_RGB_ORDER;         // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = LGFX_DLEN_16BITS;      // Set to true for panels that transmit data length in 16-bit
                                                    // units with 16-bit parallel or SPI
            cfg.bus_shared = true;                  // If the bus is shared with the SD card, set to
                                                    // true (bus control with drawJpgFile etc.)

            // Set the following only when the display is shifted with a driver with a
            // variable number of pixels, such as the ST7735 or ILI9163.
            // cfg.memory_width = TFT_WIDTH;   // Maximum width supported by the
            // driver IC cfg.memory_height = TFT_HEIGHT;  // Maximum height supported
            // by the driver IC
            _panel_instance.config(cfg);
        }

#ifdef LGFX_PIN_BL
        // Set the backlight control. (delete if not necessary)
        {
            auto cfg = _light_instance.config(); // Gets a structure for backlight settings.

            cfg.pin_bl = LGFX_PIN_BL;       // Pin number to which the backlight is connected
            cfg.invert = LGFX_INVERT_LIGHT; // true to invert the brightness of the backlight
            cfg.freq = LGFX_PWM_FREQ;
            cfg.pwm_channel = LGFX_PWM_CHANNEL;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance); // Set the backlight on the panel.
        }
#endif

#ifdef LGFX_TOUCH
        // Configure settings for touch screen control.
        {
            auto cfg = _touch_instance.config();

#if LGFX_TOUCH_CS > 0
            cfg.spi_host = LGFX_TOUCH_SPI_HOST;
            cfg.freq = LGFX_TOUCH_SPI_FREQ;
            cfg.pin_cs = LGFX_TOUCH_CS;
            cfg.pin_sclk = LGFX_TOUCH_CLK;
            cfg.pin_mosi = LGFX_TOUCH_DIN;
            cfg.pin_miso = LGFX_TOUCH_DO;
#else
            cfg.pin_cs = LGFX_TOUCH_CS;
#endif
            cfg.x_min = LGFX_TOUCH_X_MIN;
            cfg.x_max = LGFX_TOUCH_X_MAX;
            cfg.y_min = LGFX_TOUCH_Y_MIN;
            cfg.y_max = LGFX_TOUCH_Y_MAX;
            cfg.pin_int = LGFX_TOUCH_INT;
            cfg.pin_rst = LGFX_TOUCH_RST;
            cfg.offset_rotation = LGFX_TOUCH_ROTATION;

#ifdef LGFX_TOUCH_I2C_ADDR
            // I2C
            cfg.i2c_port = LGFX_TOUCH_I2C_PORT;
            cfg.i2c_addr = LGFX_TOUCH_I2C_ADDR;
            cfg.pin_sda = LGFX_TOUCH_I2C_SDA;
            cfg.pin_scl = LGFX_TOUCH_I2C_SCL;
            cfg.bus_shared = true;
            cfg.freq = LGFX_TOUCH_I2C_FREQ;
#endif
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
#endif
        setPanel(&_panel_instance); // Sets the panel to use.
    }
};
