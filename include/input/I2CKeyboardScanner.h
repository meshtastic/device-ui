#pragma once

class I2CKeyboardInputDriver;

/**
 * @brief This class is used to scan I2C connected keyboards and creates
 *        the corresponding input device (factory).
 *        It is used by the T-Deck, T-Deck Pro, T-Lora Pager, and other
 *        I2C connected keyboards.
 */
class I2CKeyboardScanner
{
  public:
    I2CKeyboardScanner(void);
    virtual I2CKeyboardInputDriver *scan(void);
    virtual ~I2CKeyboardScanner(void) {}
};
