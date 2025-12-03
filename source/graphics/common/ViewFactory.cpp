#include "graphics/common/ViewFactory.h"
#if defined(VIEW_128x64) || defined(ARCH_PORTDUINO)
#include "graphics/view/OLED/OLEDView_128x64.h"
#endif
#if defined(VIEW_160x80) || defined(ARCH_PORTDUINO)
#include "graphics/view/TFT/TFTView_160x80.h"
#endif
#if defined(VIEW_240x240) || defined(ARCH_PORTDUINO)
#include "graphics/view/TFT/TFTView_240x240.h"
#endif
#if defined(VIEW_320x240) || defined(ARCH_PORTDUINO)
#include "graphics/view/TFT/TFTView_320x240.h"
#endif
#if defined(VIEW_480x222) || defined(ARCH_PORTDUINO)
#include "graphics/view/TFT/TFTView_480x222.h"
#endif
#if defined(VIEW_320x170) || defined(ARCH_PORTDUINO)
#include "graphics/view/TFT/TFTView_320x170.h"
#endif
#include "util/ILog.h"
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
#elif defined(VIEW_480x222)
    return TFTView_480x222::instance();
#elif defined(VIEW_320x170)
    return TFTView_320x170::instance();
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
#if defined(VIEW_128x64)
    if (cfg.width() == 128 && cfg.height() == 64) {
        return OLEDView_128x64::instance(cfg);
    }
#endif
#if defined(VIEW_160x80)
    if (cfg.width() == 160 && cfg.height() == 80) {
        return TFTView_160x80::instance(cfg);
    }
#endif
#if defined(VIEW_240x240)
    if (cfg.width() == 240 && cfg.height() == 240) {
        return TFTView_240x240::instance(cfg);
    }
#endif
#if defined(VIEW_480x222)
    if (cfg.width() == 480 && cfg.height() == 222) {
        return TFTView_480x222::instance(cfg);
    }
#endif
#if defined(VIEW_320x170)
    if (cfg.width() == 320 && cfg.height() == 172) {
        return TFTView_320x170::instance(cfg);
    }
#endif
#if defined(VIEW_320x240)
    // default if nothing else matches
    return TFTView_320x240::instance(cfg);
#endif
    ILOG_CRIT("ViewFactory: did not find suitable view for creation with config %dx%d", cfg.width(), cfg.height());
    assert(false);
    return nullptr;
}
