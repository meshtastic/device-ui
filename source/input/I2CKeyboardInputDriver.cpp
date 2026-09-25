
#include "input/I2CKeyboardInputDriver.h"
#include "util/ILog.h"
#ifdef T_DECK
#include "ui.h"
#endif
#include <Arduino.h>
#include <Wire.h>

#include "indev/lv_indev_private.h"

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

#ifdef T_DECK

static bool tdeckGreekLayout = false;

enum GreekDiacriticState : uint8_t {
    GREEK_DIACRITIC_NONE = 0,
    GREEK_DIACRITIC_TONOS = 1,
    GREEK_DIACRITIC_DIALYTIKA = 2,
    GREEK_DIACRITIC_TONOS_DIALYTIKA = 3,
};

static GreekDiacriticState tdeckGreekDiacriticState = GREEK_DIACRITIC_NONE;
static lv_obj_t *tdeckLayoutLabel = nullptr;

static void tdeckClearGreekDiacritics()
{
    tdeckGreekDiacriticState = GREEK_DIACRITIC_NONE;
}

static void tdeckAddGreekDiacritic(GreekDiacriticState state)
{
    tdeckGreekDiacriticState = static_cast<GreekDiacriticState>(
        static_cast<uint8_t>(tdeckGreekDiacriticState) | static_cast<uint8_t>(state));
}

