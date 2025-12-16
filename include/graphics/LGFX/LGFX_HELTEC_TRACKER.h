#pragma once

#define LGFX_USE_V1
#include "Arduino.h"
#include <LovyanGFX.hpp>

#if defined(HELTEC_TRACKER_V1_1) || defined(HELTEC_WIRELESS_TRACKER_V2)
#define HELTEC_ST7735_BL 21
#else
#define HELTEC_ST7735_BL 45
#define HELTEC_VTFT_CTRL 46
#endif

class LGFX_HELTEC_TRACKER : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7735S _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;
    uint8_t brightness = 153;

  public:
    const uint16_t screenWidth = 160;
    const uint16_t screenHeight = 80;

    void setBrightness(uint8_t brightness)
    {
        _light_instance.setBrightness(brightness);
        this->brightness = brightness;
    }

    bool hasButton(void) { return true; }

    LGFX_HELTEC_TRACKER(void)
    {
        {
            auto cfg = _bus_instance.config();

            // SPI
            cfg.spi_host = SPI3_HOST; // ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32
                                      // : VSPI_HOST or HSPI_HOST
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;         // SPI clock for transmission (up to 80MHz, rounded to the
                                               // value obtained by dividing 80MHz by an integer)
            cfg.freq_read = 16000000;          // SPI clock when receiving
            cfg.spi_3wire = false;             // Set to true if reception is done on the MOSI pin
            cfg.use_lock = true;               // Set to true to use transaction locking
            cfg.dma_channel = SPI_DMA_CH_AUTO; // SPI_DMA_CH_AUTO; // Set DMA channel
                                               // to use (0=not use DMA / 1=1ch / 2=ch
                                               // / SPI_DMA_CH_AUTO=auto setting)
            cfg.pin_sclk = 41;                 // Set SPI SCLK pin number
            cfg.pin_mosi = 42;                 // Set SPI MOSI pin number
            cfg.pin_miso = -1;                 // Set SPI MISO pin number (-1 = disable)
            cfg.pin_dc = 40;                   // Set SPI DC pin number (-1 = disable)

            _bus_instance.config(cfg);              // applies the set value to the bus.
            _panel_instance.setBus(&_bus_instance); // set the bus on the panel.
        }

        {                                        // Set the display panel control.
            auto cfg = _panel_instance.config(); // Gets a structure for display panel settings.

            cfg.pin_cs = 38;   // Pin number where CS is connected (-1 = disable)
            cfg.pin_rst = 39;  // Pin number where RST is connected  (-1 = disable)
            cfg.pin_busy = -1; // Pin number where BUSY is connected (-1 = disable)

            // The following setting values ​​are general initial values ​​for
            // each panel, so please comment out any unknown items and try them.
            cfg.panel_width = screenHeight; // actual displayable width
            cfg.panel_height = screenWidth; // actual displayable height
            cfg.offset_x = 24;              // Panel offset amount in X direction
            cfg.offset_y = 0;               // Panel offset amount in Y direction
            cfg.offset_rotation = 3;        // Rotation direction value offset 0~7 (4~7 is upside down)
            cfg.dummy_read_pixel = 8;       // Number of bits for dummy read before pixel readout
            cfg.dummy_read_bits = 1;        // Number of bits for dummy read before non-pixel data read
            cfg.readable = true;            // Set to true if data can be read
            cfg.invert = false;             // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = false;          // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = false;         // Set to true for panels that transmit data length in 16-bit
                                            // units with 16-bit parallel or SPI
            cfg.bus_shared = true;          // If the bus is shared with the SD card, set to
                                            // true (bus control with drawJpgFile etc.)

            // Set the following only when the display is shifted with a driver with a
            // variable number of pixels, such as the ST7735 or ILI9163.
            cfg.memory_width = 80;   // Maximum width supported by the driver IC
            cfg.memory_height = 160; // Maximum height supported by the driver IC
            _panel_instance.config(cfg);
        }

        // Set the backlight control. (delete if not necessary)
        {
            auto cfg = _light_instance.config(); // Gets a structure for backlight settings.

            cfg.pin_bl = HELTEC_ST7735_BL; // Pin number to which the backlight is connected
            cfg.invert = true;             // true to invert the brightness of the backlight
            // cfg.freq = 44100;              // PWM frequency of backlight
            // cfg.pwm_channel = 7;           // PWM channel number to use

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance); // Set the backlight on the panel.
        }

        setPanel(&_panel_instance); // Sets the panel to use.
    }

    void sleep(void)
    {
        digitalWrite(3, LOW);
        _panel->setSleep(true);
    }

    void wakeup(void)
    {
        digitalWrite(3, HIGH);
        _panel->setSleep(false);
    }
};
