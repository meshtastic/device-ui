#include "DisplayDriverConfig.h"
#include "LGFXConfig.h"
#include "LGFXDriver.h"
#ifndef PORTDUINO
#include "LGFX_T_DECK.h"
#include "LGFX_T_HMI.h"
#include "LGFX_T_WATCH_S3.h"
// #include "LGFX_UNPHONE.h"
#include "LGFX_ESP2432S028RV1.h"
#include "LGFX_ESP2432S028RV2.h"
#include "LGFX_HELTEC_TRACKER.h"
#include "LGFX_WT_SC01PLUS.h"
#endif

DisplayDriverConfig::DisplayDriverConfig(void) : _device(device_t::NONE) {}

DisplayDriverConfig::DisplayDriverConfig(enum device_t device) : _device(device) {}

DisplayDriverConfig::DisplayDriverConfig(struct panel_config_t &&panel, struct bus_config_t &&bus, struct light_config_t &&light,
                                         struct touch_config_t &&touch)
    : _device(device_t::NONE)
{
}

DisplayDriverConfig &DisplayDriverConfig::device(enum device_t device)
{
    _device = device;
    return *this;
}

DisplayDriverConfig &DisplayDriverConfig::panel(panel_config_t &&cfg)
{
    _panel = cfg;
    _width = _panel.panel_width;
    _height = _panel.panel_height;
    return *this;
}

DisplayDriverConfig &DisplayDriverConfig::bus(bus_config_t &&cfg)
{
    _bus = cfg;
    return *this;
}

DisplayDriverConfig &DisplayDriverConfig::touch(touch_config_t &&cfg)
{
    _touch = cfg;
    return *this;
}

DisplayDriverConfig &DisplayDriverConfig::light(light_config_t &&cfg)
{
    _light = cfg;
    return *this;
}
