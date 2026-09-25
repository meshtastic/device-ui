#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include <Arduino.h>
#include <esp_log.h>

#if !defined(CONFIG_IDF_TARGET_ESP32P4)
#error "CONFIG_IDF_TARGET_ESP32P4 should be set"
#endif

#include "graphics/LGFX/experimental/esp32p4/Panel_EK79007D.hpp"

// ThinkNode MX specific configuration of GT911 touch driver
class GT911_MX : public lgfx::Touch_GT911
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

class LGFX_ELECROW_MX : public lgfx::LGFX_Device
{
    lgfx::experimental::Bus_DSI _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Panel_EK79007D _panel_instance;
    GT911_MX _touch_instance;

  public:
    const uint32_t screenWidth = 1024;
    const uint32_t screenHeight = 600;

#if defined(USE_FULL_DOUBLE_BUFFER)
    void *getFrameBuffer(uint8_t index) const { return _panel_instance.getFrameBuffer(index); }
    bool presentFrameBuffer(const void *frame_buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
    {
        return _panel_instance.presentFrameBuffer(frame_buffer, x, y, width, height);
    }
    void waitFrameBuffer(void) { _panel_instance.waitFrameBuffer(); }
#endif
    bool hasButton(void) { return false; }

    bool init_impl(bool use_reset, bool use_clear) override
    {
#if !CONFIG_SPIRAM
        ESP_LOGE("LGFX", "ELECROW_MX needs PSRAM enabled");
        if (false)
#elif CONFIG_SPIRAM_SPEED <= 80
        ESP_LOGE("LGFX", "ELECROW_MX needs PSRAM speed above 80 MHz");
#endif
            if (_bus_instance.init()) {
                lgfx::delay(250);
            }
        return lgfx::LGFX_Device::init_impl(use_reset, use_clear);
    }

    LGFX_ELECROW_MX(void)
    {
        {
            auto cfg = _panel_instance.config();

            cfg.memory_width = screenWidth;
            cfg.memory_height = screenHeight;
            cfg.panel_width = screenWidth;
            cfg.panel_height = screenHeight;
            cfg.readable = true;
            cfg.rgb_order = true;
            cfg.bus_shared = false;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.pin_cs = GPIO_NUM_NC;
            cfg.pin_rst = GPIO_NUM_24;
            _panel_instance.config(cfg);
        }

        {
            auto cfg = _panel_instance.config_detail();
#if defined(USE_FULL_DOUBLE_BUFFER)
            cfg.num_fbs = 2;
#endif
            cfg.dpi_freq_mhz = 51;
            cfg.hsync_back_porch = 90;
            cfg.hsync_pulse_width = 70;
            cfg.hsync_front_porch = 160;
            cfg.vsync_back_porch = 13;
            cfg.vsync_pulse_width = 10;
            cfg.vsync_front_porch = 21;
            _panel_instance.config_detail(cfg);
        }

        {
            auto cfg = _bus_instance.config();
            cfg.bus_id = 0;
            cfg.lane_num = 2;
            cfg.lane_mbps = 900;
            cfg.ldo_chan_id = 3;
            cfg.ldo_voltage_mv = 2500;
            _bus_instance.config(cfg);
        }
        _panel_instance.setBus(&_bus_instance);

        {
            auto cfg = _touch_instance.config();
            cfg.pin_cs = -1;
            cfg.x_min = 0;
            cfg.y_min = 0;
            cfg.x_max = screenWidth - 1;
            cfg.y_max = screenHeight - 1;
            cfg.pin_int = GPIO_NUM_8;
            cfg.pin_rst = GPIO_NUM_34;
            cfg.offset_rotation = 2;
            // I2C
            cfg.i2c_port = 1;
            cfg.i2c_addr = 0x5D;
            cfg.pin_sda = GPIO_NUM_12;
            cfg.pin_scl = GPIO_NUM_13;
            cfg.bus_shared = true;
            cfg.freq = 400000;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = GPIO_NUM_26;
            cfg.invert = false;
            cfg.freq = 30000;
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        setPanel(&_panel_instance);
    }
};
