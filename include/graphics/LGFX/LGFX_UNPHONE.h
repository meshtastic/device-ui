#pragma once
#define LGFX_USE_V1

#include <LovyanGFX.hpp>

#ifdef UNPHONE
#include "unPhone.h"
extern unPhone unphone;
#endif

#ifndef SPI_FREQUENCY
#define SPI_FREQUENCY 40000000
#endif

class LGFX_UNPHONE_V9 : public lgfx::LGFX_Device
{
    class Panel_Unphone : public lgfx::Panel_HX8357D
    {
      public:
        void setBrightness(uint8_t brightness) override { light()->setBrightness(brightness); }
    } _panel_instance;

    class Unphone_Light : public lgfx::Light_PWM
    {
      public:
        bool init(uint8_t brightness) override
        {
            setBrightness(128);
            return true;
        }
        void setBrightness(uint8_t brightness) override { unphone.backlight(brightness > 0); }
    } _light_instance;

    lgfx::Bus_SPI _bus_instance;
    lgfx::Touch_XPT2046 _touch_instance;

  public:
    const uint32_t screenWidth = 320;
    const uint32_t screenHeight = 480;

    bool hasButton(void) { return true; }

    LGFX_UNPHONE_V9(void)
    {
        {
            // configure SPI
            auto cfg = _bus_instance.config();

            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = SPI_FREQUENCY; // SPI clock for transmission (up to 80MHz, rounded to the value obtained by dividing
                                            // 80MHz by an integer)
            cfg.freq_read = 16000000;       // SPI clock when receiving
            cfg.spi_3wire = false;          // Set to true if reception is done on the MOSI pin
            cfg.use_lock = true;            // Set to true to use transaction locking
            cfg.dma_channel = SPI_DMA_CH_AUTO; // SPI_DMA_CH_AUTO; // Set DMA channel to use (0=not use DMA / 1=1ch / 2=ch /
                                               // SPI_DMA_CH_AUTO=auto setting)
            cfg.pin_sclk = 39;                 // Set SPI SCLK pin number
            cfg.pin_mosi = 40;                 // Set SPI MOSI pin number
            cfg.pin_miso = 41;                 // Set SPI MISO pin number (-1 = disable)
            cfg.pin_dc = 47;                   // Set SPI DC pin number (-1 = disable)

            _bus_instance.config(cfg);              // applies the set value to the bus.
            _panel_instance.setBus(&_bus_instance); // set the bus on the panel.
        }
        {
            // Set the display panel control.
            auto cfg = _panel_instance.config(); // Gets a structure for display panel settings.

            cfg.pin_cs = 48;   // Pin number where CS is connected (-1 = disable)
            cfg.pin_rst = 46;  // Pin number where RST is connected  (-1 = disable)
            cfg.pin_busy = -1; // Pin number where BUSY is connected (-1 = disable)

            cfg.panel_width = screenWidth;   // actual displayable width
            cfg.panel_height = screenHeight; // actual displayable height
            cfg.offset_x = 0;                // Panel offset amount in X direction
            cfg.offset_y = 0;                // Panel offset amount in Y direction
            cfg.offset_rotation = 6;         // Rotation direction value offset 0~7 (4~7 is upside down)
            cfg.dummy_read_pixel = 8;        // Number of bits for dummy read before pixel readout
            cfg.dummy_read_bits = 1;         // Number of bits for dummy read before non-pixel data read
            cfg.readable = true;             // Set to true if data can be read
            cfg.invert = false;              // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = false;           // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = false;
            cfg.bus_shared = true; // If the bus is shared with the SD card, set to true (bus control with drawJpgFile etc.)

            _panel_instance.config(cfg);
        }

        {
            // Configure settings for touch control.
            auto cfg = _touch_instance.config();

            cfg.freq = 1000000;
            cfg.x_min = 300;
            cfg.x_max = 3800;
            cfg.y_min = 500;
            cfg.y_max = 3750;
            cfg.pin_int = -1;
            cfg.bus_shared = true;
            cfg.offset_rotation = 6;

            cfg.spi_host = SPI2_HOST;
            cfg.pin_sclk = 39;
            cfg.pin_mosi = 40;
            cfg.pin_miso = 41;
            cfg.pin_cs = 38;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        _panel_instance.setLight(&_light_instance);

        setPanel(&_panel_instance);
    }
};
