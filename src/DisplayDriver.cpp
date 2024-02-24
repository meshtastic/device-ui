#include "DisplayDriver.h"


DisplayDriver::DisplayDriver(uint16_t width, uint16_t height) :
    lvgl(width, height), screenWidth(width), screenHeight(height) {

}

void DisplayDriver::init(void) {
    lvgl.init();
}

