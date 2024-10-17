#include "DisplayDriver.h"
#include "ILog.h"

DisplayDriver::DisplayDriver(uint16_t width, uint16_t height)
    : lvgl(width, height), display(nullptr), view(nullptr), screenWidth(width), screenHeight(height)
{
}

void DisplayDriver::init(DeviceGUI *gui)
{
    ILOG_DEBUG("DisplayDriver init...");
    view = gui;
    lvgl.init();
}
