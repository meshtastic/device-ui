#include "graphics/driver/DisplayDriverFactory.h"

#if defined(LGFX_DRIVER) || defined(ARCH_PORTDUINO)
#include "graphics/LGFX/LGFXConfig.h"
#include "graphics/LGFX/LGFXDriver.h"
#endif

#if defined(LVGL_DRIVER) || defined(ARCH_PORTDUINO)
#include "graphics/LVGL/LVGLConfig.h"
#include "graphics/LVGL/LVGLDriver.h"
#endif

#if defined(OLED_DRIVER) || defined(ARCH_PORTDUINO)
#include "graphics/driver/OLEDDriver.h"
#endif

#if defined(EINK_DRIVER) || defined(ARCH_PORTDUINO)
#include "graphics/driver/EINKDriver.h"
#endif

#if defined(USE_X11)
#include "graphics/driver/X11Driver.h"
#endif

#if defined(USE_SDL2)
#include "graphics/driver/SDL2Driver.h"
#endif

#if defined(GFX_DRIVER_INC)
#include GFX_DRIVER_INC
#endif

#include <assert.h>

DisplayDriverFactory::DisplayDriverFactory() {}

/**
 * ctor for compiled-in display driver
 */
DisplayDriver *DisplayDriverFactory::create(uint16_t width, uint16_t height)
{
#if defined(USE_X11)
    return &X11Driver::create(width, height);
#elif defined(USE_SDL2)
    return &SDL2Driver::create(width, height);
#elif defined(LGFX_DRIVER)
    return new LGFXDriver<LGFX_DRIVER>(width, height);
#elif defined(LVGL_DRIVER)
    return new LVGL_DRIVER();
#endif
    ILOG_CRIT("DisplayDriverFactory: missing or wrong configuration");
    assert(false);
    return nullptr;
}

/**
 * ctor for run-time configurable display driver
 */
DisplayDriver *DisplayDriverFactory::create(const DisplayDriverConfig &cfg)
{
#if defined(ARCH_PORTDUINO)
    if (cfg._device == DisplayDriverConfig::device_t::CUSTOM_TFT) {
        if (strcasecmp(cfg._panel.type, "ST7789") == 0)
            return new LVGLConfig(cfg);
        else
            return new LGFXDriver<LGFXConfig>(cfg);
    }
#endif
#if defined(OLEDConfig) || defined(ARCH_PORTDUINO)
    if (cfg._device == DisplayDriverConfig::device_t::CUSTOM_OLED) {
        // TODO return new OLEDDriver<OLEDConfig>(cfg);
    }
#endif
#if defined(EINKConfig) || defined(ARCH_PORTDUINO)
    if (cfg._device == DisplayDriverConfig::device_t::CUSTOM_EINK) {
        // TODO return new EINKDriver<EINKConfig>(cfg);
    }
#endif
#if defined(USE_X11)
    if (cfg._device == DisplayDriverConfig::device_t::X11) {
        return &X11Driver::create(cfg.width(), cfg.height());
    }
#endif
#if defined(USE_SDL2)
    if (cfg._device == DisplayDriverConfig::device_t::SDL2) {
        return &SDL2Driver::create(cfg.width(), cfg.height());
    }
#endif

#if !defined(ARCH_PORTDUINO)
#if defined(LGFX_DRIVER)
    return new LGFXDriver<LGFX_DRIVER>(cfg.width(), cfg.height());
#elif defined(LVGL_DRIVER)
    return new LVGL_DRIVER(cfg);
#endif
#endif
    return nullptr;
}
