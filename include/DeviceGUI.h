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
    virtual void blankScreen(bool enable) {}

    DisplayDriver *getDisplayDriver(void) const { return displaydriver; }
    InputDriver *getInputDriver(void) const { return inputdriver; }

  protected:
    DisplayDriver *displaydriver;
    InputDriver *inputdriver;
};