static void tdeckSyncLayoutIndicatorStyle()
{
    if (!tdeckLayoutLabel || !objects.top_messages_node_label)
        return;

    const lv_font_t *font = lv_obj_get_style_text_font(objects.top_messages_node_label, LV_PART_MAIN);
    if (font)
        lv_obj_set_style_text_font(tdeckLayoutLabel, font, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(tdeckLayoutLabel,
                                lv_obj_get_style_text_color(objects.top_messages_node_label, LV_PART_MAIN),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void tdeckUpdateLayoutIndicator(lv_indev_t *indev)
{
    if (!tdeckLayoutLabel) {
        tdeckLayoutLabel = lv_label_create(lv_layer_top());
        lv_obj_set_style_text_font(tdeckLayoutLabel, &ui_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(tdeckLayoutLabel, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align(tdeckLayoutLabel, LV_ALIGN_TOP_RIGHT, -60, 2);
        lv_obj_remove_flag(tdeckLayoutLabel, LV_OBJ_FLAG_CLICKABLE);
    }

    lv_group_t *group = lv_indev_get_group(indev);
    lv_obj_t *focused = group ? lv_group_get_focused(group) : nullptr;
    bool textareaFocused = focused && lv_obj_check_type(focused, &lv_textarea_class);

    if (!textareaFocused) {
        lv_obj_add_flag(tdeckLayoutLabel, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_label_set_text(tdeckLayoutLabel, tdeckGreekLayout ? "EL" : "EN");
    tdeckSyncLayoutIndicatorStyle();
    lv_obj_remove_flag(tdeckLayoutLabel, LV_OBJ_FLAG_HIDDEN);
}

static uint32_t greekUtf8(uint32_t codepoint)
{
    return (0xC0 | (codepoint >> 6)) | ((0x80 | (codepoint & 0x3f)) << 8);
}

static uint32_t mapGreekKey(uint8_t key)
{
    switch (key) {
    case 'a': return greekUtf8(0x03B1); // α
    case 'b': return greekUtf8(0x03B2); // β
    case 'c': return greekUtf8(0x03C8); // ψ
    case 'd': return greekUtf8(0x03B4); // δ
    case 'e': return greekUtf8(0x03B5); // ε
    case 'f': return greekUtf8(0x03C6); // φ
    case 'g': return greekUtf8(0x03B3); // γ
    case 'h': return greekUtf8(0x03B7); // η
    case 'i': return greekUtf8(0x03B9); // ι
    case 'j': return greekUtf8(0x03BE); // ξ
    case 'k': return greekUtf8(0x03BA); // κ
    case 'l': return greekUtf8(0x03BB); // λ
    case 'm': return greekUtf8(0x03BC); // μ
    case 'n': return greekUtf8(0x03BD); // ν
    case 'o': return greekUtf8(0x03BF); // ο
    case 'p': return greekUtf8(0x03C0); // π
    case 'q': return ';';
    case 'r': return greekUtf8(0x03C1); // ρ
    case 's': return greekUtf8(0x03C3); // σ
    case 't': return greekUtf8(0x03C4); // τ
    case 'u': return greekUtf8(0x03B8); // θ
    case 'v': return greekUtf8(0x03C9); // ω
    case 'w': return greekUtf8(0x03C2); // ς
    case 'x': return greekUtf8(0x03C7); // χ
    case 'y': return greekUtf8(0x03C5); // υ
    case 'z': return greekUtf8(0x03B6); // ζ

    case 'A': return greekUtf8(0x0391); // Α
    case 'B': return greekUtf8(0x0392); // Β
    case 'C': return greekUtf8(0x03A8); // Ψ
    case 'D': return greekUtf8(0x0394); // Δ
    case 'E': return greekUtf8(0x0395); // Ε
    case 'F': return greekUtf8(0x03A6); // Φ
    case 'G': return greekUtf8(0x0393); // Γ
    case 'H': return greekUtf8(0x0397); // Η
    case 'I': return greekUtf8(0x0399); // Ι
    case 'J': return greekUtf8(0x039E); // Ξ
    case 'K': return greekUtf8(0x039A); // Κ
    case 'L': return greekUtf8(0x039B); // Λ
    case 'M': return greekUtf8(0x039C); // Μ
    case 'N': return greekUtf8(0x039D); // Ν
    case 'O': return greekUtf8(0x039F); // Ο
    case 'P': return greekUtf8(0x03A0); // Π
    case 'Q': return ':';
    case 'R': return greekUtf8(0x03A1); // Ρ
    case 'S': return greekUtf8(0x03A3); // Σ
    case 'T': return greekUtf8(0x03A4); // Τ
    case 'U': return greekUtf8(0x0398); // Θ
    case 'V': return greekUtf8(0x03A9); // Ω
    case 'W': return greekUtf8(0x03A3); // Σ
    case 'X': return greekUtf8(0x03A7); // Χ
    case 'Y': return greekUtf8(0x03A5); // Υ
    case 'Z': return greekUtf8(0x0396); // Ζ
    default: return key;
    }
}

static uint32_t mapGreekDiacriticKey(uint8_t key, GreekDiacriticState state)
{
    if (state == GREEK_DIACRITIC_TONOS) {
        switch (key) {
        case 'a': return greekUtf8(0x03AC); // ά
        case 'e': return greekUtf8(0x03AD); // έ
        case 'h': return greekUtf8(0x03AE); // ή
        case 'i': return greekUtf8(0x03AF); // ί
        case 'o': return greekUtf8(0x03CC); // ό
        case 'y': return greekUtf8(0x03CD); // ύ
        case 'v': return greekUtf8(0x03CE); // ώ
        case 'A': return greekUtf8(0x0386); // Ά
        case 'E': return greekUtf8(0x0388); // Έ
        case 'H': return greekUtf8(0x0389); // Ή
        case 'I': return greekUtf8(0x038A); // Ί
        case 'O': return greekUtf8(0x038C); // Ό
        case 'Y': return greekUtf8(0x038E); // Ύ
        case 'V': return greekUtf8(0x038F); // Ώ
        default: return 0;
        }
    }

    if (state == GREEK_DIACRITIC_DIALYTIKA) {
        switch (key) {
        case 'i': return greekUtf8(0x03CA); // ϊ
        case 'y': return greekUtf8(0x03CB); // ϋ
        case 'I': return greekUtf8(0x03AA); // Ϊ
        case 'Y': return greekUtf8(0x03AB); // Ϋ
        default: return 0;
        }
    }

    if (state == GREEK_DIACRITIC_TONOS_DIALYTIKA) {
        switch (key) {
        case 'i': return greekUtf8(0x0390); // ΐ
        case 'y': return greekUtf8(0x03B0); // ΰ
        case 'I': return greekUtf8(0x03AA); // Ϊ
        case 'Y': return greekUtf8(0x03AB); // Ϋ
        default: return 0;
        }
    }

    return 0;
}

static bool tdeckReadRawMatrix(uint8_t address, uint8_t raw[5])
{
    for (uint8_t i = 0; i < 5; i++)
        raw[i] = 0;

    Wire.beginTransmission(address);
    Wire.write((uint8_t)0x03); // raw mode
    if (Wire.endTransmission() != 0)
        return false;

    uint8_t bytes = Wire.requestFrom(address, (uint8_t)5);
    uint8_t count = 0;

    while (Wire.available() && count < 5)
        raw[count++] = Wire.read();

    while (Wire.available())
        (void)Wire.read();

    Wire.beginTransmission(address);
    Wire.write((uint8_t)0x04); // key mode
    Wire.endTransmission();

    return bytes == 5 && count == 5;
}

void TDeckKeyboardInputDriver::readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data)
{
    uint8_t rawKey = 0;
    uint32_t keyValue = 0;

    data->state = LV_INDEV_STATE_RELEASED;
    data->key = 0;

    tdeckUpdateLayoutIndicator(indev);

    uint8_t bytes = Wire.requestFrom(address, (uint8_t)1);
    if (Wire.available() > 0 && bytes > 0) {
        rawKey = Wire.read();

        // Ignore empty reads and shift-0.
        if (rawKey != 0x00 && rawKey != 0xE0) {
            ILOG_DEBUG("TDeck raw key: 0x%02X (%u)", rawKey, rawKey);

            // Alt+Space is indistinguishable from Space in normal key mode,
            // so inspect the raw matrix only when Space is received.
            if (rawKey == 0x20) {
                uint8_t raw[5] = {};

                if (tdeckReadRawMatrix(address, raw)) {
                    bool altPressed = (raw[0] & 0x10) != 0;
                    bool spacePressed = (raw[0] & 0x20) != 0;

                    if (altPressed && spacePressed) {
                        tdeckGreekLayout = !tdeckGreekLayout;
                        tdeckClearGreekDiacritics();
                        tdeckUpdateLayoutIndicator(indev);
                        ILOG_DEBUG("TDeck keyboard layout: %s", tdeckGreekLayout ? "EL" : "EN");
                        return;
                    }
                }
            }

            // On the T-Deck controller '$' is the physical key between M and Enter.
            // In Greek mode it becomes a tonos dead key.
            if (tdeckGreekLayout && rawKey == '$') {
                tdeckAddGreekDiacritic(GREEK_DIACRITIC_TONOS);
                return;
            }

            // The keyboard controller implements Shift by subtracting 32 from
            // the ASCII value, therefore Shift+$ is received as 0x04.
            if (tdeckGreekLayout && rawKey == 0x04) {
                tdeckAddGreekDiacritic(GREEK_DIACRITIC_DIALYTIKA);
                return;
            }

            data->state = LV_INDEV_STATE_PRESSED;

            if (rawKey == 0x0D) {
                tdeckClearGreekDiacritics();
                keyValue = LV_KEY_ENTER;
            } else if (tdeckGreekLayout) {
                if (tdeckGreekDiacriticState != GREEK_DIACRITIC_NONE) {
                    GreekDiacriticState pending = tdeckGreekDiacriticState;
                    tdeckClearGreekDiacritics();

                    keyValue = mapGreekDiacriticKey(rawKey, pending);
                    if (!keyValue)
                        keyValue = mapGreekKey(rawKey);
                } else {
                    keyValue = mapGreekKey(rawKey);
                }
            } else {
                tdeckClearGreekDiacritics();

                // Avoid passing the controller's Shift+$ control character
                // through to LVGL in English mode.
                keyValue = rawKey == 0x04 ? '$' : rawKey;
            }
        }
    }

    data->key = keyValue;
}


#else

// Keep the original implementation for non-T-Deck builds.
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

#endif // T_DECK

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
