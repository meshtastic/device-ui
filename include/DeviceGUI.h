#pragma once

class DisplayDriver;
class IClientBase;

/**
 * @brief DeviceGUI - this class hierarchy implements the view part of MVC
 * 
 */
class DeviceGUI {
public:
    DeviceGUI(DisplayDriver* driver);
    virtual void init(IClientBase* client);
    virtual void task_handler(void);
    virtual void addNode(void) {};

protected:
    DisplayDriver* displaydriver;
};

