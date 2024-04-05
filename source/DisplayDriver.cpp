#include "DisplayDriver.h"

DisplayDriver::DisplayDriver(uint16_t width, uint16_t height) : lvgl(width, height), screenWidth(width), screenHeight(height) {}

void DisplayDriver::init(DeviceGUI *gui)
{
    view = gui;
    lvgl.init();
}
