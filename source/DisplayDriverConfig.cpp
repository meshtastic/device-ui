#include "DisplayDriverConfig.h"

DisplayDriverConfig::DisplayDriverConfig(void) : _device(device_t::NONE), _width(c_default_width), _height(c_default_height) {}

DisplayDriverConfig::DisplayDriverConfig(enum device_t device, uint16_t width, uint16_t height)
    : _device(device), _width(width), _height(height)
{
}

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
