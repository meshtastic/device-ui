#pragma once

#include "comms/IClientBase.h"
#include "graphics/DeviceGUI.h"
#include "graphics/driver/DisplayDriverConfig.h"

#if defined(ARDUINO_ARCH_ESP32)
#include "esp_sleep.h"
#endif

/**
 * @brief DeviceScreen - sets up the GUI and display drivers
 *
 */
class DeviceScreen
{
  public:
    static DeviceScreen &create(void);
    static DeviceScreen &create(const DisplayDriverConfig *cfg);
    static DeviceScreen &create(DisplayDriverConfig &&cfg);

    void init(IClientBase *client);
    void task_handler(void);

#if defined(ARDUINO_ARCH_ESP32)
    int prepareSleep(void *);
    int wakeUp(esp_sleep_wakeup_cause_t cause);
#endif
    void sleep(uint32_t sleepTime = 5);

  private:
    DeviceScreen(const DisplayDriverConfig *cfg);
    DeviceScreen(DisplayDriverConfig &&cfg);
    DeviceGUI *gui;
};