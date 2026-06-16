#pragma once

#define LGFX_USE_V1
#include "util/ILog.h"
#include <LovyanGFX.hpp>

#ifndef SPI_FREQUENCY
#define SPI_FREQUENCY 80000000
#endif

class Panel_TDeck : public lgfx::Panel_ST7789
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
#include <bb_captouch.h>

#define TOUCH_SDA 18
#define TOUCH_SCL 8
#define TOUCH_INT 16
#define TOUCH_RST -1

// avoid lovyanGFX touch driver:
// custom class for redirecting getTouch() calls and to alternative implementation
class LGFX_Touch : public lgfx::LGFX_Device
{
  public:
    bool init_impl(bool use_reset, bool use_clear) override
    {
        bool result = LGFX_Device::init_impl(use_reset, use_clear);
        bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
        bbct.setOrientation(90, 320, 240);
        return result;
    }

    LGFX_Touch *touch(void) { return this; }

    int8_t getTouchInt(void) { return TOUCH_INT; }

    // unfortunately not declared as virtual in base class, need to choose a different name
    bool getTouchXY(uint16_t *touchX, uint16_t *touchY)
    {
        TOUCHINFO ti;
        // if (!digitalRead(TOUCH_INT)) // cannot use as T-Deck Plus has inconsistent factory settings for IRQ pin
        if (bbct.getSamples(&ti)) {
            *touchX = ti.x[0];
            *touchY = ti.y[0] - 90;
            return true;
        }
        return false;
    };

    void wakeup(void) {}
    void sleep(void) {}

  private:
    BBCapTouch bbct;
};

class GT911_TDECK : public lgfx::Touch_GT911
{
};
class LGFX_TDECK : public LGFX_Touch

#else
// T-Deck specific configuration of GT911 touch driver
class GT911_TDECK : public lgfx::Touch_GT911
{
  public:
    static constexpr int GT911_I2C_ADDR_1 = 0x14;
    static constexpr int GT911_I2C_ADDR_2 = 0x5D;

    bool init(void) override
    {
        bool result = lgfx::Touch_GT911::init();
        if (result) {
            checkAndConfigureGT911();
        }
        return result;
    }

  private:
    bool transactionWriteReadWithAddrRetry(const uint8_t *writeBuf, size_t writeLen, uint8_t *readBuf, size_t readLen)
    {
        auto txrx =
            lgfx::i2c::transactionWriteRead(_cfg.i2c_port, _cfg.i2c_addr, writeBuf, writeLen, readBuf, readLen, _cfg.freq);
        if (txrx.has_value()) {
            return true;
        }

        int altAddr = (_cfg.i2c_addr == GT911_I2C_ADDR_1) ? GT911_I2C_ADDR_2 : GT911_I2C_ADDR_1;
        txrx = lgfx::i2c::transactionWriteRead(_cfg.i2c_port, altAddr, writeBuf, writeLen, readBuf, readLen, _cfg.freq);
        if (txrx.has_value()) {
            ILOG_DEBUG("GT911 diagnostic read succeeded using alternate I2C address 0x%02X (configured=0x%02X)", altAddr,
                       _cfg.i2c_addr);
            return true;
        }
        return false;
    }

    bool transactionWriteWithAddrRetry(const uint8_t *writeBuf, size_t writeLen)
    {
        auto tx = lgfx::i2c::transactionWrite(_cfg.i2c_port, _cfg.i2c_addr, writeBuf, writeLen, _cfg.freq);
        if (tx.has_value()) {
            return true;
        }

        int altAddr = (_cfg.i2c_addr == GT911_I2C_ADDR_1) ? GT911_I2C_ADDR_2 : GT911_I2C_ADDR_1;
        tx = lgfx::i2c::transactionWrite(_cfg.i2c_port, altAddr, writeBuf, writeLen, _cfg.freq);
        if (tx.has_value()) {
            ILOG_DEBUG("GT911 diagnostic write succeeded using alternate I2C address 0x%02X (configured=0x%02X)", altAddr,
                       _cfg.i2c_addr);
            return true;
        }
        return false;
    }

    static uint8_t calcGT911Checksum(const uint8_t *buf, size_t len)
    {
        uint8_t checksum = 0;
        for (size_t i = 0; i < len; ++i) {
            checksum += buf[i];
        }
        return static_cast<uint8_t>(~checksum + 1);
    }

