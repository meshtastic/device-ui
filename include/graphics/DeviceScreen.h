#pragma once

#include "comms/IClientBase.h"
#include "graphics/DeviceGUI.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "util/ISpiLock.h"

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

    /**
     * @param spiLock the host's bus lock, for boards where the display or SD card shares
     * an SPI bus with peripherals the host drives itself. See ISpiLock - note the
     * reentrancy requirement.
     *
     * Taken by reference rather than pointer so that create(nullptr) stays unambiguous
     * against the config-taking overloads below.
     */
    static DeviceScreen &create(ISpiLock &spiLock);
    static DeviceScreen &create(const DisplayDriverConfig *cfg, ISpiLock *spiLock = nullptr);
    static DeviceScreen &create(DisplayDriverConfig &&cfg, ISpiLock *spiLock = nullptr);

    void init(IClientBase *client);
    void task_handler(void);
    void toggleDisplay(void);

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