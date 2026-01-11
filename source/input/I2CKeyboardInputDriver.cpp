
#include "input/I2CKeyboardInputDriver.h"
#include "util/ILog.h"
#include <Arduino.h>
#include <Wire.h>

#include "indev/lv_indev_private.h"
#include "widgets/textarea/lv_textarea.h"

I2CKeyboardInputDriver::KeyboardList I2CKeyboardInputDriver::i2cKeyboardList;
NavigationCallback I2CKeyboardInputDriver::navigateHomeCallback = nullptr;
bool I2CKeyboardInputDriver::altModifierHeld = false;
ScrollCallback I2CKeyboardInputDriver::scrollCallback = nullptr;

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

// ---------- TCA8418 Register Definitions ----------
#define TCA8418_REG_CFG 0x01
#define TCA8418_REG_INT_STAT 0x02
#define TCA8418_REG_KEY_LCK_EC 0x03
#define TCA8418_REG_KEY_EVENT_A 0x04
#define TCA8418_REG_KP_GPIO_1 0x1D
#define TCA8418_REG_KP_GPIO_2 0x1E
#define TCA8418_REG_KP_GPIO_3 0x1F
#define TCA8418_REG_GPIO_DIR_1 0x23
#define TCA8418_REG_GPIO_DIR_2 0x24
#define TCA8418_REG_GPIO_DIR_3 0x25
#define TCA8418_REG_GPI_EM_1 0x20
#define TCA8418_REG_GPI_EM_2 0x21
#define TCA8418_REG_GPI_EM_3 0x22
#define TCA8418_REG_GPIO_INT_LVL_1 0x26
#define TCA8418_REG_GPIO_INT_LVL_2 0x27
#define TCA8418_REG_GPIO_INT_LVL_3 0x28
#define TCA8418_REG_GPIO_INT_EN_1 0x1A
#define TCA8418_REG_GPIO_INT_EN_2 0x1B
#define TCA8418_REG_GPIO_INT_EN_3 0x1C
#define TCA8418_REG_DEBOUNCE_DIS_1 0x29
#define TCA8418_REG_DEBOUNCE_DIS_2 0x2A
#define TCA8418_REG_DEBOUNCE_DIS_3 0x2B

// Helper to write a register
static void tca8418WriteReg(uint8_t address, uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

// Helper to read a register
static uint8_t tca8418ReadReg(uint8_t address, uint8_t reg)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(address, (uint8_t)1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}

// ---------- TCA8418KeyboardInputDriver Implementation ----------

static uint8_t tca8418Address = 0x34;

TCA8418KeyboardInputDriver::TCA8418KeyboardInputDriver(uint8_t address)
{
    tca8418Address = address;
    registerI2CKeyboard(this, "TCA8418 Keyboard", address);
}

void TCA8418KeyboardInputDriver::init(void)
{
    I2CKeyboardInputDriver::init();

    // Initialize TCA8418 - set up keyboard matrix
    ILOG_DEBUG("TCA8418 init at address 0x%02X", tca8418Address);

    // Set all GPIO pins to input
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPIO_DIR_1, 0x00);
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPIO_DIR_2, 0x00);
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPIO_DIR_3, 0x00);

    // Add all pins to key events
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPI_EM_1, 0xFF);
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPI_EM_2, 0xFF);
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPI_EM_3, 0xFF);

    // Set all pins to falling edge interrupts
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPIO_INT_LVL_1, 0x00);
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPIO_INT_LVL_2, 0x00);
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPIO_INT_LVL_3, 0x00);

    // Enable interrupts for all pins
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPIO_INT_EN_1, 0xFF);
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPIO_INT_EN_2, 0xFF);
    tca8418WriteReg(tca8418Address, TCA8418_REG_GPIO_INT_EN_3, 0xFF);

    // Enable debounce
    tca8418WriteReg(tca8418Address, TCA8418_REG_DEBOUNCE_DIS_1, 0x00);
    tca8418WriteReg(tca8418Address, TCA8418_REG_DEBOUNCE_DIS_2, 0x00);
    tca8418WriteReg(tca8418Address, TCA8418_REG_DEBOUNCE_DIS_3, 0x00);

    // Flush any pending key events
    while (tca8418ReadReg(tca8418Address, TCA8418_REG_KEY_EVENT_A) != 0) {
        // Keep reading until FIFO is empty
    }

    // Clear interrupt status
    tca8418WriteReg(tca8418Address, TCA8418_REG_INT_STAT, 0x03);

    // Enable key event interrupt (critical for key FIFO to work)
    uint8_t cfg = tca8418ReadReg(tca8418Address, TCA8418_REG_CFG);
    cfg |= 0x01; // KE_IEN - Key events interrupt enable
    tca8418WriteReg(tca8418Address, TCA8418_REG_CFG, cfg);

    ILOG_INFO("TCA8418 keyboard initialized (CFG=0x%02X)", cfg);
}

