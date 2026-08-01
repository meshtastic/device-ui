#pragma once

#ifdef INPUTDRIVER_DIRECTIONALPAD_INT

#include "Arduino.h"
#include "Wire.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "input/InputDriver.h"

// ---------------------------------------------------------------------------
// Minimal TCA6424A helper for reading input ports (and optionally initialising
// the expander when the firmware has not already done so).
//
// The full Tca6424 implementation lives in the firmware's variant.cpp; this
// subset only adds readPort() plus the optional begin() path guarded by
// TCA6424_REQUIRES_INIT.
// ---------------------------------------------------------------------------
class Tca6424Pad
{
  public:
    explicit Tca6424Pad(uint8_t address, TwoWire &wire = Wire) : address(address), wire(&wire) {}

#ifdef TCA6424_REQUIRES_INIT
    // Configure all three ports as inputs and initialise output registers.
    // Call only when the main firmware has not already initialised the expander.
    bool begin(void)
    {
        // All pins as inputs (CONFIG register: 0 = output, 1 = input)
        return writeReg(0x0C, 0xFF) && writeReg(0x0D, 0xFF) && writeReg(0x0E, 0xFF);
    }
#endif

    // Read one input port (0-2).  Returns 0xFF on I2C error.
    // Reading also clears the TCA6424A INT assertion for that port.
    uint8_t readPort(uint8_t port)
    {
        wire->beginTransmission(address);
        wire->write(port); // INPUT_PORT0 = 0x00, PORT1 = 0x01, PORT2 = 0x02
        if (wire->endTransmission(false) != 0)
            return 0xFF;
        wire->requestFrom(address, (uint8_t)1);
        if (!wire->available())
            return 0xFF;
        return wire->read();
    }

  private:
#ifdef TCA6424_REQUIRES_INIT
    bool writeReg(uint8_t reg, uint8_t value)
    {
        wire->beginTransmission(address);
        wire->write(reg);
        wire->write(value);
        return wire->endTransmission() == 0;
    }
#endif

    uint8_t address;
    TwoWire *wire;
};

// ---------------------------------------------------------------------------
// DirectionalPadInputDriver
//
// LVGL keypad driver for a 6-key directional pad wired to a TCA6424A IO
// expander.  Button assignments (PORT 0, active-low):
//
//   bit 0 → UP     (LV_KEY_UP)
//   bit 1 → DOWN   (LV_KEY_DOWN)
//   bit 2 → LEFT   (LV_KEY_LEFT)
//   bit 3 → RIGHT  (LV_KEY_RIGHT)
//   bit 4 → CONFIRM (LV_KEY_ENTER, long-press supported via LVGL timer)
//   bit 5 → RETURN (LV_KEY_ESC)
//
// Compile-time configuration macros (set via build flags or nicheGraphics.h):
//
//   INPUTDRIVER_DIRECTIONALPAD_INT       GPIO pin connected to TCA6424A INT
//                                        (required – gates the whole driver)
//   INPUTDRIVER_DIRECTIONALPAD_I2C_ADDR  TCA6424A I2C address (default 0x22)
//   INPUTDRIVER_DIRECTIONALPAD_WIRE      TwoWire instance to use (default Wire)
//   TCA6424_REQUIRES_INIT                Define when this driver must initialise
//                                        the expander (firmware has not done so)
//
// The TCA6424A INT output goes active-low whenever any input changes.  Because
// I2C cannot safely be used inside an ISR, the interrupt handler only sets a
// flag; button_read() performs the actual port read and enqueues change events
// when LVGL calls it on its normal poll cycle.  The held-key state is also
// reflected directly from lastPortState so LVGL's long-press timer runs
// correctly without needing repeated INT pulses.
// ---------------------------------------------------------------------------
class DirectionalPadInputDriver : public InputDriver
{
  public:
    DirectionalPadInputDriver(void);
    virtual void init(void) override;
    virtual ~DirectionalPadInputDriver(void) {}

    static void IRAM_ATTR intHandler(void);

  private:
    struct PadEvent {
        uint32_t key;
        lv_indev_state_t state;
    };

    static volatile bool inputPending;
    static QueueHandle_t eventQueue;
    static uint8_t lastPortState;
    static uint32_t prevKey;

    static uint32_t portBitToLvKey(uint8_t bit);
    static void button_read(lv_indev_t *indev, lv_indev_data_t *data);
};

#endif // INPUTDRIVER_DIRECTIONALPAD_INT
