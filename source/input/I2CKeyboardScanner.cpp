#include "input/I2CKeyboardScanner.h"
#include "input/I2CKeyboardInputDriver.h"

#include "util/ILog.h"
#include <Wire.h>

enum KeyboardAddresses {
    SCAN_TDECK_KB_ADDR = 0x55, // also BQ27720
    SCAN_TCA8418_KB_ADDR = 0x34,
    SCAN_CARDKB_ADDR = 0x5F,
    SCAN_BBQ10_KB_ADDR = 0x1F,
    SCAN_MPR121_KB_ADDR = 0x5A, // also DRV2605
    SCAN_STC8H_KB_ADDR = 0x6C   // Thinknode-M9
};

namespace
{

bool readRegisterByte(TwoWire &bus, uint8_t address, uint8_t reg, uint8_t &value)
{
    bus.beginTransmission(address);
    bus.write(reg);
    if (bus.endTransmission(false) != 0) {
        return false;
    }
    if (bus.requestFrom((int)address, 1) != 1 || !bus.available()) {
        return false;
    }
    value = bus.read();
    return true;
}

bool isBQ27220(TwoWire &bus, uint8_t address)
{
    uint8_t value = 0;
    // Mirrors firmware scanner logic: non-zero at reg 0x04 indicates BQ27220.
    return readRegisterByte(bus, address, 0x04, value) && value != 0;
}

bool isTCA8418(TwoWire &bus, uint8_t address)
{
    uint8_t value = 0;
    // Mirrors firmware scanner logic at address 0x34: reg 0x90 == 0 for TCA8418.
    return readRegisterByte(bus, address, 0x90, value) && value == 0x00;
}

bool isDRV2605(TwoWire &bus, uint8_t address)
{
    uint8_t value = 0;
    // Mirrors firmware scanner logic at address 0x5A: status reg 0x00 == 0xE0 for DRV2605.
    return readRegisterByte(bus, address, 0x00, value) && value == 0xE0;
}

} // namespace

I2CKeyboardScanner::I2CKeyboardScanner(void) {}

I2CKeyboardInputDriver *I2CKeyboardScanner::scan(void)
{
    I2CKeyboardInputDriver *driver = nullptr;
#ifndef ARCH_PORTDUINO
    uint8_t i2cKeyboards_bus0[] = {SCAN_TDECK_KB_ADDR, SCAN_TCA8418_KB_ADDR, SCAN_CARDKB_ADDR, SCAN_BBQ10_KB_ADDR,
                                   SCAN_MPR121_KB_ADDR};
#if WIRE_INTERFACES_COUNT >= 2
    uint8_t i2cKeyboards_bus1[] = {SCAN_CARDKB_ADDR, SCAN_STC8H_KB_ADDR};
#endif

    ILOG_DEBUG("I2CKeyboardScanner scanning bus 0 ...");
    for (uint8_t i = 0; i < sizeof(i2cKeyboards_bus0); i++) {
        uint8_t address = i2cKeyboards_bus0[i];
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            switch (address) {
#if defined T_DECK
            case SCAN_TDECK_KB_ADDR:
                if (isBQ27220(Wire, address)) {
                    ILOG_DEBUG("Address 0x%02X appears to be BQ27220, skipping T-Deck keyboard", address);
                } else {
                    driver = new TDeckKeyboardInputDriver(address);
                }
                break;
#endif
            case SCAN_TCA8418_KB_ADDR:
#if defined(T_LORA_PAGER)
                driver = new TLoraPagerKeyboardInputDriver(address);
#elif defined(T_DECK_PRO)
                driver = new TDeckProKeyboardInputDriver(address);
#else
                if (isTCA8418(Wire, address)) {
                    driver = new TCA8418KeyboardInputDriver(address);
                } else {
                    ILOG_DEBUG("Address 0x%02X is not TCA8418 signature, skipping keyboard attach", address);
                }
#endif
                break;
            case SCAN_CARDKB_ADDR:
                driver = new CardKBInputDriver(address);
                break;
            case SCAN_BBQ10_KB_ADDR:
                driver = new BBQ10KeyboardInputDriver(address);
                break;
            case SCAN_MPR121_KB_ADDR:
                if (isDRV2605(Wire, address)) {
                    ILOG_DEBUG("Address 0x%02X appears to be DRV2605, skipping MPR121 keyboard", address);
                } else {
                    driver = new MPR121KeyboardInputDriver(address);
                }
                break;
            default:
                break;
            }
        }
    }

#if WIRE_INTERFACES_COUNT >= 2
    ILOG_DEBUG("I2CKeyboardScanner scanning bus 1 ...");
    for (uint8_t i = 0; i < sizeof(i2cKeyboards_bus1); i++) {
        uint8_t address = i2cKeyboards_bus1[i];
        Wire1.beginTransmission(address);
        if (Wire1.endTransmission() == 0) {
            switch (address) {
            case SCAN_CARDKB_ADDR:
                driver = new CardKBInputDriver(address, Wire1);
                break;
#ifdef HAS_STC8H_KB
            case SCAN_STC8H_KB_ADDR:
                driver = new STC8HKeyboardInputDriver(address, Wire1);
                break;
#endif
            default:
                break;
            }
        }
    }
#endif

    if (I2CKeyboardInputDriver::getI2CKeyboardList().empty()) {
        ILOG_DEBUG("No I2C keyboards found");
    }
#endif
    return driver;
}