void TCA8418KeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    data->state = LV_INDEV_STATE_RELEASED;
    data->key = 0;

    // Read key count from KEY_LCK_EC register (bits 0-3)
    Wire.beginTransmission(address);
    Wire.write(TCA8418_REG_KEY_LCK_EC);
    Wire.endTransmission();
    Wire.requestFrom(address, (uint8_t)1);
    if (Wire.available()) {
        uint8_t keyCount = Wire.read() & 0x0F;
        if (keyCount > 0) {
            // Read key event from FIFO
            Wire.beginTransmission(address);
            Wire.write(TCA8418_REG_KEY_EVENT_A);
            Wire.endTransmission();
            Wire.requestFrom(address, (uint8_t)1);
            if (Wire.available()) {
                uint8_t keyEvent = Wire.read();
                uint8_t keyCode = keyEvent & 0x7F;
                bool pressed = (keyEvent & 0x80) != 0;

                if (pressed && keyCode > 0) {
                    data->state = LV_INDEV_STATE_PRESSED;
                    data->key = keyCode; // Will be mapped by subclass
                    ILOG_DEBUG("TCA8418 key event: code=%d pressed=%d", keyCode, pressed);
                }
            }
        }
    }
}

// ---------- TLoraPagerKeyboardInputDriver Implementation ----------

// T-Pager keyboard layout: 4 rows x 10 columns = 31 keys
// Key mapping from TCA8418 key codes to characters [normal, shift, sym]
static const char TLoraPagerKeyMap[31][3] = {
    {'q', 'Q', '1'},  // Key 1
    {'w', 'W', '2'},  // Key 2
    {'e', 'E', '3'},  // Key 3
    {'r', 'R', '4'},  // Key 4
    {'t', 'T', '5'},  // Key 5
    {'y', 'Y', '6'},  // Key 6
    {'u', 'U', '7'},  // Key 7
    {'i', 'I', '8'},  // Key 8
    {'o', 'O', '9'},  // Key 9
    {'p', 'P', '0'},  // Key 10
    {'a', 'A', '*'},  // Key 11
    {'s', 'S', '/'},  // Key 12
    {'d', 'D', '+'},  // Key 13
    {'f', 'F', '-'},  // Key 14
    {'g', 'G', '='},  // Key 15
    {'h', 'H', ':'},  // Key 16
    {'j', 'J', '\''},  // Key 17
    {'k', 'K', '"'},  // Key 18
    {'l', 'L', '@'},  // Key 19
    {0x0D, 0x09, 0x0D}, // Key 20: Enter, Tab (shift), Enter (sym)
    {0, 0, 0},        // Key 21: Sym modifier (no output)
    {'z', 'Z', '_'},  // Key 22
    {'x', 'X', '$'},  // Key 23
    {'c', 'C', ';'},  // Key 24
    {'v', 'V', '?'},  // Key 25
    {'b', 'B', '!'},  // Key 26
    {'n', 'N', ','},  // Key 27
    {'m', 'M', '.'},  // Key 28
    {0, 0, 0},        // Key 29: Shift modifier (no output)
    {0x08, 0x08, 0x1B}, // Key 30: Backspace, Backspace (shift), ESC (sym)
    {' ', ' ', ' '}   // Key 31: Space
};

// Modifier key indices (0-based)
static const uint8_t MODIFIER_SYM_KEY = 20;    // Key 21
static const uint8_t MODIFIER_SHIFT_KEY = 28;  // Key 29

// Modifier state (sticky toggles)
static uint8_t modifierState = 0;  // 0=normal, 1=shift, 2=sym

TLoraPagerKeyboardInputDriver::TLoraPagerKeyboardInputDriver(uint8_t address) : TCA8418KeyboardInputDriver(address)
{
    // Don't register here - parent TCA8418KeyboardInputDriver already registers
}

void TLoraPagerKeyboardInputDriver::init(void)
{
    TCA8418KeyboardInputDriver::init();

    // Set up T-Pager keyboard matrix: 4 rows x 10 columns
    // Rows 0-3 (bits 0-3 in KP_GPIO_1)
    tca8418WriteReg(tca8418Address, TCA8418_REG_KP_GPIO_1, 0x0F);
    // Columns 0-7 (bits 0-7 in KP_GPIO_2)
    tca8418WriteReg(tca8418Address, TCA8418_REG_KP_GPIO_2, 0xFF);
    // Columns 8-9 (bits 0-1 in KP_GPIO_3)
    tca8418WriteReg(tca8418Address, TCA8418_REG_KP_GPIO_3, 0x03);

    ILOG_INFO("TLoraPagerKeyboardInputDriver initialized (4x10 matrix)");
}

