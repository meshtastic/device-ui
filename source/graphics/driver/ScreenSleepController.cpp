#include "graphics/driver/ScreenSleepController.h"

#include "graphics/DeviceGUI.h"
#include "graphics/driver/DisplayDriver.h"
#include "input/InputDriver.h"
#include "input/ScreenSleepObserver.h"
#include "util/ILog.h"
#include "lvgl.h"
#include <math.h>

ScreenSleepController::ScreenSleepController(DisplayDriver *panel, uint8_t wakeBrightness)
    : panel(panel), wakeBrightness(wakeBrightness)
{
}

void ScreenSleepController::setContext(DeviceGUI *v, lv_indev_t *t)
{
    view  = v;
    touch = t;
}

void ScreenSleepController::wake(void)
{
    wakeRequested = true;
}

void ScreenSleepController::sleep(void)
{
    sleepRequested = true;
}

bool ScreenSleepController::isSleeping(void) const
{
    return powerSaving;
}

void ScreenSleepController::setWakeBrightness(uint8_t brightness)
{
    wakeBrightness = brightness;
}

uint8_t ScreenSleepController::getWakeBrightness(void) const
{
    return wakeBrightness;
}

void ScreenSleepController::tick(uint32_t screenTimeout, bool hasTouch, bool hasButton)
{
    bool shouldSleep = (screenTimeout > 0 && lv_display_get_inactive_time(NULL) > screenTimeout) || powerSaving ||
                       (view && view->isScreenLocked()) || sleepRequested;

    if (shouldSleep) {
        // Only sleep if there is a means to wake up again
        if ((view && (view->getInputDriver()->hasPointerDevice() || view->getInputDriver()->hasKeyboardDevice())) ||
            hasTouch || hasButton) {
            if (panel->hasBacklight()) {
                if (!powerSaving) dimStep(hasTouch, hasButton);
                if (powerSaving)  checkWakeConditions(screenTimeout, hasTouch, hasButton);
            } else {
                // No backlight: blank/unblank directly without a brightness fade
                if (!powerSaving) {
                    view->blankScreen(true);
                    if (auto *id = InputDriver::instance()) {
                        if (id->hasEncoderDevice())
                            lv_indev_enable(id->getEncoder(), false);
                    }
                    panel->panelSleep();
                    panel->powerSaveOn();
                    powerSaving = true;
                }
                if ((screenTimeout + 50 > lv_display_get_inactive_time(NULL) && !view->isScreenLocked() && !sleepRequested) ||
                    wakeRequested) {
                    view->blankScreen(false);
                    panel->powerSaveOff();
                    panel->panelWake();
                    if (auto *id = InputDriver::instance()) {
                        if (id->hasEncoderDevice())
                            lv_indev_enable(id->getEncoder(), true);
                    }
                    powerSaving    = false;
                    wakeRequested  = false;
                    sleepRequested = false;
                    lv_display_trigger_activity(NULL);
                }
            }
        }
    } else if (panel->getBrightness() < wakeBrightness) {
        // Brightness is below target — either a timeout fade was interrupted by activity,
        // or we just exited powersave and the wake animation is in progress.
        if (wakeStartTime == 0)
            startWakeFromCurrent(wakeBrightness);
        wakeStep();
    }
}

// Join the wake sinusoid at the current brightness so the animation reverses smoothly
// from wherever the dim-to-off curve was cancelled.
// asinf(current/targetBrightness) gives the sine-curve phase that matches the current brightness,
// which is used to backdate wakeStartTime so wakeStep() picks up at the right point.
void ScreenSleepController::startWakeFromCurrent(uint8_t targetBrightness)
{
    float frac = targetBrightness > 0 ? (float)panel->getBrightness() / (float)targetBrightness : 0.0f;
    if (frac > 1.0f) frac = 1.0f;
    wakeStartTime = millis() - (uint32_t)(asinf(frac) / 1.5707963f * defaultDimDuration);
    dimStartTime  = 0;
}

