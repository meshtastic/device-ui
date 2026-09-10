#pragma once

#ifdef SENSECAP_INDICATOR
#include <Wire.h> // TwoWire is a typedef on some cores, no forward declaration
#endif

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

#ifdef SENSECAP_INDICATOR
    /**
     * Override the bus used for the secondary (bus 1) keyboard scan, e.g. a
     * bridged TwoWire implementation on devices whose second bus lives behind
     * a co-processor. Must be set before input driver initialization; when
     * unset the local Wire1 is used.
     */
    static void setSecondaryBus(TwoWire *bus);

  private:
    static TwoWire *secondaryBus;
#endif
};
