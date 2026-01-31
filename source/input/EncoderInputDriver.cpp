#ifdef INPUTDRIVER_ENCODER_TYPE

#include "input/EncoderInputDriver.h"
#include "Arduino.h"
#include "util/ILog.h"

volatile EncoderInputDriver::EncoderActionType EncoderInputDriver::action = TB_ACTION_NONE;

EncoderInputDriver::EncoderInputDriver(void) {}

void EncoderInputDriver::init(void)
{
    LOG_INFO("Initialize encoder input driver type %d", INPUTDRIVER_ENCODER_TYPE);
    if (INPUTDRIVER_ENCODER_TYPE == 1) {
#ifdef INPUTDRIVER_ENCODER_LEFT
        pinMode(INPUTDRIVER_ENCODER_LEFT, INPUT_PULLUP);
#endif
#ifdef INPUTDRIVER_ENCODER_RIGHT
        pinMode(INPUTDRIVER_ENCODER_RIGHT, INPUT_PULLUP);
#endif
#ifdef INPUTDRIVER_ENCODER_BTN
        pinMode(INPUTDRIVER_ENCODER_BTN, INPUT);
#endif
    }
    // two buttons
    else if (INPUTDRIVER_ENCODER_TYPE == 2) {
#ifdef INPUTDRIVER_ENCODER_LEFT
#ifdef INPUTDRIVER_ENCODER_LEFT_PULLUP
        pinMode(INPUTDRIVER_ENCODER_LEFT, INPUT_PULLUP);
#else
        pinMode(INPUTDRIVER_ENCODER_LEFT, INPUT);
#endif
        attachInterrupt(INPUTDRIVER_ENCODER_LEFT, intLeftHandler, RISING);
#endif
#ifdef INPUTDRIVER_ENCODER_RIGHT
#ifdef INPUTDRIVER_ENCODER_RIGHT_PULLUP
        pinMode(INPUTDRIVER_ENCODER_RIGHT, INPUT_PULLUP);
#else
        pinMode(INPUTDRIVER_ENCODER_RIGHT, INPUT);
#endif
        attachInterrupt(INPUTDRIVER_ENCODER_RIGHT, intRightHandler, RISING);
#endif
#ifdef INPUTDRIVER_ENCODER_BTN
#ifdef INPUTDRIVER_ENCODER_BTN_PULLUP
        pinMode(INPUTDRIVER_ENCODER_BTN, INPUT_PULLUP);
#else
        pinMode(INPUTDRIVER_ENCODER_BTN, INPUT);
#endif
#endif
#ifdef INPUTDRIVER_ENCODER_ESC
        pinMode(INPUTDRIVER_ENCODER_ESC, INPUT);
#endif
    }
    // trackball or joystick type encoder with four directions
    else if (INPUTDRIVER_ENCODER_TYPE == 3) {
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
        pinMode(INPUTDRIVER_ENCODER_BTN, INPUT);
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
    data->key = 0;
    data->enc_diff = 0;
    data->state = LV_INDEV_STATE_RELEASED;

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
        // FIXME: need same logix as below to trigger LONG_PRESSED events
        if (!digitalRead(INPUTDRIVER_ENCODER_BTN)) {
            data->key = LV_KEY_ENTER;
            data->state = LV_INDEV_STATE_PRESSED;
        }
#endif
    }
    // interrupt driven
    else if (INPUTDRIVER_ENCODER_TYPE == 2) {
        static uint32_t prevkey = 0;
#ifdef INPUTDRIVER_ENCODER_LEFT
        if (action == TB_ACTION_LEFT) {
            data->enc_diff -= 1;
        }
#endif
#ifdef INPUTDRIVER_ENCODER_RIGHT
        if (action == TB_ACTION_RIGHT) {
            data->enc_diff += 1;
        }
#endif
#ifdef INPUTDRIVER_ENCODER_BTN
        static uint32_t pressedDurationBtn = 0;
        if (!digitalRead(INPUTDRIVER_ENCODER_BTN)) {
            if (!pressedDurationBtn)
                pressedDurationBtn = millis();
            else if (millis() > pressedDurationBtn + 400) {
                ILOG_DEBUG("encoder btn pressed");
                action = TB_ACTION_PRESSED;
                data->key = LV_KEY_ENTER;
                data->state = LV_INDEV_STATE_PRESSED;
                pressedDurationBtn = 0;
            }
            //            else if (millis() > pressedDurationBtn + 10) {
            //                data->key = LV_KEY_RIGHT;
            //                data->state = LV_INDEV_STATE_PRESSED;
            //            }
        } else {
            pressedDurationBtn = 0;
        }
#endif
#ifdef INPUTDRIVER_ENCODER_ESC
        static uint32_t pressedDurationEsc = 0;
        if (!digitalRead(INPUTDRIVER_ENCODER_ESC)) {
            if (!pressedDurationEsc)
                pressedDurationEsc = millis();
            else if (millis() > pressedDurationEsc + 400) {
                ILOG_DEBUG("encoder esc pressed");
                action = TB_ACTION_ESC;
                data->key = LV_KEY_ESC;
                data->state = LV_INDEV_STATE_PRESSED;
                pressedDurationEsc = 0;
            }
        } else {
            pressedDurationEsc = 0;
        }
#endif
        static uint32_t lastPressed = 0;
        if (action != TB_ACTION_NONE && (millis() > lastPressed + 200)) {
            ILOG_DEBUG("encoder action %d", action);
            lastPressed = millis();
            prevkey = data->key;
            action = TB_ACTION_NONE;
        } else {
            // this logic is required for LONG_PRESSED event, see lv_indev.c
            if (prevkey != 0) {
                data->state = LV_INDEV_STATE_RELEASED;
                data->key = prevkey;
                prevkey = 0;
            }
        }
    }
    // trackball/joystick with additional up/down inputs to control sliders
    else if (INPUTDRIVER_ENCODER_TYPE == 3) {
        static uint32_t prevkey = 0;
        static uint32_t lastPressed = millis();

        data->key = 0;
        data->enc_diff = 0;
        data->state = LV_INDEV_STATE_RELEASED;

#ifdef INPUTDRIVER_ENCODER_BTN
        if (action == TB_ACTION_NONE) {
            if (!digitalRead(INPUTDRIVER_ENCODER_BTN)) {
                action = TB_ACTION_PRESSED;
            }
        }
#endif
        // slow down repeating key to max. four events per second
        // the button is an exception for LONG_PRESSED monitoring
        if (action != TB_ACTION_NONE && (action == TB_ACTION_PRESSED || millis() > lastPressed + 250)) {
            if (action == TB_ACTION_PRESSED) {
                data->key = LV_KEY_ENTER;
                data->state = LV_INDEV_STATE_PRESSED;
            } else if (action == TB_ACTION_UP) {
                data->enc_diff = -1;
            } else if (action == TB_ACTION_DOWN) {
                data->enc_diff = 1;
            } else if (action == TB_ACTION_LEFT) {
                data->key = LV_KEY_DOWN; // slider widget reacts on UP/DOWN
                data->state = LV_INDEV_STATE_PRESSED;
            } else if (action == TB_ACTION_RIGHT) {
                data->key = LV_KEY_UP; // slider widget reacts on UP/DOWN
                data->state = LV_INDEV_STATE_PRESSED;
            }

            lastPressed = millis();
            prevkey = data->key;
            action = TB_ACTION_NONE;
        } else {
            // this logic is required for LONG_PRESSED event, see lv_indev.c
            if (prevkey != 0) {
                data->state = LV_INDEV_STATE_RELEASED;
                data->key = prevkey;
                prevkey = 0;
            }
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