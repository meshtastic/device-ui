#pragma once
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class Panel_HELTEC_V4_TFT : public lgfx::Panel_ST7789
{
  protected:
    const uint8_t *getInitCommands(uint8_t listno) const override
    {
        static uint8_t list[] = {CMD_GAMMASET, 1, 0x01, // Gamma set, curve 1
                                 0xFF, 0xFF};

        if (listno == 1)
            return list;
        return Panel_ST7789::getInitCommands(listno);
    }
};

#ifdef CUSTOM_TOUCH_DRIVER
#include "chsc6x.h"

#define SCREEN_X  240
#define SCREEN_Y  320

// custom class for redirecting getTouch() calls and to alternative implementation
class LGFX_Touch : public lgfx::LGFX_Device
{
  public:
    bool init_impl(bool use_reset, bool use_clear) override
    {
        bool result = LGFX_Device::init_impl(use_reset, use_clear);
        if(chsc6xTouch==nullptr){
            chsc6xTouch=new chsc6x(&Wire1,TOUCH_SDA_PIN,TOUCH_SCL_PIN,TOUCH_INT_PIN,TOUCH_RST_PIN);
        }
        chsc6xTouch->chsc6x_init();
        return result;
    }

    // rotation: 0=default, 1=90°, 2=180°, 3=270°
    void rotate_touch_coord(uint16_t raw_x, uint16_t raw_y, uint16_t rotation, uint16_t *out_x, uint16_t *out_y)
    {
        switch (rotation) {
            case 0: // 0°
                *out_x = raw_x;
                *out_y = raw_y;
                break;
            case 1: // 90°
                *out_x = raw_y;
                *out_y = SCREEN_X - 1 - raw_x;
                break;
            case 2: // 180°
                *out_x = SCREEN_X - 1 - raw_x;
                *out_y = SCREEN_Y - 1 - raw_y;
                break;
            case 3: // 270°
                *out_x = SCREEN_Y - 1 - raw_y;
                *out_y = raw_x;
                break;
            default: // fallback
                *out_x = raw_x;
                *out_y = raw_y;
        }
    }

    LGFX_Touch *touch(void) { return this; }

    int8_t getTouchInt(void) { return TOUCH_INT_PIN; }

    bool getTouchXY(uint16_t *touchX, uint16_t *touchY)
    {
        uint16_t rwa_x,raw_y;
        if (chsc6xTouch->chsc6x_read_touch_info(&rwa_x, &raw_y)==0) {
            rotate_touch_coord(rwa_x, raw_y,3,touchX, touchY);
            return true;
        }
        return false;
    };

    void wakeup(void) {};
    void sleep(void) {};
private:
    chsc6x *chsc6xTouch=nullptr;
};

class LGFX_HELTEC_V4_TFT : public LGFX_Touch
#else
class LGFX_HELTEC_V4_TFT : public lgfx::LGFX_Device
#endif
{
    Panel_HELTEC_V4_TFT _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;

  public:
    const uint32_t screenWidth = 240;
    const uint32_t screenHeight = 320;
    bool hasButton(void) { return true; }

    LGFX_HELTEC_V4_TFT(void)
    {
        {
            auto cfg = _bus_instance.config();

            // SPI
            cfg.spi_host = SPI3_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 80000000; // SPI clock for transmission (up to 80MHz, rounded to
                                            // the value obtained by dividing 80MHz by an integer)
            cfg.freq_read = 16000000;       // SPI clock when receiving
            cfg.spi_3wire = true;
            cfg.use_lock = true;               // Set to true to use transaction locking
            cfg.dma_channel = SPI_DMA_CH_AUTO; // SPI_DMA_CH_AUTO; // Set DMA channel
                                               // to use (0=not use DMA / 1=1ch / 2=ch
                                               // / SPI_DMA_CH_AUTO=auto setting)
            cfg.pin_sclk = LGFX_PIN_SCK;       // Set SPI SCLK pin number
            cfg.pin_mosi = LGFX_PIN_MOSI;      // Set SPI MOSI pin number
            cfg.pin_miso = -1;      // Set SPI MISO pin number (-1 = disable)
            cfg.pin_dc = LGFX_PIN_DC;          // Set SPI DC pin number (-1 = disable)

            _bus_instance.config(cfg);              // applies the set value to the bus.
            _panel_instance.setBus(&_bus_instance); // set the bus on the panel.
        }

        {                                        // Set the display panel control.
            auto cfg = _panel_instance.config(); // Gets a structure for display panel settings.

            cfg.pin_cs = LGFX_PIN_CS;     // Pin number where CS is connected (-1 = disable)
            cfg.pin_rst = LGFX_PIN_RST;   // Pin number where RST is connected  (-1 = disable)
            cfg.pin_busy = -1; // Pin number where BUSY is connected (-1 = disable)

            // The following setting values ​​are general initial values ​​for
            // each panel, so please comment out any unknown items and try them.

            cfg.panel_width = screenWidth;    // actual displayable width
            cfg.panel_height = screenHeight;  // actual displayable height
            cfg.offset_x = 0;           // Panel offset amount in X direction
            cfg.offset_y = 0;           // Panel offset amount in Y direction
            cfg.offset_rotation = 3;    // Rotation direction value offset 0~7 (4~7 is upside down)
            cfg.dummy_read_pixel = 9; // Number of bits for dummy read before pixel readout
            cfg.dummy_read_bits = 1;   // Number of bits for dummy read before non-pixel data read
            cfg.readable = false;           // Set to true if data can be read
            cfg.invert = true;         // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = false;         // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = false;      // Set to true for panels that transmit data length in 16-bit
                                                    // units with 16-bit parallel or SPI
            cfg.bus_shared = false;                  // If the bus is shared with the SD card, set to
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

            cfg.pin_bl = 21;    // Pin number to which the backlight is connected
            cfg.invert = false; // true to invert the brightness of the backlight
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance); // Set the backlight on the panel.
        }

        setPanel(&_panel_instance); // Sets the panel to use.
    }
};
