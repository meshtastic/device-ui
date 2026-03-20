#pragma once

/**
 * @brief Optional observer that synchronizes peripheral backlights (e.g. a keyboard
 *        LED) with the display fade animation.  Register via setInstance(); all
 *        call-sites null-guard, so it is safe to leave unset on devices that don't need it.
 */
class ScreenSleepObserver
{
  public:
    static ScreenSleepObserver *instance(void) { return observer; }
    static void setInstance(ScreenSleepObserver *obs) { observer = obs; }

    virtual void onScreenSleep(void) {}
    // progress: animation progress 0.0–1.0 (0 = animation start, 1 = animation complete);
    // fadingIn: true = waking (0→full brightness), false = dimming (full brightness→0)
    virtual void applyBrightnessProgress(float progress, bool fadingIn) {}

    virtual ~ScreenSleepObserver() = default;

  protected:
    static ScreenSleepObserver *observer;
};
