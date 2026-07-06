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

static bool probeTDeckKeyboardWithRegisterRead(uint8_t address)
{
    // Mirror firmware scanner behavior on 0x55:
    // 0x00 => T-Deck keyboard, non-zero => BQ battery gauge.
    Wire.beginTransmission(address);
    Wire.write((uint8_t)0x04);
    if (Wire.endTransmission() != 0) {
        return false;
    }

    if (Wire.requestFrom((int)address, 1) != 1) {
        return false;
    }
    if (!Wire.available()) {
        return false;
    }

    return Wire.read() == 0;
}

I2CKeyboardInputDriver *I2CKeyboardScanner::scan(void)
{
    I2CKeyboardInputDriver *driver = nullptr;
#ifndef ARCH_PORTDUINO
    constexpr uint8_t kScanPasses = 2;
    constexpr uint8_t kProbeRetries = 2;
    constexpr uint16_t kProbeRetryDelayMs = 2;
    constexpr uint16_t kInterPassDelayMs = 20;

    uint8_t i2cKeyboards[] = {SCAN_TDECK_KB_ADDR, SCAN_TCA8418_KB_ADDR, SCAN_CARDKB_ADDR, SCAN_BBQ10_KB_ADDR,
                              SCAN_MPR121_KB_ADDR};
    ILOG_DEBUG("I2CKeyboardScanner scanning...");

    // Reset I2C bus to clear any stuck state left by touch driver LovyanGFX operations
    Wire.end();
    delay(10);
    Wire.begin(I2C_SDA, I2C_SCL, 100000);
    delay(10);

    for (uint8_t pass = 0; pass < kScanPasses && I2CKeyboardInputDriver::getI2CKeyboardList().empty(); ++pass) {
        for (uint8_t i = 0; i < sizeof(i2cKeyboards); i++) {
            uint8_t address = i2cKeyboards[i];
            bool found = false;
            for (uint8_t attempt = 0; attempt < kProbeRetries; ++attempt) {
#if defined(T_DECK)
                if (address == SCAN_TDECK_KB_ADDR) {
                    found = probeTDeckKeyboardWithRegisterRead(address);
                } else
#endif
                {
                    Wire.beginTransmission(address);
                    found = (Wire.endTransmission() == 0);
                }

                if (found) {
                    break;
                }
                delay(kProbeRetryDelayMs);
            }

            if (found) {
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
                    // TODO: driver = new TCA8418KeyboardInputDriver(address);
#endif
                    break;
                case SCAN_CARDKB_ADDR:
                    driver = new CardKBInputDriver(address);
                    break;
                case SCAN_BBQ10_KB_ADDR:
                    driver = new BBQ10KeyboardInputDriver(address);
                    break;
                case SCAN_MPR121_KB_ADDR:
                    // TODO: resolve conflict with DRV2605
                    // driver = new MPR121KeyboardInputDriver(address);
                    break;
                default:
                    break;
                }
            }
        }

        if (I2CKeyboardInputDriver::getI2CKeyboardList().empty() && pass + 1 < kScanPasses) {
            delay(kInterPassDelayMs);
        }
    }
    if (I2CKeyboardInputDriver::getI2CKeyboardList().empty()) {
        ILOG_DEBUG("No I2C keyboards found");
    }
#endif
    return driver;
}
