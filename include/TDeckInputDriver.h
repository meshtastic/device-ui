#pragma once

#include "InputDriver.h"

class TDeckInputDriver : public InputDriver
{
  public:
    TDeckInputDriver(void);
    virtual void init(void) override;
    virtual void task_handler(void) override;
    virtual ~TDeckInputDriver(void);

  private:
    static void keyboard_read(lv_indev_t *indev, lv_indev_data_t *data);
};