#include "DeviceScreen.h"
#include "Arduino.h"
#include "ILog.h"
#include "ViewFactory.h"

DeviceScreen &DeviceScreen::create(void)
{
    return *new DeviceScreen(nullptr);
}

DeviceScreen &DeviceScreen::create(const DisplayDriverConfig *cfg)
{
    return *new DeviceScreen(cfg);
}

DeviceScreen::DeviceScreen(const DisplayDriverConfig *cfg)
{
    if (cfg) {
        gui = ViewFactory::create(*cfg);
    } else {
        gui = ViewFactory::create();
    }
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
