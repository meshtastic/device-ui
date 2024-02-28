#pragma once

#include "DisplayDriver.h"
#include "ViewController.h"
#include "IClientBase.h"

/**
 * @brief DeviceGUI - this class hierarchy implements the view part of MVC
 * 
 */
class DeviceGUI {
public:
    DeviceGUI(DisplayDriver* driver, ViewController* _controller);
    virtual void init(IClientBase* client);
    virtual void task_handler(void);
    virtual void addNode(void) {};

protected:
    DisplayDriver* displaydriver;
    ViewController* controller;

};

