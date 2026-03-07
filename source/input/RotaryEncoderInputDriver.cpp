#ifdef INPUTDRIVER_ROTARY_TYPE

#include "input/RotaryEncoderInputDriver.h"
#include "Arduino.h"
#include "RotaryEncoder.h"
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

    // Register as LVGL encoder input device
    encoder = lv_indev_create();
    lv_indev_set_type(encoder, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder, encoder_read);

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

    data->state = LV_INDEV_STATE_RELEASED;
    data->enc_diff = encoderDiff;
    encoderDiff = 0;

#ifdef INPUTDRIVER_ROTARY_BTN
    uint8_t btnState = rotary->readButton();
    if (btnState == RotaryEncoder::BUTTON_PRESSED) {
        data->key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PRESSED;
    } else if (btnState == RotaryEncoder::BUTTON_PRESSED_RELEASED) {
        rotary->resetButton();
        data->state = LV_INDEV_STATE_RELEASED;
    }
#endif
}

#endif
