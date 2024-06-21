#pragma once

#include "InputDriver.h"

class KeyMatrixInputDriver : public InputDriver
{
  public:
    KeyMatrixInputDriver(void);

    virtual void init(void) override;
    virtual ~KeyMatrixInputDriver(void) {}

  private:
    static void keyboard_read(lv_indev_t *indev, lv_indev_data_t *data);
};