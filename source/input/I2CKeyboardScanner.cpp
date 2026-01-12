#include "input/I2CKeyboardScanner.h"
#include "input/I2CKeyboardInputDriver.h"

#include "util/ILog.h"
#include <Wire.h>

enum KeyboardAddresses {
    SCAN_TDECK_KB_ADDR = 0x55,
    SCAN_TCA8418_KB_ADDR = 0x34,
    SCAN_CARDKB_ADDR = 0x5F,
    SCAN_BBQ10_KB_ADDR = 0x1F,
    SCAN_MPR121_KB_ADDR = 0x5A
};

I2CKeyboardScanner::I2CKeyboardScanner(void) {}

I2CKeyboardInputDriver *I2CKeyboardScanner::scan(void)
{
    I2CKeyboardInputDriver *driver = nullptr;
#ifndef ARCH_PORTDUINO

#if defined(T_LORA_PAGER)
    // T-Lora Pager only has TCA8418 keyboard at 0x34
    // Don't scan other addresses - they conflict with BQ27220 (0x55) and DRV2605 (0x5A)
    uint8_t i2cKeyboards[] = {SCAN_TCA8418_KB_ADDR};
#else
    uint8_t i2cKeyboards[] = {SCAN_TDECK_KB_ADDR, SCAN_TCA8418_KB_ADDR, SCAN_CARDKB_ADDR, SCAN_BBQ10_KB_ADDR,
                              SCAN_MPR121_KB_ADDR};
#endif

    ILOG_INFO("I2CKeyboardScanner scanning for keyboards...");
    for (uint8_t i = 0; i < sizeof(i2cKeyboards); i++) {
        uint8_t address = i2cKeyboards[i];
        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();
        ILOG_DEBUG("  Scanning 0x%02X: %s", address, error == 0 ? "FOUND" : "not found");
        if (error == 0) {
            switch (address) {
            case SCAN_TDECK_KB_ADDR:
                ILOG_INFO("Found T-Deck keyboard at 0x%02X", address);
                driver = new TDeckKeyboardInputDriver(address);
                break;
            case SCAN_TCA8418_KB_ADDR:
#if defined(T_LORA_PAGER)
                ILOG_INFO("Found TCA8418 keyboard at 0x%02X (T-Lora Pager)", address);
                driver = new TLoraPagerKeyboardInputDriver(address);
#elif defined(T_DECK_PRO)
                ILOG_INFO("Found TCA8418 keyboard at 0x%02X (T-Deck Pro)", address);
                driver = new TDeckProKeyboardInputDriver(address);
#else
                ILOG_INFO("Found TCA8418 keyboard at 0x%02X", address);
                driver = new TCA8418KeyboardInputDriver(address);
#endif
                break;
            case SCAN_CARDKB_ADDR:
                ILOG_INFO("Found CardKB at 0x%02X", address);
                driver = new CardKBInputDriver(address);
                break;
            case SCAN_BBQ10_KB_ADDR:
                ILOG_INFO("Found BBQ10 keyboard at 0x%02X", address);
                driver = new BBQ10KeyboardInputDriver(address);
                break;
            case SCAN_MPR121_KB_ADDR:
                ILOG_INFO("Found MPR121 keyboard at 0x%02X", address);
                driver = new MPR121KeyboardInputDriver(address);
                break;
            default:
                break;
            }
        }
    }
    if (I2CKeyboardInputDriver::getI2CKeyboardList().empty()) {
        ILOG_DEBUG("No I2C keyboards found");
    }
#endif
    return driver;
}
