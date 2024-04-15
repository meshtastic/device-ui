#pragma once

#include "DisplayDriverConfig.h"
#include "ILog.h"
#include "strings.h"
#include <LovyanGFX.hpp>

/**
 * @brief Runtime configuration class for lovyanGFX driver
 *
 */
class LGFXConfig : public lgfx::LGFX_Device
{
    lgfx::Panel_Device *_panel_instance;
    lgfx::ILight *_light_instance;
    lgfx::ITouch *_touch_instance;

  public:
    uint16_t screenWidth = 0;
    uint16_t screenHeight = 0;
    LGFXConfig(void) {}

    LGFXConfig(const DisplayDriverConfig *config)
    {
        { // configure panel settings
            if (strcasecmp(config->_panel.type, "ST7789") == 0)
                _panel_instance = new lgfx::Panel_ST7789;
            else if (strcasecmp(config->_panel.type, "ST7735") == 0)
                _panel_instance = new lgfx::Panel_ST7735;
            else if (strcasecmp(config->_panel.type, "ST7735S") == 0)
                _panel_instance = new lgfx::Panel_ST7735S;
            else if (strcasecmp(config->_panel.type, "ILI9341") == 0)
                _panel_instance = new lgfx::Panel_ILI9341;
            //            else if (strcasecmp(config->_panel.type, "ILI9341_2") == 0)
            //                _panel_instance = new lgfx::Panel_ILI9341_2;
            else if (strcasecmp(config->_panel.type, "HX8357D") == 0)
                _panel_instance = new lgfx::Panel_HX8357D;
            else {
                ILOG_ERROR("Device panel support not yet implemented for '%s'\n", config->_panel.type);
                return;
            }

            auto cfg = _panel_instance->config();

            if (config->_panel.rotation) {
                cfg.panel_width = config->_height;
                cfg.panel_height = config->_width;
            } else {
                cfg.panel_width = config->_width;
                cfg.panel_height = config->_height;
            }
            screenWidth = cfg.panel_width;
            screenHeight = cfg.panel_height;

            cfg.pin_cs = config->_panel.pin_cs;
            cfg.pin_rst = config->_panel.pin_rst;
            cfg.pin_busy = config->_panel.pin_busy;
            cfg.offset_x = config->_panel.offset_x;
            cfg.offset_y = config->_panel.offset_y;
            cfg.offset_rotation = config->_panel.offset_rotation;
            cfg.invert = config->_panel.invert;
            cfg.dummy_read_pixel = config->_panel.dummy_read_pixel;
            cfg.dummy_read_bits = config->_panel.dummy_read_bits;
            cfg.readable = config->_panel.readable;
            cfg.invert = config->_panel.invert;
            cfg.rgb_order = config->_panel.rgb_order;
            cfg.dlen_16bit = config->_panel.dlen_16bit;
            cfg.bus_shared = config->_panel.bus_shared;

            _panel_instance->config(cfg);
        }

        { // configure bus settings
            if (config->_bus.pin_d0 > 0) {
#ifndef PORTDUINO
                lgfx::Bus_Parallel8 _bus_instance;
                auto cfg = _bus_instance.config();

                for (int i = 0; i < 8; i++)
                    cfg.pin_data[i] = config->_bus.pin_data[i];
                cfg.pin_rd = config->_bus.pin_rd;
                cfg.pin_wr = config->_bus.pin_wr;
                cfg.pin_rs = config->_bus.pin_rs;
                _bus_instance.config(cfg);
                _panel_instance->setBus(&_bus_instance);
#else
                ILOG_ERROR("Bus_Parallel8 not supported on native\n");
                return;
#endif
            } else {
                lgfx::Bus_SPI _bus_instance; // = new lgfx::Bus_SPI;
                auto cfg = _bus_instance.config();
                cfg.spi_mode = 0;
                cfg.pin_dc = config->_bus.pin_dc;
                _bus_instance.config(cfg);
                _panel_instance->setBus(&_bus_instance);
            }
        }

        { // Configure settings for touch control.
            if (config->_touch.type) {
                if (strcasecmp(config->_touch.type, "XPT2046") == 0) {
                    _touch_instance = new lgfx::Touch_XPT2046;
                } else if (strcasecmp(config->_touch.type, "GT911") == 0) {
                    _touch_instance = new lgfx::Touch_GT911;
                } else if (strcasecmp(config->_touch.type, "FT5x06") == 0 || strcasecmp(config->_touch.type, "FT5206") == 0 ||
                           strcasecmp(config->_touch.type, "FT5306") == 0 || strcasecmp(config->_touch.type, "FT5406") == 0 ||
                           strcasecmp(config->_touch.type, "FT6206") == 0 || strcasecmp(config->_touch.type, "FT6236") == 0 ||
                           strcasecmp(config->_touch.type, "FT6336") == 0 || strcasecmp(config->_touch.type, "FT6436") == 0) {
                    _touch_instance = new lgfx::Touch_FT5x06;
                } else if (strcasecmp(config->_touch.type, "STMPE610") == 0) {
                    _touch_instance = new lgfx::Touch_STMPE610;
                } else {
                    ILOG_ERROR("Touch panel '%s' support not implemented\n", config->_touch.type);
                    return;
                }
            }
            auto cfg = _touch_instance->config();

            cfg.freq = config->_touch.freq;
            cfg.x_min = config->_touch.x_min;
            cfg.x_max = config->_touch.x_max ? config->_touch.x_max : config->_width - 1;
            cfg.y_min = config->_touch.y_min;
            cfg.y_max = config->_touch.y_max ? config->_touch.y_max : config->_height - 1;
            cfg.pin_int = config->_touch.pin_int;
            cfg.pin_rst = config->_touch.pin_rst;
            cfg.bus_shared = config->_touch.bus_shared;
            cfg.offset_rotation = config->_touch.offset_rotation;
            cfg.pin_cs = config->_touch.pin_cs;

            if (config->_touch.i2c_addr) {
                cfg.i2c_port = config->_touch.i2c_port;
                cfg.pin_scl = config->_touch.pin_scl;
                cfg.pin_sda = config->_touch.pin_sda;
                cfg.i2c_addr = config->_touch.i2c_addr;
            } else if (config->_touch.spi_host) {
                cfg.spi_host = config->_touch.spi_host;
                cfg.pin_sclk = config->_touch.pin_sclk;
                cfg.pin_mosi = config->_touch.pin_mosi;
                cfg.pin_miso = config->_touch.pin_miso;
            } else {
                ILOG_ERROR("Touch panel config must specify either spi_host or i2c_addr\n");
                return;
            }

            _touch_instance->config(cfg);
            _panel_instance->setTouch(_touch_instance);
        }

        { // configure TFT backlight
            if (config->_light.pin_bl != -1) {
#ifndef PORTDUINO
                auto light = new lgfx::Light_PWM;
                auto cfg = light->config();

                cfg.freq = config->_light.freq;
                cfg.pin_bl = config->_light.pin_bl;
                cfg.pwm_channel = config->_light.pwm_channel;
                cfg.invert = config->_light.invert;

                light->config(cfg);
                _panel_instance->setLight(light);
                _light_instance = light;
#else
                ILOG_WARN("Light_PWM control not supported on native\n");
                // FIXME: control pin_bl anyhow?
#endif
            }
        }

        setPanel(_panel_instance);
    }
};
