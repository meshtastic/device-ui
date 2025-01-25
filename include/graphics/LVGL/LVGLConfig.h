#pragma once

#ifdef LVGL_DRIVER

#include "graphics/LVGL/LVGLDriver.h"
#include "graphics/LVGL/LVGL_ST7789_Driver.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "strings.h"
#include "util/ILog.h"

class DisplayDeviceDriver;

/**
 * @brief Runtime configuration class for lvgl driver
 *
 */
class LVGLConfig : public LVGLDriver<LVGLConfig>
{
  public:
    uint16_t screenWidth = 0;
    uint16_t screenHeight = 0;

    void init(void) {}
    bool hasButton(void) { return false; }
    bool light(void) { return true; }

    LVGLConfig(void) : LVGLDriver(0, 0) {}

    LVGLConfig(const DisplayDriverConfig &config) : LVGLDriver(config._width, config._height)
    {
        ILOG_DEBUG("LVGLConfig ...\n");
        { // configure panel settings
            if (strcasecmp(config._panel.type, "ST7789") == 0) {
                lvglDeviceDriver = new LVGL_ST7789_Driver(config);
                // lvglDeviceDriver = new LVGLDriver<LVGL_DRIVER>(config);
                //  TODO: allocate touch driver
            } else {
                ILOG_ERROR("LVGL device panel support not yet implemented for '%s'\n", config._panel.type);
                return;
            }
        }
#if 0
            auto cfg = lvglDriver->config();

            if (config._panel.rotation) {
                cfg.panel_width = config._height;
                cfg.panel_height = config._width;
            } else {
                cfg.panel_width = config._width;
                cfg.panel_height = config._height;
            }
            screenWidth = cfg.panel_width;
            screenHeight = cfg.panel_height;

            cfg.pin_cs = config._panel.pin_cs;
            cfg.pin_rst = config._panel.pin_rst;
            cfg.pin_busy = config._panel.pin_busy;
            cfg.offset_x = config._panel.offset_x;
            cfg.offset_y = config._panel.offset_y;
            cfg.offset_rotation = config._panel.offset_rotation;
            cfg.invert = config._panel.invert;
            cfg.dummy_read_pixel = config._panel.dummy_read_pixel;
            cfg.dummy_read_bits = config._panel.dummy_read_bits;
            cfg.readable = config._panel.readable;
            cfg.invert = config._panel.invert;
            cfg.rgb_order = config._panel.rgb_order;
            cfg.dlen_16bit = config._panel.dlen_16bit;
            cfg.bus_shared = config._panel.bus_shared;

            ILOG_DEBUG("Panel_Device(%s): %dx%d, cs=%d, rst=%d, busy=%d\n", config._panel.type, screenWidth, screenHeight,
                       cfg.pin_cs, cfg.pin_rst, cfg.pin_busy);
            _panel_instance->config(cfg);
        }

        { // configure bus settings
#ifndef ARCH_PORTDUINO
            if (config._bus.parallel.pin_d0 > 0) {
                lgfx::Bus_Parallel8 *bus = new lgfx::Bus_Parallel8;
                auto cfg = bus->config();
                cfg.freq_write = config._bus.freq_write;
#if CONFIG_IDF_TARGET_ESP32S3
                cfg.freq_read = config._bus.freq_read;
#endif
                for (int i = 0; i < 8; i++)
                    cfg.pin_data[i] = config._bus.parallel.pin_data[i];
                cfg.pin_rd = config._bus.parallel.pin_rd;
                cfg.pin_wr = config._bus.parallel.pin_wr;
                cfg.pin_rs = config._bus.parallel.pin_rs;
                bus->config(cfg);
                _bus_instance = bus;
                _panel_instance->setBus(_bus_instance);
            } else
#endif
            {
                lgfx::Bus_SPI *bus = new lgfx::Bus_SPI;
                auto cfg = bus->config();
                cfg.freq_write = config._bus.freq_write;
                cfg.freq_read = config._bus.freq_read;
                cfg.pin_sclk = config._bus.spi.pin_sclk;
                cfg.pin_miso = config._bus.spi.pin_miso;
                cfg.pin_mosi = config._bus.spi.pin_mosi;
                cfg.pin_dc = config._bus.spi.pin_dc;
                cfg.spi_mode = config._bus.spi.spi_mode;
#ifdef ARDUINO_ARCH_ESP32
                cfg.spi_host = (spi_host_device_t)config._bus.spi.spi_host;
#else
                cfg.spi_host = config._bus.spi.spi_host;
#endif
                ILOG_DEBUG("Bus_SPI: freq=%d, host=%02x, dc=%d\n", cfg.freq_write, cfg.spi_host, cfg.pin_dc);
                bus->config(cfg);
                _bus_instance = bus;
                _panel_instance->setBus(_bus_instance);
            }
        }

