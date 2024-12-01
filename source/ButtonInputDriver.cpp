#ifdef INPUTDRIVER_BUTTON_TYPE

#include "ButtonInputDriver.h"
#include "ILog.h"
#include <Arduino.h>

ButtonInputDriver::ButtonInputDriver(void) {}

void ButtonInputDriver::init(void)
{
    button = lv_indev_create();
    lv_indev_set_type(button, LV_INDEV_TYPE_BUTTON);
    lv_indev_set_read_cb(button, button_read);

    // assign buttons to points on the screen
    static const lv_point_t btn_points[1] = {
        {10, 235} // button0: (cog symbol) and button on blank screen
    };

    lv_indev_set_button_points(button, btn_points);

    if (!inputGroup) {
        inputGroup = lv_group_create();
        lv_group_set_default(inputGroup);
    }
    lv_indev_set_group(button, inputGroup);
    lv_indev_enable(button, false);

    pinMode(INPUTDRIVER_BUTTON_TYPE, INPUT);
}

void ButtonInputDriver::button_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    static uint8_t lastBtn = 0;
    int8_t actBtn = getButtonPressedId();
    if (actBtn >= 0) {
        data->state = LV_INDEV_STATE_PRESSED;
        lastBtn = actBtn;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->btn_id = lastBtn;
}

int8_t ButtonInputDriver::getButtonPressedId(void)
{
    if (!digitalRead(INPUTDRIVER_BUTTON_TYPE))
        return 0;
    else
        return -1;
}

#endif