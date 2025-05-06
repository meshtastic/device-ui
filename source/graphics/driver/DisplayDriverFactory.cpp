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
// TODO #include "graphics/driver/EINKDriver.h"
#endif
#if defined(USE_FRAMEBUFFER)
#include "graphics/driver/FBDriver.h"
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
#if defined(USE_FRAMEBUFFER)
    return &FBDriver::create(width, height);
#endif
#if defined(USE_X11)
    return &X11Driver::create(width, height);
#elif defined(USE_SDL2)
    return &SDL2Driver::create(width, height);
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
        if (cfg._panel.driver == DisplayDriverConfig::driver_t::LVGL && strcasecmp(cfg._panel.type, "ST7789") == 0)
            return new LVGLDriver<LVGLConfig>(cfg);
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
#if defined(USE_FRAMEBUFFER)
    if (cfg._device == DisplayDriverConfig::device_t::FB) {
        return &FBDriver::create(cfg.width(), cfg.height());
    }
#endif
#if defined(USE_X11)
    if (cfg._device == DisplayDriverConfig::device_t::X11) {
        return &X11Driver::create(cfg.width(), cfg.height());
    }
#endif
    switch (cfg._device) {
#ifndef ARCH_PORTDUINO
#if !defined(LGFX_DRIVER)
#error "LGFX_DRIVER must be defined!"
#endif
#if defined(T_HMI)
    case DisplayDriverConfig::device_t::THMI:
        return new LGFXDriver<LGFX_T_HMI>(cfg.width(), cfg.height());
        break;
#elif defined(T_DECK)
    case DisplayDriverConfig::device_t::TDECK:
        return new LGFXDriver<LGFX_TDECK>(cfg.width(), cfg.height());
        break;
#elif defined(SENSECAP_INDICATOR)
    case DisplayDriverConfig::device_t::INDICATOR:
        return new LGFXDriver<LGFX_INDICATOR>(cfg.width(), cfg.height());
        break;
#elif defined(ESP_4848S040)
    case DisplayDriverConfig::device_t::ESP4848S040:
        return new LGFXDriver<LGFX_4848S040>(cfg.width(), cfg.height());
        break;
#elif defined(MAKERFABS_480X480)
    case DisplayDriverConfig::device_t::MAKERFABS480X480:
        return new LGFXDriver<LGFX_MAKERFABS480X480>(cfg.width(), cfg.height());
        break;
#elif defined(PICOMPUTER_S3)
    case DisplayDriverConfig::device_t::BPICOMPUTER_S3:
        return new LGFXDriver<LGFX_PICOMPUTER_S3>(cfg.width(), cfg.height());
        break;
#elif defined(TWATCH_S3)
    case DisplayDriverConfig::device_t::TWATCH_S3:
        return new LGFXDriver<LGFX_TWATCH_S3>(cfg.width(), cfg.height());
        break;
#elif defined(UNPHONE)
    case DisplayDriverConfig::device_t::UNPHONE_V9:
        return new LGFXDriver<LGFX_UNPHONE_V9>(cfg.width(), cfg.height());
        break;
#elif defined(ELECROW_PANEL)
    case DisplayDriverConfig::device_t::ELECROW_ADV:
        return new LGFXDriver<LGFX_ELECROW70>(cfg.width(), cfg.height());
        break;
#elif defined(HELTEC_TRACKER)
    case DisplayDriverConfig::device_t::HELTEC_TRACKER:
        // return new LGFXDriver<LGFX_HELTEC_TRACKER>(cfg.width(), cfg.height());
        break;
#elif defined(WT_SC01_PLUS)
    case DisplayDriverConfig::device_t::WT32_SC01_PLUS:
        return new LGFXDriver<LGFX_WT_SC01_PLUS>(cfg.width(), cfg.height());
        break;
#elif defined(ESP2432S028RV1)
    case DisplayDriverConfig::device_t::ESP2432S028RV1:
        return new LGFXDriver<LGFX_ESP2432S028RV1>(cfg.width(), cfg.height());
        break;
#elif defined(ESP2432S028RV2)
    case DisplayDriverConfig::device_t::ESP2432S028RV2:
        return new LGFXDriver<LGFX_ESP2432S028RV2>(cfg.width(), cfg.height());
        break;
#endif
#elif defined(USE_FRAMEBUFFER)
    case DisplayDriverConfig::device_t::FB:
        return &FBDriver::create(cfg.width(), cfg.height());
        break;
#elif defined(USE_X11)
    case DisplayDriverConfig::device_t::X11:
        return &X11Driver::create(cfg.width(), cfg.height());
        break;
#endif
    ILOG_CRIT("DisplayDriverFactory: unsupported or missing device type");
    return nullptr;
}