    bool writeGT911ConfigBlock(const uint8_t (&configBlock)[184])
    {
        constexpr uint16_t configStart = 0x8047;
        constexpr size_t maxChunk = 30;
        size_t offset = 0;
        while (offset < sizeof(configBlock)) {
            const size_t chunk = (sizeof(configBlock) - offset > maxChunk) ? maxChunk : (sizeof(configBlock) - offset);
            const uint16_t reg = static_cast<uint16_t>(configStart + offset);
            uint8_t packet[2 + maxChunk] = {static_cast<uint8_t>((reg >> 8) & 0xFF), static_cast<uint8_t>(reg & 0xFF)};
            for (size_t i = 0; i < chunk; ++i) {
                packet[2 + i] = configBlock[offset + i];
            }
            if (!transactionWriteWithAddrRetry(packet, chunk + 2)) {
                return false;
            }
            offset += chunk;
        }

        uint8_t checksumWrite[3] = {0x80, 0xFF, calcGT911Checksum(configBlock, sizeof(configBlock))};
        if (!transactionWriteWithAddrRetry(checksumWrite, sizeof(checksumWrite))) {
            return false;
        }

        uint8_t freshWrite[3] = {0x81, 0x00, 0x01};
        return transactionWriteWithAddrRetry(freshWrite, sizeof(freshWrite));
    }

    bool readGT911ConfigBlock16(uint8_t (&configBlock)[16])
    {
        constexpr uint16_t configReg = 0x8047;
        uint8_t writeBuf[2] = {static_cast<uint8_t>((configReg >> 8) & 0xFF), static_cast<uint8_t>(configReg & 0xFF)};
        return transactionWriteReadWithAddrRetry(writeBuf, sizeof(writeBuf), configBlock, sizeof(configBlock));
    }

    void checkAndConfigureGT911(void)
    {
        // Log the runtime bus config that LovyanGFX selected during init.
        ILOG_DEBUG("GT911 I2C config: port=%d addr=0x%02X sda=%d scl=%d freq=%u", _cfg.i2c_port, _cfg.i2c_addr, _cfg.pin_sda,
                   _cfg.pin_scl, _cfg.freq);

        // READ PRODUCT INFO (HARDWARE ID & FIRMWARE)
        String productID = readGT911String(0x8140, 4);
        uint16_t fwVersion = readGT911U16(0x8144);
        uint16_t xResolution = readGT911U16(0x8146);
        uint16_t yResolution = readGT911U16(0x8148);

        ILOG_DEBUG("--- GT911 Chip Identification ---");
        ILOG_DEBUG("Product ID:        %s", productID.c_str());
        ILOG_DEBUG("Firmware Version:  0x%04X", fwVersion);
        ILOG_DEBUG("Screen Resolution: %u x %u", xResolution, yResolution);

        // READ CONFIGURATION PARAMETERS (RAM WORKING MEMORY)
        uint8_t configBlockSmall[16] = {0};
        if (!readGT911ConfigBlock16(configBlockSmall)) {
            ILOG_ERROR("Failed to read GT911 config block at 0x8047");
            return;
        }

        uint8_t configVersion = configBlockSmall[0];   // Register 0x8047
        uint8_t moduleSwitch1 = configBlockSmall[6];   // Register 0x804D
        uint8_t intTriggerMode = moduleSwitch1 & 0x03; // Lowest 2 bits

        static constexpr const char *intModeNames[] = {
            "0x00 (Rising Edge Trigger)",
            "0x01 (Falling Edge Trigger)",
            "0x02 (CONSTANT LOW LEVEL ON TOUCH)",
            "0x03 (Constant High Level on Touch)",
        };

        ILOG_DEBUG("--- GT911 Configuration Diagnostics ---");
        ILOG_DEBUG("Config Version (Reg 0x8047): 0x%02X%s", configVersion,
                   configVersion >= 0x5A ? "  <-- WARNING: High version number! Might block lower updates." : "  (Normal range)");
        ILOG_DEBUG("Module_Switch1 (Reg 0x804D): 0x%02X", moduleSwitch1);
        ILOG_INFO("Current TP INT Driver Mode:  %s", intModeNames[intTriggerMode]);

        if (intTriggerMode == 0x02) {
            return;
        }

        uint8_t configBlock[184] = {0};
        uint8_t writeReg[2] = {0x80, 0x47};
        if (!transactionWriteReadWithAddrRetry(writeReg, sizeof(writeReg), configBlock, sizeof(configBlock))) {
            ILOG_ERROR("Failed to read full GT911 config block for update");
            return;
        }

        configBlock[6] = static_cast<uint8_t>((configBlock[6] & 0xFC) | 0x02);
        if (configBlock[0] < 0xFF) {
            ++configBlock[0];
        }

        if (!writeGT911ConfigBlock(configBlock)) {
            ILOG_ERROR("Failed to write updated GT911 config block");
            return;
        }

        uint8_t verifyBlock[16] = {0};
        if (!readGT911ConfigBlock16(verifyBlock)) {
            ILOG_ERROR("GT911 config write done, but readback failed");
            return;
        }

        uint8_t verifyVersion = verifyBlock[0];
        uint8_t verifyMode = static_cast<uint8_t>(verifyBlock[6] & 0x03);
        if (verifyMode == 0x02) {
            ILOG_INFO("GT911 TP_INT mode confirmed constant LOW after write (version=0x%02X)", verifyVersion);
        } else {
            ILOG_ERROR("GT911 TP_INT verification failed: mode=0x%02X (expected 0x02), version=0x%02X", verifyMode,
                       verifyVersion);
        }
    }