// Advance one frame of the dim-to-off animation, or enter powersave when complete.
// Handles: starting a fresh dim, speeding up via manual sleep request, and
// cancelling mid-fade when a wake key is pressed.
void ScreenSleepController::dimStep(bool hasTouch, bool hasButton)
{
    uint8_t brightness = panel->getBrightness();
    if (brightness > 0) {
        if (dimStartTime == 0) {
            dimStartTime       = millis();
            dimStartBrightness = brightness;
            wakeStartTime      = 0; // discard any stale wake animation from a prior cycle
            dimDuration        = (sleepRequested || (view && view->isScreenLocked()))
                                 ? defaultDimDuration : timeoutDimDuration;
            wakeRequested = false; // discard any stale wake flag from before the dim started
            if (auto *obs = ScreenSleepObserver::instance()) obs->onScreenSleep();
        }
        if (wakeRequested) {
            // Cancel mid-fade: reverse the animation smoothly from current brightness.
            startWakeFromCurrent(dimStartBrightness);
            wakeRequested  = false;
            sleepRequested = false;
            lv_display_trigger_activity(NULL);
        } else {
            // Sinusoidal ease-out (cos t·π/2, peak→0)
            float progress = (float)(millis() - dimStartTime) / (float)dimDuration;
            if (progress > 1.0f) progress = 1.0f;
            panel->setHardwareBrightness((uint8_t)(dimStartBrightness * cosf(progress * 1.5707963f)));
            if (auto *obs = ScreenSleepObserver::instance()) obs->applyBrightnessProgress(progress, false);
        }
    } else {
        enterPowerSave(hasTouch, hasButton);
    }
}

// Advance one frame of the off-to-bright wake animation.
// Sinusoidal ease-in (sin t·π/2, 0→peak) mirrors the dim curve.
// wakeStartTime may be backdated (via asinf) to join the curve mid-way when
// cancelling a dim or waking from a partially-off state.
void ScreenSleepController::wakeStep(void)
{
    dimStartTime = 0;
    if (wakeStartTime == 0) {
        panel->setHardwareBrightness(wakeBrightness);
        if (auto *obs = ScreenSleepObserver::instance()) obs->applyBrightnessProgress(1.0f, true);
        return;
    }
    float progress = (float)(millis() - wakeStartTime) / (float)defaultDimDuration;
    if (progress >= 1.0f) {
        panel->setHardwareBrightness(wakeBrightness);
        wakeStartTime = 0;
        if (auto *obs = ScreenSleepObserver::instance()) obs->applyBrightnessProgress(1.0f, true);
    } else {
        panel->setHardwareBrightness((uint8_t)(wakeBrightness * sinf(progress * 1.5707963f)));
        if (auto *obs = ScreenSleepObserver::instance()) obs->applyBrightnessProgress(progress, true);
    }
}

// Power down the panel and disable indevs that should not fire during sleep.
void ScreenSleepController::enterPowerSave(bool hasTouch, bool hasButton)
{
    ILOG_INFO("enter powersave");
    view->screenSaving(true);
    if (auto *id = InputDriver::instance()) {
        if (hasTouch && hasButton) {
            ILOG_DEBUG("disable touch, enable button input");
            lv_indev_enable(touch, false);
            lv_indev_enable(id->getButton(), true);
        }
        if (id->hasEncoderDevice())
            lv_indev_enable(id->getEncoder(), false);
    }
    panel->panelSleep();
    panel->powerSaveOn();
    powerSaving = true;
    if (auto *obs = ScreenSleepObserver::instance()) obs->applyBrightnessProgress(1.0f, false); // dim animation complete: peripheral backlight off
}

// Check whether any wake condition is met and, if so, power the panel back up.
// Manual-sleep guard: the activity-based condition is suppressed while sleepRequested
// is true, preventing an immediate re-wake when the user was recently active.
void ScreenSleepController::checkWakeConditions(uint32_t screenTimeout, bool hasTouch, bool hasButton)
{
    int pin_int = panel->getTouchIntPin();
    if (hasButton) {
#ifdef BUTTON_PIN // only relevant for CYD scenario
        pin_int = BUTTON_PIN;
#endif
    }
    bool activityWake = !sleepRequested &&
                        screenTimeout + 50 > lv_display_get_inactive_time(NULL) &&
                        !(view && view->isScreenLocked());
    if ((pin_int >= 0 && view->sleep(pin_int)) || activityWake || wakeRequested)
        exitPowerSave(hasTouch, hasButton);
}

// Power the panel back up, re-enable indevs, and start the wake brightness animation.
void ScreenSleepController::exitPowerSave(bool hasTouch, bool hasButton)
{
    delay(2); // let the CPU finish restoring registers in case of light sleep
    ILOG_INFO("leaving powersave");
    powerSaving    = false;
    dimStartTime   = 0;
    wakeRequested  = false;
    sleepRequested = false;
    view->triggerHeartbeat();
    panel->powerSaveOff();
    panel->panelWake();
    panel->setHardwareBrightness(0);
    wakeStartTime = millis();
    view->screenSaving(false);
    if (auto *id = InputDriver::instance()) {
        if (hasTouch && hasButton) {
            ILOG_DEBUG("enable touch, disable button input");
            lv_indev_enable(touch, true);
            lv_indev_enable(id->getButton(), false);
        }
        if (id->hasEncoderDevice())
            lv_indev_enable(id->getEncoder(), true);
    }
    lv_display_trigger_activity(NULL);
}
