#include "DeviceScreen.h"
#include "Arduino.h"
#include "ILog.h"

// device-ui library requires defining VIEW_CLASS_INC and VIEW_CLASS
// e.g. -DVIEW_CLASS_INC=\"TFTView_320x240.h\" -DVIEW_CLASS=TFTView_320x240

#if defined(VIEW_CLASS_INC)
#include VIEW_CLASS_INC
#elif defined(USE_X11)
#include "X11Driver.h"
#else
#error "Undefined device view!"
#endif

DeviceScreen &DeviceScreen::create(void)
{
    return *new DeviceScreen;
}

DeviceScreen::DeviceScreen()
{
    gui = VIEW_CLASS::instance();
}

void DeviceScreen::init(IClientBase *client)
{
    ILOG_DEBUG("DeviceScreen::init()...\n");
    gui->init(client);

#ifdef TFT_BL
    digitalWrite(TFT_BL, HIGH);
    pinMode(TFT_BL, OUTPUT);
#endif

#ifdef VTFT_CTRL
    digitalWrite(VTFT_CTRL, LOW);
    pinMode(VTFT_CTRL, OUTPUT);
#endif
    ILOG_DEBUG("DeviceScreen::init() done.\n");
}

void DeviceScreen::task_handler(void)
{
    gui->task_handler();
}
