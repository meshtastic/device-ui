
#include "input/I2CKeyboardInputDriver.h"
#include "indev/lv_indev_private.h"
#include "input/policy/DefaultInputPolicyFactory.h"
#include "input/policy/InputContextState.h"
#include "input/policy/InputPipeline.h"
#include "input/policy/InputSourceRegistry.h"
#include "input/policy/UICommandDispatcher.h"
#include "util/ILog.h"
#include <Arduino.h>

std::shared_ptr<input_policy::InputPipeline> matrixPipeline;
std::shared_ptr<input_policy::IInputContextProvider> contextProvider;
std::shared_ptr<input_policy::IUICommandDispatcher> commandDispatcher;

I2CKeyboardInputDriver::KeyboardList I2CKeyboardInputDriver::i2cKeyboardList;

I2CKeyboardInputDriver::I2CKeyboardInputDriver(void) {}

void I2CKeyboardInputDriver::init(void)
{
    keyboard = lv_indev_create();
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
    uint8_t bytes = Wire.requestFrom(address, (uint8_t)1);
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

TCA8418KeyboardInputDriver::TCA8418KeyboardInputDriver(uint8_t address)
{
    registerI2CKeyboard(this, "TCA8418 Keyboard", address);
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

TLoraPagerKeyboardInputDriver::TLoraPagerKeyboardInputDriver(uint8_t address) : TCA8418KeyboardInputDriver(address)
{
    registerI2CKeyboard(this, "TLora Pager Keyboard", address);
}

void TLoraPagerKeyboardInputDriver::init(void)
{
    // Additional initialization for TLora-Pager if needed
    TCA8418KeyboardInputDriver::init();
}

void TLoraPagerKeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    // TODO
    char keyValue = 0;
    data->state = LV_INDEV_STATE_RELEASED;
    data->key = (uint32_t)keyValue;
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
    uint8_t bytes = Wire.requestFrom(address, (uint8_t)1);
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
    Wire.requestFrom(address, (uint8_t)1);
    if (Wire.available() > 0) {
        keyValue = Wire.read();
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
    char keyValue = 0;
    data->state = LV_INDEV_STATE_RELEASED;
    data->key = (uint32_t)keyValue;
}

// ---------- TM9KeyboardInputDriver Implementation ----------

const input_policy::InputCapabilities TM9Capabilities = {
    true,  // hasArrowKeys
    false, // hasTabKey
    true,  // hasHomeKey
    true,  // hasEndKey
    true,  // hasPgUpPgDownKeys
    true,  // hasCancelKey
    true,  // hasEnterKey
    true,  // hasModifiers
    true,  // supportsLongPress
    true,  // supportsRepeat
    true   // supportsTextEntry
};

TM9KeyboardInputDriver::TM9KeyboardInputDriver(uint8_t address, TwoWire &wire_) : wire(wire_)
{
    registerI2CKeyboard(this, "TM9 Keyboard", address);
}

void TM9KeyboardInputDriver::init(void)
{
    // Additional initialization for ThinkNodeM9 if needed
    I2CKeyboardInputDriver::init();

    if (!contextProvider) {
        contextProvider = std::shared_ptr<input_policy::IInputContextProvider>(&input_policy::InputContextState::instance(),
                                                                               [](input_policy::IInputContextProvider *) {});
    }

    if (!commandDispatcher) {
        commandDispatcher = std::shared_ptr<input_policy::IUICommandDispatcher>(&input_policy::UICommandDispatcher::instance(),
                                                                                [](input_policy::IUICommandDispatcher *) {});
    }

    if (!matrixPipeline) {
        input_policy::InputSourceRegistry registry;
        input_policy::DefaultInputPolicyFactory factory;
        auto result = factory.build(registry, contextProvider, commandDispatcher);

        matrixPipeline =
            std::make_shared<input_policy::InputPipeline>(result.bindingResolver, contextProvider, commandDispatcher);
        matrixPipeline->setPolicyChain(std::move(result.chain));
    }
}

void TM9KeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    uint32_t keyValue = 0;
    static uint32_t prevKey = 0;
    bool isSyntheticLongPress = false;

    wire.beginTransmission(address);
    wire.write(0x01);
    if (wire.endTransmission(false) == 0) {
        uint8_t bytes = wire.requestFrom(address, 1);
        if (wire.available() > 0 && bytes > 0) {
            keyValue = wire.read();
            if (keyValue != 0x00) {
                data->state = LV_INDEV_STATE_PRESSED;
                ILOG_DEBUG("key press value: 0x%02X", keyValue);

                switch (keyValue) {
                case 0x0D: // Enter
                    keyValue = LV_KEY_ENTER;
                    break;
                case 0x81: // Chat boxes
                    keyValue = 0x100;
                    break;
                case 0x82: // Home
                    keyValue = 0x101;
                    break;
                case 0x83: // Preset -> quick chat
                    keyValue = 0x102;
                    break;
                case 0x84: // location -> send ping
                    keyValue = 0x103;
                    break;
                case 0x85: // MAP
                    keyValue = 0x104;
                    break;
                case 0x86: // BACK
                    keyValue = LV_KEY_ESC;
                    break;
                case 0x87: // long-press location -> toggle GPS
                    keyValue = 0x105;
                    break;
                case 0x88: // button 1st code / location 2nd code -> ignore
                    keyValue = 0;
                    data->state = LV_INDEV_STATE_RELEASED;
                    break;
                case 0xB4: // Left
                    keyValue = LV_KEY_LEFT;
                    break;
                case 0xB5: // Up
                    keyValue = LV_KEY_UP;
                    break;
                case 0xB6: // Down
                    keyValue = LV_KEY_DOWN;
                    break;
                case 0xB7: // Right
                    keyValue = LV_KEY_RIGHT;
                    break;
                case 0x08: // Del
                    keyValue = LV_KEY_BACKSPACE;
                    break;
                case 0xA3: // LONG ENTER
                    // simulate a long press (see indev_keypad_proc() in indev.c)
                    if (indev != nullptr) {
                        indev->wait_until_release = 0;
                        indev->pr_timestamp = lv_tick_get() - indev->long_press_time - 1;
                        indev->long_pr_sent = 0;
                        indev->keypad.last_state = LV_INDEV_STATE_PRESSED;
                        indev->keypad.last_key = LV_KEY_ENTER;
                    }
                    isSyntheticLongPress = true;
                    keyValue = LV_KEY_ENTER;
                    break;
                default:
                    break;
                }
            } else {
                data->state = LV_INDEV_STATE_RELEASED;
            }
        }
    }
    data->key = keyValue;

    if (keyValue != 0 && matrixPipeline) {
        input_policy::InputEvent event{};
        event.sourceId = "TM9_keyboard";
        event.rawKeyCode = keyValue;
        event.resolvedKeyCode = keyValue;
        event.pressKind = (data->state == LV_INDEV_STATE_PRESSED)
                              ? (isSyntheticLongPress ? input_policy::PressKind::LongPress : input_policy::PressKind::Press)
                              : input_policy::PressKind::Release;
        event.timestampMs = millis();

        std::vector<input_policy::InputEvent> output;
        bool forward = matrixPipeline->process(event, TM9Capabilities, output);
        if (!forward || output.empty()) {
            // ILOG_DEBUG("[TM9-KeyMatrix] Pipeline consumed event, not forwarding");
            data->key = 0;
            data->state = LV_INDEV_STATE_RELEASED;
            return;
        }

        const auto &outEvent = output.front();
        uint32_t outKey = outEvent.resolvedKeyCode != 0 ? outEvent.resolvedKeyCode : outEvent.rawKeyCode;
        if (outKey == 0) {
            // ILOG_DEBUG("[TM9-KeyMatrix] No output key from pipeline");
            data->key = 0;
            data->state = LV_INDEV_STATE_RELEASED;
            return;
        }
        data->state = (outEvent.pressKind == input_policy::PressKind::Release) ? LV_INDEV_STATE_RELEASED : LV_INDEV_STATE_PRESSED;
#if 0
        const char *pressKindName = "PRESS";
        switch (outEvent.pressKind) {
        case input_policy::PressKind::Press:
            pressKindName = "PRESS";
            break;
        case input_policy::PressKind::Release:
            pressKindName = "RELEASE";
            break;
        case input_policy::PressKind::LongPress:
            pressKindName = "LONG_PRESS";
            break;
        case input_policy::PressKind::LongPressRepeat:
            pressKindName = "LONG_PRESS_REPEAT";
            break;
        }
        ILOG_DEBUG("[TM9-KeyMatrix] Pipeline output: key=0x%x kind=%s state=%s (remapped from 0x%x)", outKey,
                   pressKindName, data->state == LV_INDEV_STATE_PRESSED ? "PRESSED" : "RELEASED", data->key);
#endif
        data->key = outKey;
    }
}

#ifdef HAS_STC8H_KB
// ---------- STC8HKeyboardInputDriver Implementation ----------

#define STC8_REG_ADDR_MATRIX_KEY 0x05

#ifndef KB_INT
#define KB_INT 12
#endif

#ifndef PIN_LED
#define PIN_LED 13
#endif

#ifndef KB_LED
#define KB_LED 46
#endif

volatile bool STC8HKeyboardInputDriver::keyEvent = false;

const input_policy::InputCapabilities matrixCapabilities = {
    true,  // hasArrowKeys
    false, // hasTabKey
    false, // hasHomeKey
    false, // hasEndKey
    false, // hasPgUpPgDownKeys
    false, // hasCancelKey
    true,  // hasEnterKey
    true,  // hasModifiers
    false, // supportsLongPress
    false, // supportsRepeat
    true   // supportsTextEntry
};

STC8HKeyboardInputDriver::STC8HKeyboardInputDriver(uint8_t address, TwoWire &wire_) : wire(wire_)
{
    registerI2CKeyboard(this, "STC8H Keyboard", address);
}

void STC8HKeyboardInputDriver::init(void)
{
    I2CKeyboardInputDriver::init();

    pinMode(KB_INT, INPUT);
    pinMode(KB_LED, OUTPUT);
    attachInterrupt(
        KB_INT, [] { keyEvent = true; }, FALLING);

    if (!contextProvider) {
        contextProvider = std::shared_ptr<input_policy::IInputContextProvider>(&input_policy::InputContextState::instance(),
                                                                               [](input_policy::IInputContextProvider *) {});
    }
    if (!commandDispatcher) {
        commandDispatcher = std::shared_ptr<input_policy::IUICommandDispatcher>(&input_policy::UICommandDispatcher::instance(),
                                                                                [](input_policy::IUICommandDispatcher *) {});
    }
    if (!matrixPipeline) {
        // Use factory to build pipeline with capability-driven policy composition
        input_policy::InputSourceRegistry registry;
        input_policy::DefaultInputPolicyFactory factory;
        auto result = factory.build(registry, contextProvider, commandDispatcher);

        matrixPipeline =
            std::make_shared<input_policy::InputPipeline>(result.bindingResolver, contextProvider, commandDispatcher);
        matrixPipeline->setPolicyChain(std::move(result.chain));
    }
}

uint8_t STC8HKeyboardInputDriver::readRegister(uint8_t address, uint8_t reg)
{
    wire.beginTransmission(address);
    wire.write(reg);
    if (wire.endTransmission(false) != 0)
        return 0xFF;
    if (wire.requestFrom(address, (uint8_t)1) != 1)
        return 0xFF;
    return wire.read();
}

void STC8HKeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    uint32_t keyValue = 0;
    if (keyEvent) {
        keyEvent = false;
        uint8_t bytes = readRegister(address, STC8_REG_ADDR_MATRIX_KEY);
        if (bytes != 0xFF) {
            keyValue = bytes;
            data->state = LV_INDEV_STATE_PRESSED;
            ILOG_DEBUG("key press value: %d", keyValue);
            switch (keyValue) {
            case 0x0D:
                keyValue = LV_KEY_ENTER;
                break;
            case 0x82: // home
                keyValue = 0x100;
                break;
            case 0x83: // time -> chat
                keyValue = 0x101;
                break;
            case 0x85: // location -> map
                keyValue = 0x102;
                break;
            case 0x88: // light, speaker, ... (= bug)
                keyValue = 0;
                data->state = LV_INDEV_STATE_RELEASED;
                digitalWrite(KB_LED, !digitalRead(KB_LED));
                break;
            case 0xb5: // Up
                keyValue = LV_KEY_UP;
                break;
            case 0xb4: // Left
                keyValue = LV_KEY_LEFT;
                break;
            case 0xb6: // Down
                keyValue = LV_KEY_DOWN;
                break;
            case 0xb7: // Right
                keyValue = LV_KEY_RIGHT;
                break;
            default:
                break;
            }
        } else {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }
    data->key = (uint32_t)keyValue;

    if (data->key != 0 && matrixPipeline) {
        input_policy::InputEvent event{};
        event.sourceId = "stc8";
        event.rawKeyCode = data->key;
        event.resolvedKeyCode = data->key;
        event.pressKind =
            (data->state == LV_INDEV_STATE_PRESSED) ? input_policy::PressKind::Press : input_policy::PressKind::Release;
        event.timestampMs = millis();

        ILOG_DEBUG("[KeyMatrix] Raw event: key=0x%x state=%s", data->key,
                   data->state == LV_INDEV_STATE_PRESSED ? "PRESSED" : "RELEASED");

        std::vector<input_policy::InputEvent> output;
        bool forward = matrixPipeline->process(event, matrixCapabilities, output);
        if (!forward || output.empty()) {
            ILOG_DEBUG("[KeyMatrix] Pipeline consumed event, not forwarding");
            data->key = 0;
            data->state = LV_INDEV_STATE_RELEASED;
            return;
        }

        const auto &outEvent = output.front();
        uint32_t outKey = outEvent.resolvedKeyCode != 0 ? outEvent.resolvedKeyCode : outEvent.rawKeyCode;
        if (outKey == 0) {
            ILOG_DEBUG("[KeyMatrix] No output key from pipeline");
            data->key = 0;
            data->state = LV_INDEV_STATE_RELEASED;
            return;
        }

        ILOG_DEBUG("[KeyMatrix] Pipeline output: key=0x%x state=%s (remapped from 0x%x)", outKey,
                   outEvent.pressKind == input_policy::PressKind::Press ? "PRESSED" : "RELEASED", data->key);
        data->key = outKey;
    }
}
#endif