#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <TouchDrvCSTXXX.hpp>

#ifndef SPI_FREQUENCY
#define SPI_FREQUENCY 75000000
#endif

#define TOUCH_INT 12

class LGFX_Touch : public lgfx::LGFX_Device
{
  public:
    bool init_impl(bool use_reset, bool use_clear) override
    {
        bool result = LGFX_Device::init_impl(use_reset, use_clear);
        lgfx::pinMode(TOUCH_INT, lgfx::pin_mode_t::input_pullup);
        touchDrv.setPins(-1, TOUCH_INT);
        result |= touchDrv.begin(Wire, 0x1A, 3, 2);
        if (!result) {
            ILOG_ERROR("Failed to initialise touch panel!");
        }
        return result;
    }

    LGFX_Touch *touch(void) { return this; }

    int8_t getTouchInt(void) { return TOUCH_INT; }

    // unfortunately not declared as virtual in base class, need to choose a different name
    bool getTouchXY(uint16_t *touchX, uint16_t *touchY)
    {
        return touchDrv.isPressed() && touchDrv.getPoint((int16_t *)touchX, (int16_t *)touchY, 1);
    }

    void wakeup(void) {} // TODO: use EXPANDS_TOUCH_RST
    void sleep(void)
    {
        /* touchDrv.sleep(); */
    }

  private:
    TouchDrvCST92xx touchDrv;
};

class LGFX_TWATCH_ULTRA : public LGFX_Touch
{
    lgfx::Panel_CO5300 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

  public:
    const uint32_t screenWidth = 410;
    const uint32_t screenHeight = 502;

    bool hasButton(void) { return true; }
    lgfx::ILight* light(void) { // only used by 'bool hasLight()' to check if ptr is valid
        static lgfx::Light_PWM _light_instance;
        return &_light_instance;
    }

    LGFX_TWATCH_ULTRA(void)
    {
        {
            auto cfg = _bus_instance.config();

            // configure SPI
            cfg.spi_host = SPI2_HOST; // ESP32-S2,S3,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
            cfg.spi_mode = SPI_MODE0;
            cfg.freq_write = SPI_FREQUENCY; // SPI clock for transmission (up to 80MHz, rounded to the value obtained by dividing
                                            // 80MHz by an integer)
            cfg.freq_read = 16000000;       // SPI clock when receiving
            cfg.spi_3wire = false;          // Set to true if reception is done on the MOSI pin
            cfg.use_lock = false;            // Set to true to use transaction locking
            cfg.dma_channel = SPI_DMA_CH_AUTO; // SPI_DMA_CH_AUTO; // Set DMA channel to use (0=not use DMA / 1=1ch / 2=ch /
                                               // SPI_DMA_CH_AUTO=auto setting)
            cfg.pin_sclk = 40;                 // Set SPI SCLK pin number
            cfg.pin_io0 = 38;
            cfg.pin_io1 = 39;
            cfg.pin_io2 = 42;
            cfg.pin_io3 = 45;

            _bus_instance.config(cfg);              // applies the set value to the bus.
            _panel_instance.setBus(&_bus_instance); // set the bus on the panel.
        }

        {                                        // Set the display panel control.
            auto cfg = _panel_instance.config(); // Gets a structure for display panel settings.

            cfg.pin_cs = 41;                 // Pin number where CS is connected (-1 = disable)
            cfg.pin_rst = 37;                // Pin number where RST is connected  (-1 = disable)
            cfg.panel_width = screenWidth;   // actual displayable width
            cfg.panel_height = screenHeight; // actual displayable height
            cfg.offset_x = 22;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;  // Rotation direction value offset 0~7 (4~7 is upside down)
            cfg.dummy_read_pixel = 8; // Number of bits for dummy read before pixel readout
            cfg.dummy_read_bits = 1;  // Number of bits for dummy read before non-pixel data read
            cfg.readable = true;      // Set to true if data can be read
            cfg.invert = false;       // Set to true if the light/darkness of the panel is reversed
            cfg.rgb_order = false;    // Set to true if the panel's red and blue are swapped
            cfg.dlen_16bit = false;   // Set to true for panels that transmit data length in 16-bit units
            cfg.bus_shared = false;   // If the bus is shared with the SD card, set to true (bus control with drawJpgFile etc.)

            // Set the following only when the display is shifted with a driver with a variable number of pixels
            cfg.memory_width = screenWidth;   // Maximum width supported by the driver IC
            cfg.memory_height = screenHeight; // Maximum height supported by the driver IC
            _panel_instance.config(cfg);
        }

        setPanel(&_panel_instance);
    }

    bool init()
    {
        ILOG_DEBUG("LGFX_Panel_CO5300::init()");
        lgfx::pinMode(37, lgfx::pin_mode_t::output);
        lgfx::gpio_hi(37);
        delay(200);
        lgfx::gpio_lo(37);
        delay(300);
        lgfx::gpio_hi(37);
        return lgfx::LGFX_Device::init();
    }
};
