
#include "input/I2CKeyboardInputDriver.h"
#include "util/ILog.h"
#include <Arduino.h>
#include <Wire.h>

#include "indev/lv_indev_private.h"

I2CKeyboardInputDriver::KeyboardList I2CKeyboardInputDriver::i2cKeyboardList;

I2CKeyboardInputDriver::I2CKeyboardInputDriver(void) {}

void I2CKeyboardInputDriver::init(void)
{
    keyboard = lv_indev_create();
    keyboardProvider = keyboard ? this : nullptr;
    lv_indev_set_type(keyboard, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(keyboard, keyboard_read);

    if (!inputGroup) {
        inputGroup = lv_group_create();
        lv_group_set_default(inputGroup);
    }
    lv_indev_set_group(keyboard, inputGroup);
}

bool I2CKeyboardInputDriver::registerI2CKeyboard(I2CKeyboardInputDriver *driver, std::string name, uint8_t address)
{
    auto keyboardDef = std::unique_ptr<KeyboardDefinition>(new KeyboardDefinition{driver, name, address});
    i2cKeyboardList.push_back(std::move(keyboardDef));
    ILOG_INFO("Registered I2C keyboard: %s at address 0x%02X", name.c_str(), address);
    return true;
}

void I2CKeyboardInputDriver::keyboard_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    // Read from all registered keyboards
    for (auto &keyboardDef : i2cKeyboardList) {
        keyboardDef->driver->readKeyboard(keyboardDef->address, indev, data);
        if (data->state == LV_INDEV_STATE_PRESSED) {
            // If any keyboard reports a key press, we stop reading further
            return;
        }
    }
}

// ---------- TDeckKeyboardInputDriver Implementation ----------

TDeckKeyboardInputDriver::TDeckKeyboardInputDriver(uint8_t address)
{
    registerI2CKeyboard(this, "T-Deck Keyboard", address);
}

/******************************************************************
    LV_KEY_NEXT: Focus on the next object
    LV_KEY_PREV: Focus on the previous object
    LV_KEY_ENTER: Triggers LV_EVENT_PRESSED, LV_EVENT_CLICKED, or LV_EVENT_LONG_PRESSED etc. events
    LV_KEY_UP: Increase value or move upwards
    LV_KEY_DOWN: Decrease value or move downwards
    LV_KEY_RIGHT: Increase value or move to the right
    LV_KEY_LEFT: Decrease value or move to the left
    LV_KEY_ESC: Close or exit (E.g. close a Drop down list)
    LV_KEY_DEL: Delete (E.g. a character on the right in a Text area)
    LV_KEY_BACKSPACE: Delete a character on the left (E.g. in a Text area)
    LV_KEY_HOME: Go to the beginning/top (E.g. in a Text area)
    LV_KEY_END: Go to the end (E.g. in a Text area)

    LV_KEY_UP        = 17,  // 0x11
    LV_KEY_DOWN      = 18,  // 0x12
    LV_KEY_RIGHT     = 19,  // 0x13
    LV_KEY_LEFT      = 20,  // 0x14
    LV_KEY_ESC       = 27,  // 0x1B
    LV_KEY_DEL       = 127, // 0x7F
    LV_KEY_BACKSPACE = 8,   // 0x08
    LV_KEY_ENTER     = 10,  // 0x0A, '\n'
    LV_KEY_NEXT      = 9,   // 0x09, '\t'
    LV_KEY_PREV      = 11,  // 0x0B, '
    LV_KEY_HOME      = 2,   // 0x02, STX
    LV_KEY_END       = 3,   // 0x03, ETX
*******************************************************************/

void TDeckKeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    char keyValue = 0;
    uint8_t bytes = Wire.requestFrom(address, 1);
    if (Wire.available() > 0 && bytes > 0) {
        keyValue = Wire.read();
        // ignore empty reads and keycode 224(E0, shift-0 on T-Deck) which causes internal issues
        if (keyValue != (char)0x00 && keyValue != (char)0xE0) {
            data->state = LV_INDEV_STATE_PRESSED;
            ILOG_DEBUG("key press value: %d", (int)keyValue);

            switch (keyValue) {
            case 0x0D:
                keyValue = LV_KEY_ENTER;
                break;
            default:
                break;
            }
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }
    data->key = (uint32_t)keyValue;
}

// ---------- TCA8418KeyboardInputDriver Implementation ----------

TCA8418KeyboardInputDriver::TCA8418KeyboardInputDriver(uint8_t address, const char *name)
{
    registerI2CKeyboard(this, name, address);
}

void TCA8418KeyboardInputDriver::init(void)
{
    // Additional initialization for TCA8418 if needed
    I2CKeyboardInputDriver::init();
}

void TCA8418KeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    // TODO
    char keyValue = 0;
    data->state = LV_INDEV_STATE_RELEASED;
    data->key = (uint32_t)keyValue;
}

// ---------- TLoraPagerKeyboardInputDriver Implementation ----------

TLoraPagerKeyboardInputDriver::TLoraPagerKeyboardInputDriver(uint8_t address)
    : TCA8418KeyboardInputDriver(address, "TLora Pager Keyboard"), address(address)
{
}