    // Helper function to read a string from GT911 registers
    String readGT911String(uint16_t reg, int len)
    {
        if (len <= 0)
            return "";

        String result = "";
        int offset = 0;
        while (offset < len) {
            const size_t chunk = ((len - offset) < 16) ? (len - offset) : 16;
            const uint16_t addr = reg + offset;
            uint8_t writeBuf[2] = {static_cast<uint8_t>((addr >> 8) & 0xFF), static_cast<uint8_t>(addr & 0xFF)};
            uint8_t readBuf[16] = {};

            if (!transactionWriteReadWithAddrRetry(writeBuf, sizeof(writeBuf), readBuf, chunk))
                return "UNKNOWN";

            for (size_t i = 0; i < chunk; ++i) {
                char c = static_cast<char>(readBuf[i]);
                if (c >= 32 && c <= 126)
                    result += c; // Only printable characters
            }

            offset += chunk;
        }
        return result;
    }

    // Helper function to read a 16-bit little-endian value
    uint16_t readGT911U16(uint16_t reg)
    {
        uint8_t writeBuf[2] = {static_cast<uint8_t>((reg >> 8) & 0xFF), static_cast<uint8_t>(reg & 0xFF)};
        uint8_t readBuf[2] = {};
        if (!transactionWriteReadWithAddrRetry(writeBuf, sizeof(writeBuf), readBuf, sizeof(readBuf)))
            return 0;

        return (static_cast<uint16_t>(readBuf[1]) << 8) | readBuf[0];
    }
};

class LGFX_TDECK : public lgfx::LGFX_Device
#endif
{
    Panel_TDeck _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;
    GT911_TDECK _touch_instance;
    uint8_t brightness = 153;

  public:
    const uint32_t screenWidth = 320;
    const uint32_t screenHeight = 240;

    bool hasButton(void) { return true; }

    void setBrightness(uint8_t brightness)
    {
        // T-Deck PWM doesn't like 100%, so limit to 253 (99%)
        _light_instance.setBrightness(brightness < 253 ? brightness : 253);
        this->brightness = brightness;
    }

    uint8_t getBrightness(void) const { return brightness; }

    LGFX_TDECK(void)
    {
        {
            auto cfg = _bus_instance.config();

            // SPI
            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = SPI_FREQUENCY; // SPI clock for transmission (up to 80MHz, rounded to
                                            // the value obtained by dividing 80MHz by an integer)
            cfg.freq_read = 16000000;       // SPI clock when receiving
            cfg.spi_3wire = false;
            cfg.use_lock = true;               // Set to true to use transaction locking
            cfg.dma_channel = SPI_DMA_CH_AUTO; // SPI_DMA_CH_AUTO; // Set DMA channel
                                               // to use (0=not use DMA / 1=1ch / 2=ch
                                               // / SPI_DMA_CH_AUTO=auto setting)
            cfg.pin_sclk = 40;                 // Set SPI SCLK pin number
            cfg.pin_mosi = 41;                 // Set SPI MOSI pin number
            cfg.pin_miso = 38;                 // Set SPI MISO pin number (-1 = disable)
            cfg.pin_dc = 11;                   // Set SPI DC pin number (-1 = disable)

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

            cfg.panel_width = screenHeight; // actual displayable width
            cfg.panel_height = screenWidth; // actual displayable height
            cfg.offset_x = 0;               // Panel offset amount in X direction
            cfg.offset_y = 0;               // Panel offset amount in Y direction
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

            cfg.pin_bl = 42;    // Pin number to which the backlight is connected
            cfg.invert = false; // true to invert the brightness of the backlight
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
            cfg.pin_int = 16;
            cfg.offset_rotation = 0;

            // I2C
            cfg.i2c_port = 0;
            cfg.i2c_addr = 0x5D;
            cfg.pin_sda = 18;
            cfg.pin_scl = 8;
            cfg.bus_shared = true;
            cfg.freq = 400000;

            _touch_instance.config(cfg);
#ifndef CUSTOM_TOUCH_DRIVER
            _panel_instance.setTouch(&_touch_instance);
#endif
        }

        setPanel(&_panel_instance); // Sets the panel to use.
    }
};
