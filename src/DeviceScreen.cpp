#include "DeviceScreen.h"

#if defined (VIEW_CLASS_INC)
#include VIEW_CLASS_INC
#elif defined(USE_X11)
#include "X11Driver.h"
#else
#error "Undefined device view!"
#endif

#include "Arduino.h"


DeviceScreen& DeviceScreen::create(void) {
    return *new DeviceScreen;
}

DeviceScreen::DeviceScreen() {
    Serial.println("DeviceScreen()...");
    gui = VIEW_CLASS::instance();
}


void DeviceScreen::init(IClientBase* client) {
    Serial.println("DeviceScreen::init()...");
    gui->init(client);

#ifdef TFT_BL
    digitalWrite(TFT_BL, HIGH);
    pinMode(TFT_BL, OUTPUT);
#endif

#ifdef VTFT_CTRL
    digitalWrite(VTFT_CTRL, LOW);
    pinMode(VTFT_CTRL, OUTPUT);
#endif

}

void DeviceScreen::task_handler(void) {
    gui->task_handler();
}