static uint32_t lastKeyboardLog = 0;

void TLoraPagerKeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    data->state = LV_INDEV_STATE_RELEASED;
    data->key = 0;

    // Read key count from KEY_LCK_EC register (bits 0-3)
    Wire.beginTransmission(address);
    Wire.write(TCA8418_REG_KEY_LCK_EC);
    Wire.endTransmission();
    Wire.requestFrom(address, (uint8_t)1);
    if (Wire.available()) {
        uint8_t keyCount = Wire.read() & 0x0F;

        // Log periodically to show we're polling
        uint32_t now = millis();
        if (now - lastKeyboardLog > 5000) {
            ILOG_DEBUG("T-Pager keyboard poll: keyCount=%d mod=%d", keyCount, modifierState);
            lastKeyboardLog = now;
        }
        if (keyCount > 0) {
            // Read key event from FIFO
            Wire.beginTransmission(address);
            Wire.write(TCA8418_REG_KEY_EVENT_A);
            Wire.endTransmission();
            Wire.requestFrom(address, (uint8_t)1);
            if (Wire.available()) {
                uint8_t keyEvent = Wire.read();
                uint8_t keyCode = keyEvent & 0x7F;
                bool pressed = (keyEvent & 0x80) != 0;

                if (pressed && keyCode > 0 && keyCode <= 31) {
                    uint8_t keyIndex = keyCode - 1;

                    // Check for modifier keys
                    if (keyIndex == MODIFIER_SHIFT_KEY) {
                        // Toggle shift modifier
                        modifierState = (modifierState == 1) ? 0 : 1;
                        ILOG_DEBUG("T-Pager: Shift toggled, modifierState=%d", modifierState);
                        return;  // Don't output a key for modifier press
                    }
                    if (keyIndex == MODIFIER_SYM_KEY) {
                        // Toggle sym modifier
                        modifierState = (modifierState == 2) ? 0 : 2;
                        // Also update the class-accessible ALT state for scroll-while-typing
                        I2CKeyboardInputDriver::setAltModifierHeld(modifierState == 2);
                        ILOG_DEBUG("T-Pager: Sym toggled, modifierState=%d altHeld=%d", modifierState, modifierState == 2);
                        return;  // Don't output a key for modifier press
                    }

                    // Get character based on modifier state
                    char keyChar = TLoraPagerKeyMap[keyIndex][modifierState];

                    if (keyChar != 0) {
                        data->state = LV_INDEV_STATE_PRESSED;

                        // Map special keys to LVGL key codes
                        switch (keyChar) {
                        case 0x0D: // Enter
                            data->key = LV_KEY_ENTER;
                            break;
                        case 0x09: // Tab
                            data->key = LV_KEY_NEXT;
                            break;
                        case 0x08: // Backspace
                            {
                                // Check if focused object is a textarea
                                lv_obj_t *focused = lv_group_get_focused(lv_group_get_default());
                                if (focused && lv_obj_check_type(focused, &lv_textarea_class)) {
                                    // In textarea - send backspace as normal
                                    data->key = LV_KEY_BACKSPACE;
                                } else {
                                    // Not in textarea - call navigation callback to focus home button
                                    if (I2CKeyboardInputDriver::navigateHomeCallback) {
                                        I2CKeyboardInputDriver::navigateHomeCallback();
                                        data->state = LV_INDEV_STATE_RELEASED;
                                        ILOG_DEBUG("Backspace: called navigateHomeCallback");
                                        return;
                                    }
                                    // Fallback to ESC if no callback registered
                                    data->key = LV_KEY_ESC;
                                }
                            }
                            break;
                        case 0x1B: // ESC
                            data->key = LV_KEY_ESC;
                            break;
                        default:
                            data->key = (uint32_t)keyChar;
                            break;
                        }
                        ILOG_DEBUG("T-Pager key: code=%d mod=%d char='%c' lvkey=%d", keyCode, modifierState, keyChar, data->key);

                        // Clear modifier after a regular key press (one-shot behavior)
                        modifierState = 0;
                    }
                }
            }
        }
    }
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

CardKBInputDriver::CardKBInputDriver(uint8_t address)
{
    registerI2CKeyboard(this, "Card Keyboard", address);
}

void CardKBInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    char keyValue = 0;
    Wire.requestFrom(address, 1);
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
    // TODO
    char keyValue = 0;
    data->state = LV_INDEV_STATE_RELEASED;
    data->key = (uint32_t)keyValue;
}