void TLoraPagerKeyboardInputDriver::init(void)
{
    TCA8418KeyboardInputDriver::init();
    resetKeys();
    backlight = 0;

    // Four rows and ten columns; unused GPIOs must not produce keyboard events.
    const uint8_t setup[][2] = {{0x01, 0x00}, {0x1A, 0x00}, {0x1B, 0x00}, {0x1C, 0x00}, {0x1D, 0x0F}, {0x1E, 0xFF},
                                {0x1F, 0x03}, {0x20, 0x00}, {0x21, 0x00}, {0x22, 0x00}, {0x23, 0x00}, {0x24, 0x00},
                                {0x25, 0x00}, {0x29, 0x00}, {0x2A, 0x00}, {0x2B, 0x00}};
    initialized = true;
    for (const auto &setting : setup) {
        if (!writeRegister(setting[0], setting[1])) {
            initialized = false;
            break;
        }
    }
    initialized = initialized && flushEvents() && writeRegister(0x01, 0x09);
    if (!initialized)
        ILOG_ERROR("Could not initialize T-LoRa Pager keyboard");

#ifdef KB_BL_PIN
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    ledcAttach(KB_BL_PIN, 1000, 8);
    ledcWrite(KB_BL_PIN, 0);
#else
    ledcSetup(4, 1000, 8);
    ledcAttachPin(KB_BL_PIN, 4);
    ledcWrite(4, 0);
#endif
#endif
}

void TLoraPagerKeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    static constexpr uint8_t shiftKey = 29;
    static constexpr uint8_t symKey = 21;
    static constexpr uint8_t shift = 1;
    static constexpr uint8_t sym = 2;
    static const char letters[] = "qwertyuiopasdfghjkl\0\0zxcvbnm\0\0 ";
    static const char symbols[] = "1234567890*/+-=:'\"@\0\0_$;?!,.\0\0\0";

    data->continue_reading = false;
    uint8_t event = pendingEvent;
    pendingEvent = 0;
    uint8_t status = 0;
    if (!initialized || (!event && !readRegister(0x02, status))) {
        resetKeys();
    } else if (status & 0x08) {
        // Overflow may have lost a release; discard the incomplete sequence.
        flushEvents();
        resetKeys();
    } else if (event || readRegister(0x04, event)) {
        if (event == 0) {
            if (status & 0x01)
                writeRegister(0x02, 0x01);
        } else {
            data->continue_reading = true;
            uint8_t key = event & 0x7F;
            bool pressed = event & 0x80;
            if (key >= 1 && key <= 31) {
                uint32_t bit = uint32_t(1) << (key - 1);
                uint8_t modifier = key == shiftKey ? shift : (key == symKey ? sym : 0);
                if (!pressed) {
                    pressedKeys &= ~bit;
                    heldModifiers &= ~modifier;
                    if (key == activeKey)
                        activeKey = 0;
                } else if (!(pressedKeys & bit)) {
                    if (modifier) {
                        pressedKeys |= bit;
                        heldModifiers |= modifier;
                        latchedModifiers ^= modifier;
                        modifierTime = millis();
                    } else if (activeKey) {
                        // LVGL needs a release between overlapping printable keys.
                        activeKey = 0;
                        pendingEvent = event;
                    } else {
                        pressedKeys |= bit;
                        if (uint32_t(millis() - modifierTime) > 1500)
                            latchedModifiers = 0;
                        uint8_t modifiers = heldModifiers | latchedModifiers;
                        latchedModifiers = 0;
                        uint32_t value = (modifiers & sym) ? symbols[key - 1] : letters[key - 1];
                        if (!(modifiers & sym) && (modifiers & shift) && value >= 'a' && value <= 'z')
                            value -= 'a' - 'A';
                        if (key == 20)
                            value = (modifiers & sym) ? ((modifiers & shift) ? LV_KEY_PREV : LV_KEY_NEXT) : LV_KEY_ENTER;
                        else if (key == 30)
                            value = (modifiers & sym) ? LV_KEY_ESC : LV_KEY_BACKSPACE;
                        else if (key == 31 && (modifiers & sym))
                            toggleBacklight();
                        if (value) {
                            activeKey = key;
                            keyValue = value;
                        }
                    }
                }
            }
        }
    } else {
        resetKeys();
    }
    data->state = activeKey ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    data->key = keyValue;
}

bool TLoraPagerKeyboardInputDriver::readRegister(uint8_t reg, uint8_t &value)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0 || Wire.requestFrom((int)address, 1) != 1 || !Wire.available())
        return false;
    value = Wire.read();
    return true;
}

