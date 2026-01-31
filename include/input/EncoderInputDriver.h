#pragma once

#include "input/InputDriver.h"

class EncoderInputDriver : public InputDriver
{
  public:
    EncoderInputDriver(void);

    virtual void init(void) override;
    virtual ~EncoderInputDriver(void) {}

    static void intPressHandler(void);
    static void intDownHandler(void);
    static void intUpHandler(void);
    static void intLeftHandler(void);
    static void intRightHandler(void);

  protected:
    enum EncoderActionType {
        TB_ACTION_NONE,
        TB_ACTION_PRESSED,
        TB_ACTION_UP,
        TB_ACTION_DOWN,
        TB_ACTION_LEFT,
        TB_ACTION_RIGHT,
        TB_ACTION_ESC
    };

    static volatile EncoderActionType action;

    static void encoder_read(lv_indev_t *indev, lv_indev_data_t *data);
};