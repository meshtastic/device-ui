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

DeviceScreen &DeviceScreen::create(DisplayDriverConfig &&cfg)
{
    return *new DeviceScreen(std::move(cfg));
}

DeviceScreen::DeviceScreen(const DisplayDriverConfig *cfg)
{
    if (cfg) {
        gui = ViewFactory::create(*cfg);
    } else {
        gui = ViewFactory::create();
    }
}

DeviceScreen::DeviceScreen(DisplayDriverConfig &&cfg)
{
    gui = ViewFactory::create(cfg);
}

void DeviceScreen::init(IClientBase *client)
{
    ILOG_DEBUG("DeviceScreen::init()...");
    gui->init(client);

    // #ifdef TFT_BL
    //     digitalWrite(TFT_BL, HIGH);
    //     pinMode(TFT_BL, OUTPUT);
    // #endif

#ifdef VTFT_CTRL
    digitalWrite(VTFT_CTRL, LOW);
    pinMode(VTFT_CTRL, OUTPUT);
#endif
    ILOG_DEBUG("DeviceScreen::init() done.");
}

void DeviceScreen::task_handler(void)
{
    gui->task_handler();
}
