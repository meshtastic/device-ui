#pragma once

#include "input/InputDriver.h"

class I2CKeyboardInputDriver : public InputDriver
{
  public:
    I2CKeyboardInputDriver(void);
    virtual void init(void) override;
    virtual void task_handler(void) override;
    virtual ~I2CKeyboardInputDriver(void);

  private:
    static void keyboard_read(lv_indev_t *indev, lv_indev_data_t *data);
};