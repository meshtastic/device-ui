#ifdef INPUTDRIVER_ROTARY_TYPE

#include "input/RotaryInputDriver.h"
#include "Arduino.h"
#include "util/ILog.h"
#include <RotaryEncoder.h>

RotaryEncoder *RotaryInputDriver::rotary = nullptr;
QueueHandle_t RotaryInputDriver::inputEventQueue = nullptr;

RotaryInputDriver::RotaryInputDriver(void) {}

void RotaryInputDriver::init(void)
{
    // up/down fsm encoder
    if (INPUTDRIVER_ROTARY_TYPE == 1) {
        ILOG_DEBUG("creating RotaryEncoder(%d,%d,%d)", INPUTDRIVER_ROTARY_UP, INPUTDRIVER_ROTARY_DOWN, INPUTDRIVER_ROTARY_BTN);
        rotary = new RotaryEncoder(INPUTDRIVER_ROTARY_UP, INPUTDRIVER_ROTARY_DOWN, INPUTDRIVER_ROTARY_BTN);
    }
    inputEventQueue = xQueueCreate(10, sizeof(RotaryActionType));

#ifdef INPUTDRIVER_ROTARY_UP
    attachInterrupt(INPUTDRIVER_ROTARY_UP, intHandler, CHANGE);
#endif
#ifdef INPUTDRIVER_ROTARY_DOWN
    attachInterrupt(INPUTDRIVER_ROTARY_DOWN, intHandler, CHANGE);
#endif
#ifdef INPUTDRIVER_ROTARY_BTN
    attachInterrupt(INPUTDRIVER_ROTARY_BTN, intPressHandler, CHANGE);
#endif

    encoder = lv_indev_create();
    lv_indev_set_type(encoder, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(encoder, encoder_read);

    if (!inputGroup) {
        inputGroup = lv_group_create();
        lv_group_set_default(inputGroup);
    }
    lv_indev_set_group(encoder, inputGroup);
}

void RotaryInputDriver::encoder_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    static uint32_t prevkey = 0;
    static uint32_t lastPressed = millis();

    data->key = 0;
    data->enc_diff = 0;
    data->state = LV_INDEV_STATE_RELEASED;

    // read FSM rotary encoder queue
    if (INPUTDRIVER_ROTARY_TYPE == 1) {
        RotaryActionType action;
        while (xQueueReceive(inputEventQueue, &action, 0)) {
            ILOG_DEBUG("RotaryActionType: %d", action);
            if (action == TB_ACTION_PRESSED && millis() > lastPressed + 50) {
                data->key = LV_KEY_ENTER;
                data->state = LV_INDEV_STATE_PRESSED;
                lastPressed = millis();
            } else if (action == TB_ACTION_RELEASED) {
                data->key = prevkey;
                data->state = LV_INDEV_STATE_RELEASED;
                prevkey = 0;
            } else if (action == TB_ACTION_CCW) {
                data->enc_diff -= 1;
            } else if (action == TB_ACTION_CW) {
                data->enc_diff += 1;
            }
            if (action != TB_ACTION_NONE) {
                prevkey = data->key;
            }
            ILOG_DEBUG("RotaryAction state:%d, key:%d, prevkey:%d", data->state, data->key, prevkey);
        }
    }
}

void RotaryInputDriver::intPressHandler()
{
    RotaryActionType action = TB_ACTION_NONE;
    if (rotary->readButton() == RotaryEncoder::ButtonState::BUTTON_PRESSED) {
        action = TB_ACTION_PRESSED;
    } else {
        action = TB_ACTION_RELEASED;
    }
    xQueueSendFromISR(inputEventQueue, &action, NULL);
}

void RotaryInputDriver::intHandler()
{
    RotaryActionType action = TB_ACTION_NONE;
    switch (rotary->process()) {
    case RotaryEncoder::DIRECTION_CW:
        action = TB_ACTION_CW;
        xQueueSendFromISR(inputEventQueue, &action, NULL);
        break;
    case RotaryEncoder::DIRECTION_CCW:
        action = TB_ACTION_CCW;
        xQueueSendFromISR(inputEventQueue, &action, NULL);
        break;
    default:
        break;
    }
}

#endif