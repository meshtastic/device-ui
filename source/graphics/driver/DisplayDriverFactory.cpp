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
#if defined(USE_FRAMEBUFFER)
#include "graphics/driver/FBDriver.h"
#endif

#if defined(USE_X11)
#include "graphics/driver/X11Driver.h"
#endif

#if defined(USE_SDL)
#include "graphics/driver/SDLDriver.h"
#endif

#if defined(GFX_DRIVER_INC)
#include GFX_DRIVER_INC
#endif

#include <assert.h>
#include "util/ILog.h"

DisplayDriverFactory::DisplayDriverFactory() {}

/**
 * ctor for compiled-in display driver
 */
DisplayDriver *DisplayDriverFactory::create(uint16_t width, uint16_t height)
{
#if defined(USE_FRAMEBUFFER)
    return &FBDriver::create(width, height);
#endif
#if defined(USE_X11)
    return &X11Driver::create(width, height);
#elif defined(USE_SDL)
    return &SDLDriver::create(width, height);
#elif defined(LGFX_DRIVER)
    return new LGFXDriver<LGFX_DRIVER>(width, height);
#elif defined(LVGL_DRIVER)
    return new LVGLDriver<LVGL_DRIVER>(width, height);
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
#if defined(LVGL_DRIVER)
        if (strcasecmp(cfg._panel.type, "ST7789") == 0) {
            // TODO: add all ported LVGL drivers, e.g. ST7735, ST7796, ILI9341
            return new LVGLDriver<LVGLConfig>(cfg);
        }
#elif defined(LVGL_DRIVER)
        return new LGFXDriver<LGFXConfig>(cfg);
#endif
    }
#endif
#if defined(OLED_DRIVER) || defined(ARCH_PORTDUINO)
    if (cfg._device == DisplayDriverConfig::device_t::CUSTOM_OLED) {
        //return new OLEDDriver<OLEDConfig>(cfg);
    }
#endif
#if defined(EINK_DRIVER) || defined(ARCH_PORTDUINO)
    if (cfg._device == DisplayDriverConfig::device_t::CUSTOM_EINK) {
        //return new EINKDriver<EINKConfig>(cfg);
    }
#endif
#if defined(USE_X11)
    if (cfg._device == DisplayDriverConfig::device_t::X11) {
        return &X11Driver::create(cfg.width(), cfg.height());
    }
#endif
#if defined(USE_FRAMEBUFFER)
    if (cfg._device == DisplayDriverConfig::device_t::FB) {
        return &FBDriver::create(cfg.width(), cfg.height());
    }
#endif
#if defined(USE_SDL)
    if (cfg._device == DisplayDriverConfig::device_t::SDL) {
        return &SDLDriver::create(cfg.width(), cfg.height());
    }
#endif

#if !defined(ARCH_PORTDUINO)
#if defined(LGFX_DRIVER)
    return new LGFXDriver<LGFX_DRIVER>(cfg.width(), cfg.height());
#elif defined(LVGL_DRIVER)
    return new LVGLDriver<LVGLConfig>(cfg.width(), cfg.height());
#endif
#endif
    ILOG_CRIT("DisplayDriverFactory: missing or wrong GFX configuration for device %d", static_cast<int>(cfg._device));
    assert(false);
    return nullptr;
}
