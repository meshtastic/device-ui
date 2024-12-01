#pragma once

#include "InputDriver.h"

/**
 * Button (if available) is used to wake up from display sleep
 */
class ButtonInputDriver : public InputDriver
{
  public:
    ButtonInputDriver(void);

    virtual void init(void) override;
    virtual ~ButtonInputDriver(void) {}

  private:
    static void button_read(lv_indev_t *indev, lv_indev_data_t *data);
    static int8_t getButtonPressedId(void);
};