#ifdef INPUTDRIVER_ENCODER_TYPE

#include "EncoderInputDriver.h"
#include "Arduino.h"
#include "ILog.h"

volatile EncoderInputDriver::EncoderActionType EncoderInputDriver::action = TB_ACTION_NONE;

EncoderInputDriver::EncoderInputDriver(void) {}

void EncoderInputDriver::init(void)
{
    if (INPUTDRIVER_ENCODER_TYPE == 3) {
#ifdef INPUTDRIVER_ENCODER_LEFT
        pinMode(INPUTDRIVER_ENCODER_LEFT, INPUT_PULLUP);
        attachInterrupt(INPUTDRIVER_ENCODER_LEFT, intLeftHandler, RISING);
#endif
#ifdef INPUTDRIVER_ENCODER_RIGHT
        pinMode(INPUTDRIVER_ENCODER_RIGHT, INPUT_PULLUP);
        attachInterrupt(INPUTDRIVER_ENCODER_RIGHT, intRightHandler, RISING);
#endif
#ifdef INPUTDRIVER_ENCODER_UP
        pinMode(INPUTDRIVER_ENCODER_UP, INPUT_PULLUP);
        attachInterrupt(INPUTDRIVER_ENCODER_UP, intUpHandler, RISING);
#endif
#ifdef INPUTDRIVER_ENCODER_DOWN
        pinMode(INPUTDRIVER_ENCODER_DOWN, INPUT_PULLUP);
        attachInterrupt(INPUTDRIVER_ENCODER_DOWN, intDownHandler, RISING);
#endif
#ifdef INPUTDRIVER_ENCODER_BTN
        pinMode(INPUTDRIVER_ENCODER_BTN, INPUT_PULLUP);
        attachInterrupt(INPUTDRIVER_ENCODER_BTN, intPressHandler, RISING);
#endif
    }

    encoder = lv_indev_create();
    lv_indev_set_type(encoder, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder, encoder_read);

    if (!inputGroup) {
        inputGroup = lv_group_create();
        lv_group_set_default(inputGroup);
    }
    lv_indev_set_group(encoder, inputGroup);
}

void EncoderInputDriver::encoder_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    // encoder w/o interrupts but read GPIOs directly
    if (INPUTDRIVER_ENCODER_TYPE == 1) {
#ifdef INPUTDRIVER_ENCODER_LEFT
        if (digitalRead(INPUTDRIVER_ENCODER_LEFT))
            data->enc_diff = -1;
#endif
#ifdef INPUTDRIVER_ENCODER_RIGHT
        if (digitalRead(INPUTDRIVER_ENCODER_RIGHT))
            data->enc_diff = 1;
#endif
#ifdef INPUTDRIVER_ENCODER_BTN
        if (!digitalRead(INPUTDRIVER_ENCODER_BTN)) {
            data->key = LV_KEY_ENTER;
            data->state = LV_INDEV_STATE_PRESSED;
        }
#endif
    }
    // trackball with additional up/down inputs to control sliders
    else if (INPUTDRIVER_ENCODER_TYPE == 3) {
        if (action == TB_ACTION_NONE) {
            return;
        }

        data->state = LV_INDEV_STATE_RELEASED;
        data->enc_diff = 0;

        static uint32_t lastPressed = millis();
        if (millis() > lastPressed + 200) {
            if (action == TB_ACTION_PRESSED) {
                data->key = LV_KEY_ENTER;
                data->state = LV_INDEV_STATE_PRESSED;
            } else if (action == TB_ACTION_UP) {
                data->enc_diff = -1;
            } else if (action == TB_ACTION_DOWN) {
                data->enc_diff = 1;
            } else if (action == TB_ACTION_LEFT) {
                data->key = LV_KEY_DOWN;
                data->state = LV_INDEV_STATE_PRESSED;
            } else if (action == TB_ACTION_RIGHT) {
                data->key = LV_KEY_UP;
                data->state = LV_INDEV_STATE_PRESSED;
            }

            lastPressed = millis();
            action = TB_ACTION_NONE;
        }
    }
}

void EncoderInputDriver::intPressHandler()
{
    action = TB_ACTION_PRESSED;
}

void EncoderInputDriver::intDownHandler()
{
    action = TB_ACTION_DOWN;
}

void EncoderInputDriver::intUpHandler()
{
    action = TB_ACTION_UP;
}

void EncoderInputDriver::intLeftHandler()
{
    action = TB_ACTION_LEFT;
}

void EncoderInputDriver::intRightHandler()
{
    action = TB_ACTION_RIGHT;
}

#endif