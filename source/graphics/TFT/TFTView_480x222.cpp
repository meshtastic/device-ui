#if defined(VIEW_480x222)

#include "graphics/view/TFT/TFTView_480x222.h"
#include "graphics/common/ViewController.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverFactory.h"
#include "graphics/map/MapPanel.h"
#include "images.h"
#include "input/InputDriver.h"
#include "lv_i18n.h"
#include "ui.h"
#include "util/ILog.h"

#define THIS TFTView_480x222::instance() // need to use THIS in all static methods

TFTView_480x222 *TFTView_480x222::instance(void)
{
    if (!gui)
        gui = new TFTView_480x222(nullptr, DisplayDriverFactory::create(480, 222));
    return static_cast<TFTView_480x222 *>(gui);
}

TFTView_480x222 *TFTView_480x222::instance(const DisplayDriverConfig &cfg)
{
    if (!gui)
        gui = new TFTView_480x222(&cfg, DisplayDriverFactory::create(cfg));
    return static_cast<TFTView_480x222 *>(gui);
}

TFTView_480x222::TFTView_480x222(const DisplayDriverConfig *cfg, DisplayDriver *driver) : PluggableView(cfg, driver) {}

void TFTView_480x222::init(IClientBase *client)
{
    ILOG_DEBUG("TFTView_480x222 init...");
    PluggableView::init(client);
}

void TFTView_480x222::ui_events_init(void)
{
    PluggableView::ui_events_init();
}

/**
 * @brief initialize UI with persistent data
 */
bool TFTView_480x222::setupUIConfig(const meshtastic_DeviceUIConfig &uiconfig)
{
    return PluggableView::setupUIConfig(uiconfig);
}

/**
 * @brief Initialize all screens and apply customizations
 *
 */
void TFTView_480x222::init_screens(void)
{
    PluggableView::init_screens();
}

/**
 * Overwrite the default generated function ui_init()
 * Wire plugin callbacks into existing view actions
 */

void TFTView_480x222::ui_init(void)
{
    PluggableView::ui_init();
}

void TFTView_480x222::task_handler(void)
{
    PluggableView::task_handler();
}

#endif