#include "input/InputDriver.h"

InputDriver *InputDriver::driver = nullptr;
lv_indev_t *InputDriver::keyboard = nullptr;
InputDriver *InputDriver::keyboardProvider = nullptr;
lv_indev_t *InputDriver::pointer = nullptr;
lv_indev_t *InputDriver::encoder = nullptr;
lv_indev_t *InputDriver::button = nullptr;
lv_group_t *InputDriver::inputGroup = nullptr;

InputDriver *InputDriver::instance(void)
{
    if (!driver)
        driver = new InputDriver;
    return driver;
}

InputDriver::~InputDriver(void)
{
    if (keyboardProvider == this)
        keyboardProvider = nullptr;
    if (keyboard)
        releaseKeyboardDevice();
    if (pointer)
        releasePointerDevice();
}
