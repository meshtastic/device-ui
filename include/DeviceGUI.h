#pragma once

#include "DisplayDriver.h"
#include "ViewController.h"

/**
 * @brief DeviceGUI - this class hierarchy implements the view part of MVC
 * 
 */
class DeviceGUI {
public:
    DeviceGUI(DisplayDriver* driver);
    virtual void init(void);
    virtual void task_handler(void) { displaydriver->task_handler(); };
    virtual void addNode(void) {};

protected:
    DisplayDriver* displaydriver;
    ViewController* controller;

};

