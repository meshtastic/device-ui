#pragma once

#include "DeviceGUI.h"

/**
 * @brief DeviceScreen - sets up the GUI and display drivers
 * 
 */
class DeviceScreen {
public:
    static DeviceScreen& create(void);

    void init(void);
    void task_handler(void);

private:
    DeviceScreen();
    DeviceGUI* gui;
};