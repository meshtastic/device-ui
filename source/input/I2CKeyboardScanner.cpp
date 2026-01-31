#include "input/I2CKeyboardScanner.h"
#include "input/I2CKeyboardInputDriver.h"

#include "util/ILog.h"
#include <Wire.h>

enum KeyboardAddresses {
    SCAN_TDECK_KB_ADDR = 0x55, // also BQ27720
    SCAN_TCA8418_KB_ADDR = 0x34,
    SCAN_CARDKB_ADDR = 0x5F,
    SCAN_BBQ10_KB_ADDR = 0x1F,
    SCAN_MPR121_KB_ADDR = 0x5A // also DRV2605
};

I2CKeyboardScanner::I2CKeyboardScanner(void) {}

I2CKeyboardInputDriver *I2CKeyboardScanner::scan(void)
{
    I2CKeyboardInputDriver *driver = nullptr;
#ifndef ARCH_PORTDUINO
    uint8_t i2cKeyboards[] = {SCAN_TDECK_KB_ADDR, SCAN_TCA8418_KB_ADDR, SCAN_CARDKB_ADDR, SCAN_BBQ10_KB_ADDR,
                              SCAN_MPR121_KB_ADDR};
    ILOG_DEBUG("I2CKeyboardScanner scanning...");
    for (uint8_t i = 0; i < sizeof(i2cKeyboards); i++) {
        uint8_t address = i2cKeyboards[i];
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            switch (address) {
#if defined T_DECK
            case SCAN_TDECK_KB_ADDR:
                driver = new TDeckKeyboardInputDriver(address);
                break;
#endif
            case SCAN_TCA8418_KB_ADDR:
#if defined(T_LORA_PAGER)
                driver = new TLoraPagerKeyboardInputDriver(address);
#elif defined(T_DECK_PRO)
                driver = new TDeckProKeyboardInputDriver(address);
#else
                driver = new TCA8418KeyboardInputDriver(address);
#endif
                break;
            case SCAN_CARDKB_ADDR:
                driver = new CardKBInputDriver(address);
                break;
            case SCAN_BBQ10_KB_ADDR:
                driver = new BBQ10KeyboardInputDriver(address);
                break;
            case SCAN_MPR121_KB_ADDR:
                //                driver = new MPR121KeyboardInputDriver(address);
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
