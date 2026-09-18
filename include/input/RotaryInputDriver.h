#pragma once

#include "input/InputDriver.h"

#if defined(ARDUINO_ARCH_ESP32)
#define HAS_FREE_RTOS
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#endif

#if defined(ARDUINO_NRF52_ADAFRUIT) || defined(ARDUINO_ARCH_RP2040)
#define HAS_FREE_RTOS

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#endif

class RotaryEncoder;

/**
 * FSM based rotary encoder
 */

class RotaryInputDriver : public InputDriver
{
  public:
    RotaryInputDriver(void);

    virtual void init(void) override;
    virtual ~RotaryInputDriver(void) {}

    static void intPressHandler(void);
    static void intHandler(void);

  protected:
    enum RotaryActionType { TB_ACTION_NONE, TB_ACTION_PRESSED, TB_ACTION_RELEASED, TB_ACTION_CW, TB_ACTION_CCW };

    static void encoder_read(lv_indev_t *indev, lv_indev_data_t *data);

  private:
    static RotaryEncoder *rotary;
    static QueueHandle_t inputEventQueue;
};