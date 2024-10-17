#include "ViewFactory.h"
#if defined(VIEW_128x64) || defined(ARCH_PORTDUINO)
#include "OLEDView_128x64.h"
#endif
#if defined(VIEW_160x80) || defined(ARCH_PORTDUINO)
#include "TFTView_160x80.h"
#endif
#if defined(VIEW_240x240) || defined(ARCH_PORTDUINO)
#include "TFTView_240x240.h"
#endif
#if defined(VIEW_320x240) || defined(ARCH_PORTDUINO)
#include "TFTView_320x240.h"
#endif
#if defined(VIEW_480x320) || defined(ARCH_PORTDUINO)
#include "TFTView_480x320.h"
#endif
#include "ILog.h"
#include <assert.h>

ViewFactory::ViewFactory(void) {}

/**
 * @brief Compile-time fix view creation
 *
 * @return DeviceGUI*
 */
DeviceGUI *ViewFactory::create(void)
{
#if defined(VIEW_128x64)
    return OLEDView_128x64::instance();
#elif defined(VIEW_160x80)
    return TFTView_160x80::instance();
#elif defined(VIEW_240x240)
    return TFTView_240x240::instance();
#elif defined(VIEW_480x320)
    return TFTView_480x320::instance();
#elif defined(VIEW_320x240)
    return TFTView_320x240::instance();
#endif
    ILOG_CRIT("ViewFactory: VIEW is not defined and no config provided");
    assert(false);
    return nullptr;
}

/**
 * @brief Run-time view creation
 *
 * @param cfg
 * @return DeviceGUI*
 */
DeviceGUI *ViewFactory::create(const DisplayDriverConfig &cfg)
{
#if defined(VIEW_128x64) || defined(ARCH_PORTDUINO)
    if (cfg.width() == 128 && cfg.height() == 64) {
        return OLEDView_128x64::instance(cfg);
    }
#endif
#if defined(VIEW_160x80) || defined(ARCH_PORTDUINO)
    if (cfg.width() == 160 && cfg.height() == 80) {
        return TFTView_160x80::instance(cfg);
    }
#endif
#if defined(VIEW_240x240) || defined(ARCH_PORTDUINO)
    if (cfg.width() == 240 && cfg.height() == 240) {
        return TFTView_240x240::instance(cfg);
    }
#endif
#if defined(VIEW_480x320) || defined(ARCH_PORTDUINO)
    if (cfg.width() == 480 && cfg.height() == 320) {
        return TFTView_480x320::instance(cfg);
    }
#endif
#if defined(VIEW_320x240) || defined(ARCH_PORTDUINO)
    // default if nothing else matches
    return TFTView_320x240::instance(cfg);
#endif
}
