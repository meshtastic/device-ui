#pragma once

#include "input/InputDriver.h"

class RotaryEncoder;

/**
 * @brief Input driver for quadrature rotary encoders with push button.
 *        Uses the RotaryEncoder library for proper quadrature decoding.
 *        Designed for devices like T-Lora-Pager that have a scroll wheel.
 */
class RotaryEncoderInputDriver : public InputDriver
{
  public:
    RotaryEncoderInputDriver(void);
    virtual void init(void) override;
    virtual void task_handler(void) override;
    virtual ~RotaryEncoderInputDriver(void);

  protected:
    static void encoder_read(lv_indev_t *indev, lv_indev_data_t *data);

  private:
    static RotaryEncoder *rotary;
    static volatile int16_t encoderDiff;
};