        { // Configure settings for touch control.
            if (config._touch.type && strcasecmp(config._touch.type, "NOTOUCH") != 0) {
                if (strcasecmp(config._touch.type, "XPT2046") == 0) {
                    _touch_instance = new lgfx::Touch_XPT2046;
                } else if (strcasecmp(config._touch.type, "GT911") == 0) {
                    _touch_instance = new lgfx::Touch_GT911;
                } else if (strcasecmp(config._touch.type, "FT5x06") == 0 || strcasecmp(config._touch.type, "FT5206") == 0 ||
                           strcasecmp(config._touch.type, "FT5306") == 0 || strcasecmp(config._touch.type, "FT5406") == 0 ||
                           strcasecmp(config._touch.type, "FT6206") == 0 || strcasecmp(config._touch.type, "FT6236") == 0 ||
                           strcasecmp(config._touch.type, "FT6336") == 0 || strcasecmp(config._touch.type, "FT6436") == 0) {
                    _touch_instance = new lgfx::Touch_FT5x06;
                } else if (strcasecmp(config._touch.type, "STMPE610") == 0) {
                    _touch_instance = new lgfx::Touch_STMPE610;
                } else {
                    ILOG_ERROR("Touch panel '%s' support not implemented\n", config._touch.type);
                    return;
                }

                auto cfg = _touch_instance->config();

                cfg.freq = config._touch.freq;
                if (config._touch.x_min >= 0)
                    cfg.x_min = config._touch.x_min;
                if (config._touch.x_max >= 0)
                    cfg.x_max = config._touch.x_max;
                if (config._touch.y_min >= 0)
                    cfg.y_min = config._touch.y_min;
                if (config._touch.y_max >= 0)
                    cfg.y_max = config._touch.y_max;
                cfg.pin_int = config._touch.pin_int;
                cfg.pin_rst = config._touch.pin_rst;
                cfg.bus_shared = config._touch.bus_shared;
                cfg.offset_rotation = config._touch.offset_rotation;
                cfg.pin_cs = config._touch.pin_cs;

                if (config._touch.i2c.i2c_addr > 0 && cfg.pin_cs == -1) {
                    cfg.i2c_port = config._touch.i2c.i2c_port;
                    cfg.pin_scl = config._touch.i2c.pin_scl;
                    cfg.pin_sda = config._touch.i2c.pin_sda;
                    cfg.i2c_addr = config._touch.i2c.i2c_addr;
                    ILOG_DEBUG("Touch_I2C: freq=%d, port=%02x, addr=%d, cs=%d, int=%d, rst=%d\n", cfg.freq, cfg.i2c_port,
                               cfg.i2c_addr, cfg.pin_cs, cfg.pin_int, cfg.pin_rst);
                } else {
                    cfg.spi_host = config._touch.spi.spi_host;
                    cfg.pin_sclk = config._touch.spi.pin_sclk;
                    cfg.pin_mosi = config._touch.spi.pin_mosi;
                    cfg.pin_miso = config._touch.spi.pin_miso;
                    ILOG_DEBUG("Touch_SPI: freq=%d, host=%02x, cs=%d, int=%d, rst=%d\n", cfg.freq, cfg.spi_host, cfg.pin_cs,
                               cfg.pin_int, cfg.pin_rst);
                }

                _touch_instance->config(cfg);
                _panel_instance->setTouch(_touch_instance);
            }
        }

        { // configure TFT backlight
            if (config._light.pin_bl != -1) {
#ifdef ARCH_PORTDUINO
                auto light = new portduino::Light_PWM;
#else
                auto light = new lgfx::Light_PWM;
#endif
                auto cfg = light->config();

                cfg.freq = config._light.freq;
                cfg.pin_bl = config._light.pin_bl;
                if (config._light.pwm_channel >= 0)
                    cfg.pwm_channel = config._light.pwm_channel;
                cfg.invert = config._light.invert;

                ILOG_DEBUG("Light_PWM: freq=%d, pwm=%d, bl=%d\n", cfg.freq, cfg.pwm_channel, cfg.pin_bl);
                light->config(cfg);
                _panel_instance->setLight(light);
                _light_instance = light;
            }
        }

        setPanel(_panel_instance);
#endif
    }

    ~LVGLConfig() { delete lvglDeviceDriver; }

  protected:
    lv_display_t *create(uint32_t hor_res, uint32_t ver_res) override
    {
        return nullptr; // TODO
    }

  private:
    DisplayDeviceDriver *lvglDeviceDriver;
};

#endif