bool TLoraPagerKeyboardInputDriver::writeRegister(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

bool TLoraPagerKeyboardInputDriver::flushEvents(void)
{
    uint8_t event = 0;
    for (unsigned i = 0; i < 10; ++i) {
        if (!readRegister(0x04, event))
            return false;
        if (!event)
            break;
    }
    for (uint8_t reg = 0x11; reg <= 0x13; ++reg) {
        if (!readRegister(reg, event))
            return false;
    }
    return writeRegister(0x02, 0x1F);
}

void TLoraPagerKeyboardInputDriver::resetKeys(void)
{
    pressedKeys = 0;
    heldModifiers = 0;
    latchedModifiers = 0;
    activeKey = 0;
    pendingEvent = 0;
}

void TLoraPagerKeyboardInputDriver::toggleBacklight(void)
{
    backlight = backlight == 0 ? 40 : (backlight == 40 ? 127 : 0);
#ifdef KB_BL_PIN
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    ledcWrite(KB_BL_PIN, backlight);
#else
    ledcWrite(4, backlight);
#endif
#endif
}

// ---------- TDeckProKeyboardInputDriver Implementation ----------

TDeckProKeyboardInputDriver::TDeckProKeyboardInputDriver(uint8_t address) : TCA8418KeyboardInputDriver(address)
{
    registerI2CKeyboard(this, "T-Deck Pro Keyboard", address);
}

void TDeckProKeyboardInputDriver::init(void)
{
    // Additional initialization for TLora-Pager if needed
    TCA8418KeyboardInputDriver::init();
}

void TDeckProKeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    // TODO
    char keyValue = 0;
    data->state = LV_INDEV_STATE_RELEASED;
    data->key = (uint32_t)keyValue;
}

// ---------- BBQ10KeyboardInputDriver Implementation ----------

BBQ10KeyboardInputDriver::BBQ10KeyboardInputDriver(uint8_t address)
{
    registerI2CKeyboard(this, "BBQ10 Keyboard", address);
}

void BBQ10KeyboardInputDriver::init(void)
{
    I2CKeyboardInputDriver::init();
    // Additional initialization for BBQ10 if needed
}

void BBQ10KeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    char keyValue = 0;
    uint8_t bytes = Wire.requestFrom(address, 1);
    if (Wire.available() > 0 && bytes > 0) {
        keyValue = Wire.read();
        // ignore empty reads and keycode 224(E0, shift-0 on T-Deck) which causes internal issues
        if (keyValue != (char)0x00 && keyValue != (char)0xE0) {
            data->state = LV_INDEV_STATE_PRESSED;
            ILOG_DEBUG("key press value: %d", (int)keyValue);

            switch (keyValue) {
            case 0x0D:
                keyValue = LV_KEY_ENTER;
                break;
            default:
                break;
            }
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }
    data->key = (uint32_t)keyValue;
}

// ---------- CardKBInputDriver Implementation ----------

CardKBInputDriver::CardKBInputDriver(uint8_t address, TwoWire &wire_) : wire(wire_)
{
    registerI2CKeyboard(this, "Card Keyboard", address);
}

void CardKBInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    char keyValue = 0;
    wire.requestFrom(address, 1);
    if (wire.available() > 0) {
        keyValue = wire.read();
        // ignore empty reads and keycode 224 which causes internal issues
        if (keyValue != (char)0x00 && keyValue != (char)0xE0) {
            data->state = LV_INDEV_STATE_PRESSED;
            ILOG_DEBUG("key press value: %d", (int)keyValue);

            switch (keyValue) {
            case 0x0D:
                keyValue = LV_KEY_ENTER;
                break;
            case 0xB4:
                keyValue = LV_KEY_LEFT;
                break;
            case 0xB5:
                keyValue = LV_KEY_UP;
                break;
            case 0xB6:
                keyValue = LV_KEY_DOWN;
                break;
            case 0xB7:
                keyValue = LV_KEY_RIGHT;
                break;
            case 0x99: // Fn+UP
                keyValue = LV_KEY_HOME;
                break;
            case 0xA4: // Fn+DOWN
                keyValue = LV_KEY_END;
                break;
            case 0x8B: // Fn+BS
                keyValue = LV_KEY_DEL;
                break;
            case 0x8C: // Fn+TAB
                keyValue = LV_KEY_PREV;
                break;
            case 0xA3: // Fn+ENTER
                // simulate a long press on Fn+ENTER (see indev_keypad_proc() in indev.c)
                indev->wait_until_release = 0;
                indev->pr_timestamp = lv_tick_get() - indev->long_press_time - 1;
                indev->long_pr_sent = 0;
                indev->keypad.last_state = LV_INDEV_STATE_PRESSED;
                indev->keypad.last_key = LV_KEY_ENTER;
                keyValue = LV_KEY_ENTER;
                break;
            default:
                break;
            }
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }
    data->key = (uint32_t)keyValue;
}

// ---------- MPR121KeyboardInputDriver Implementation ----------

MPR121KeyboardInputDriver::MPR121KeyboardInputDriver(uint8_t address)
{
    registerI2CKeyboard(this, "MPR121 Keyboard", address);
}

void MPR121KeyboardInputDriver::init(void)
{
    I2CKeyboardInputDriver::init();
    // Additional initialization for MPR121 if needed
}

void MPR121KeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    // TODO
    char keyValue = 0;
    data->state = LV_INDEV_STATE_RELEASED;
    data->key = (uint32_t)keyValue;
}
