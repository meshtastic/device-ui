#pragma once

#include "lvgl.h"
#include <cstdint>

class DisplayDriver;
class DeviceGUI;

/**
 * @brief Controls the display sleep/wake state machine and sinusoidal backlight
 *        fade animations.  Owned by LGFXDriver; accessible externally via
 *        DisplayDriver::sleepController().
 *
 *        Unlike a traditional screensaver (which keeps the screen on to prevent
 *        burn-in), this class powers the display off entirely and coordinates
 *        peripheral backlight synchronisation via ScreenSleepObserver.
 */
class ScreenSleepController
{
  public:
    explicit ScreenSleepController(DisplayDriver *panel, uint8_t wakeBrightness);
    void setContext(DeviceGUI *view, lv_indev_t *touch);

    void wake(void);        // request wake — safe to call from any context
    void sleep(void);       // request sleep — safe to call from any context
    bool isSleeping(void) const;

    void tick(uint32_t screenTimeout, bool hasTouch, bool hasButton);

    void    setWakeBrightness(uint8_t brightness);
    uint8_t getWakeBrightness(void) const;

  private:
    void dimStep(bool hasTouch, bool hasButton);
    void wakeStep(void);
    void enterPowerSave(bool hasTouch, bool hasButton);
    void exitPowerSave(bool hasTouch, bool hasButton);
    void checkWakeConditions(uint32_t screenTimeout, bool hasTouch, bool hasButton);
    void startWakeFromCurrent(uint8_t targetBrightness);

    // ms for the brightness fade triggered by a manual sleep request or screen-lock
    static constexpr uint16_t defaultDimDuration = 1000;
    // ms for the brightness fade triggered by the inactivity timeout
    static constexpr uint16_t timeoutDimDuration = 10000;

    DisplayDriver *panel;
    DeviceGUI     *view  = nullptr;
    lv_indev_t    *touch = nullptr;

    volatile bool wakeRequested  = false;
    volatile bool sleepRequested = false;
    bool          powerSaving    = false;

    // Target brightness to restore after waking.  Mirrored here on every user-facing
    // setBrightness() call; animation frames use setHardwareBrightness() so they never
    // corrupt this value.
    uint8_t  wakeBrightness     = 0;
    uint32_t dimStartTime       = 0;
    uint8_t  dimStartBrightness = 0;
    uint16_t dimDuration        = 0;
    uint32_t wakeStartTime      = 0;
};
