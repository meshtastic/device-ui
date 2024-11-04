#include "DisplayDriverFactory.h"
#include "LGFXConfig.h"
#include <assert.h>
#if defined(LGFX_DRIVER) || defined(ARCH_PORTDUINO)
#include "LGFXDriver.h"
#endif
#if defined(OLED_DRIVER) || defined(ARCH_PORTDUINO)
#include "OLEDDriver.h"
#endif
#if defined(EINK_DRIVER) || defined(ARCH_PORTDUINO)
// TODO #include "EINKDriver.h"
#endif
#if defined(USE_X11)
#include "X11Driver.h"
#endif

#ifndef ARCH_PORTDUINO
#ifdef LGFX_DRIVER_TEMPLATE
#include "LGFX_GENERIC.h"
#endif
#ifdef T_HMI
#include "LGFX_T_HMI.h"
#endif
#ifdef SENSECAP_INDICATOR
#include "LGFX_INDICATOR.h"
#endif
#ifdef ESP_4848S040
#include "LGFX_4848S040.h"
#endif
#ifdef T_DECK
#include "LGFX_T_DECK.h"
#endif
#ifdef PICOMPUTER_S3
#include "LGFX_PICOMPUTER_S3.h"
#endif
#ifdef T_WATCH_S3
#include "LGFX_T_WATCH_S3.h"
#endif
#ifdef UNPHONE
#include "LGFX_UNPHONE.h"
#endif
#ifdef ESP32_2432S022
#include "LGFX_ESP2432S022.h"
#endif
#ifdef ESP32_2432S028RV1
#include "LGFX_ESP2432S028RV1.h"
#endif
#ifdef ESP32_2432S028RV2
#include "LGFX_ESP2432S028RV2.h"
#endif
#ifdef WT32_SC01
#include "LGFX_WT_SC01PLUS.h"
#endif
#ifdef HELTEC_TRACKER
#include "LGFX_HELTEC_TRACKER.h"
#endif
#ifdef NODEMCU_32S
#include "LGFX_ESPILI9341XPT2046.h"
#endif
#endif

DisplayDriverFactory::DisplayDriverFactory() {}

DisplayDriver *DisplayDriverFactory::create(uint16_t width, uint16_t height)
{
#if defined(USE_X11)
    return &X11Driver::create(width, height);
#elif defined(LGFX_DRIVER)
    return new LGFXDriver<LGFX_DRIVER>(width, height);
#endif
    ILOG_CRIT("DisplayDriverFactory: missing or wrong configuration");
    assert(false);
    return nullptr;
}

DisplayDriver *DisplayDriverFactory::create(const DisplayDriverConfig &cfg)
{
#if defined(LGFXConfig) || defined(ARCH_PORTDUINO)
    if (cfg._device == DisplayDriverConfig::device_t::CUSTOM_TFT) {
        // for now assume LGFX driver, but could be also TFT_eSPI if implemented
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
#elif defined(USE_X11)
    case DisplayDriverConfig::device_t::X11:
        return &X11Driver::create(cfg.width(), cfg.height());
        break;
#endif
    default:
        ILOG_CRIT("LGFX device_t config not implemented: %d", cfg._device);
        assert(false);
        break;
    }
    return nullptr;
}
