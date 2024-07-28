#pragma once

#include <stdint.h>

class DisplayDriverConfig;
class DisplayDriver;
class InputDriver;
class IClientBase;

/**
 * @brief DeviceGUI - this class hierarchy implements the view part of MVC
 *
 */
class DeviceGUI
{
  public:
    DeviceGUI(const DisplayDriverConfig *cfg, DisplayDriver *driver);
    virtual ~DeviceGUI();
    virtual void init(IClientBase *client);
    virtual void task_handler(void);
    virtual bool sleep(int16_t pin) { return false; }
    virtual void triggerHeartbeat(void) {}
    // called to inform view to do screen blanking
    virtual void blankScreen(bool enable) {}
    // called when display (driver) is doing screen blanking
    virtual void screenSaving(bool enabled){};
    // return true if screen view is locked
    virtual bool isScreenLocked(void) { return false; };

    DisplayDriver *getDisplayDriver(void) const { return displaydriver; }
    InputDriver *getInputDriver(void) const { return inputdriver; }

  protected:
    DisplayDriver *displaydriver;
    InputDriver *inputdriver;
};
