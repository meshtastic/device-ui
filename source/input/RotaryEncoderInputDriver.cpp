#ifdef INPUTDRIVER_ROTARY_TYPE

#include "input/RotaryEncoderInputDriver.h"
#include "input/I2CKeyboardInputDriver.h"
#include "RotaryEncoder.h"
#include "Arduino.h"
#include "util/ILog.h"

// Static member initialization
RotaryEncoder *RotaryEncoderInputDriver::rotary = nullptr;
volatile int16_t RotaryEncoderInputDriver::encoderDiff = 0;

RotaryEncoderInputDriver::RotaryEncoderInputDriver(void) {}

RotaryEncoderInputDriver::~RotaryEncoderInputDriver(void)
{
    if (rotary) {
        delete rotary;
        rotary = nullptr;
    }
}

void RotaryEncoderInputDriver::init(void)
{
    ILOG_DEBUG("RotaryEncoderInputDriver init...");

#if defined(INPUTDRIVER_ROTARY_UP) && defined(INPUTDRIVER_ROTARY_DOWN)
    // Create the RotaryEncoder instance
    // ROTARY_UP is pin A, ROTARY_DOWN is pin B in quadrature terms
#ifdef INPUTDRIVER_ROTARY_BTN
    rotary = new RotaryEncoder(INPUTDRIVER_ROTARY_UP, INPUTDRIVER_ROTARY_DOWN, INPUTDRIVER_ROTARY_BTN);
#else
    rotary = new RotaryEncoder(INPUTDRIVER_ROTARY_UP, INPUTDRIVER_ROTARY_DOWN);
#endif
#endif

    if (!rotary) {
        ILOG_ERROR("RotaryEncoderInputDriver: Failed to create RotaryEncoder - check pin definitions");
        return;
    }

    // Create LVGL encoder input device
    encoder = lv_indev_create();
    lv_indev_set_type(encoder, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder, encoder_read);

    // Create or use existing input group
    if (!inputGroup) {
        inputGroup = lv_group_create();
        lv_group_set_default(inputGroup);
    }
    lv_indev_set_group(encoder, inputGroup);

    ILOG_INFO("RotaryEncoderInputDriver initialized (pins A=%d, B=%d, BTN=%d)",
              INPUTDRIVER_ROTARY_UP, INPUTDRIVER_ROTARY_DOWN,
#ifdef INPUTDRIVER_ROTARY_BTN
              INPUTDRIVER_ROTARY_BTN
#else
              -1
#endif
    );
}

void RotaryEncoderInputDriver::task_handler(void)
{
    if (!rotary)
        return;

    // Process rotary encoder - must be called frequently for proper quadrature decoding
    RotaryEncoder::Direction dir = rotary->process();

    if (dir == RotaryEncoder::DIRECTION_CW) {
        encoderDiff++;
    } else if (dir == RotaryEncoder::DIRECTION_CCW) {
        encoderDiff--;
    }
}

void RotaryEncoderInputDriver::encoder_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (!rotary) {
        data->state = LV_INDEV_STATE_RELEASED;
        data->enc_diff = 0;
        return;
    }

    // Default state
    data->state = LV_INDEV_STATE_RELEASED;

    // Check if alt is held and we have encoder movement - scroll instead of navigate
    if (I2CKeyboardInputDriver::isAltModifierHeld() && encoderDiff != 0) {
        auto scrollCb = I2CKeyboardInputDriver::getScrollCallback();
        if (scrollCb) {
            // Positive diff = clockwise = scroll down, negative = counter-clockwise = scroll up
            scrollCb(encoderDiff > 0 ? 1 : -1);
            ILOG_DEBUG("Alt+encoder scroll: diff=%d", encoderDiff);
        }
        encoderDiff = 0;  // Consume the input
        data->enc_diff = 0;
        return;
    }

    // Normal encoder behavior - navigation
    data->enc_diff = encoderDiff;
    encoderDiff = 0;

    // Button handling
#ifdef INPUTDRIVER_ROTARY_BTN
    uint8_t btnState = rotary->readButton();

    if (btnState == RotaryEncoder::BUTTON_PRESSED) {
        data->key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PRESSED;
    } else if (btnState == RotaryEncoder::BUTTON_PRESSED_RELEASED) {
        // Button was pressed and released - reset for next press
        rotary->resetButton();
        data->state = LV_INDEV_STATE_RELEASED;
    }
#endif
}

#endif
