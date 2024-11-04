#pragma once

#include "stdint.h"
#include <string>

const uint16_t c_default_width = 320;
const uint16_t c_default_height = 240;

class DisplayDriver;

/**
 * @brief generic runtime configuration class for any display driver
 *
 */
class DisplayDriverConfig
{
  public:
    enum struct device_t {
        NONE,
        CUSTOM_TFT,
        CUSTOM_OLED,
        CUSTOM_EINK,
        X11,
        THMI,
        TDECK,
        INDICATOR,
        BPICOMPUTER_S3,
        TWATCH_S3,
        UNPHONE_V9,
        HELTEC_TRACKER,
        WT32_SC01_PLUS,
        ESP2432S028RV1,
        ESP2432S028RV2,
        ESP4848S040
    };

    struct panel_config_t {
        char *type = nullptr;
        uint16_t panel_width = 0;
        uint16_t panel_height = 0;
        bool rotation = false;
        uint16_t epd_mode = 0; // epd_quality, epd_text, epd_fast, epd_fastest
        int16_t pin_cs = -1;
        int16_t pin_rst = -1;
        int16_t pin_busy = -1;
        uint16_t memory_width = 0;
        uint16_t memory_height = 0;
        uint16_t offset_x = 0;
        uint16_t offset_y = 0;
        uint8_t offset_rotation = 0;
        uint8_t dummy_read_pixel = 8;
        uint8_t dummy_read_bits = 1;
        uint16_t end_read_delay_us = 0;
        bool readable = true;
        bool invert = false;
        bool rgb_order = false;
        bool dlen_16bit = false;
        bool bus_shared = true;
    };

    // SPI bus or parallel8 bus
    struct bus_config_t {
        char *type = nullptr;
        uint32_t freq_write = 16000000;
        uint32_t freq_read = 8000000;
        union {
            int8_t pin_ctrl[4] = {-1, -1, -1, -1};
            struct {
                int8_t pin_sclk;
                int8_t pin_miso;
                int8_t pin_mosi;
                int8_t pin_dc;
                uint8_t spi_mode;
                bool spi_3wire;
                bool use_lock;
                uint8_t dma_channel;
                uint16_t spi_host;
            } spi;
            struct { // parallel8 bus
                union {
                    struct {
                        int8_t pin_rd;
                        int8_t pin_wr;
                        int8_t pin_rs; // D/C
                    };
                };
                union {
                    int8_t pin_data[8];
                    struct {
                        int8_t pin_d0;
                        int8_t pin_d1;
                        int8_t pin_d2;
                        int8_t pin_d3;
                        int8_t pin_d4;
                        int8_t pin_d5;
                        int8_t pin_d6;
                        int8_t pin_d7;
                    };
                };
            } parallel;
        };
    };

    struct touch_config_t {
        char *type = nullptr;
        uint32_t freq = 1000000;
        int16_t x_min = -1;
        int16_t x_max = -1;
        int16_t y_min = -1;
        int16_t y_max = -1;
        int16_t pin_int = -1;
        int16_t pin_rst = -1;
        uint8_t offset_rotation = 0;
        bool bus_shared = true;
        union {
            struct {
                int8_t spi_host;
                int16_t pin_sclk;
                int16_t pin_mosi;
                int16_t pin_miso;
            } spi;
            struct {
                int8_t i2c_port;
                int16_t i2c_addr;
                int16_t pin_scl;
                int16_t pin_sda;
            } i2c;
        };
        int16_t pin_cs = -1;
    };

    struct input_config_t {
        std::string keyboardDevice;
        std::string pointerDevice;
    };

    struct light_config_t {
        uint32_t freq = 1200;
        int16_t pin_bl = -1;
        int8_t pwm_channel = -1;
        bool invert = false;
    };

    DisplayDriverConfig(void);
    DisplayDriverConfig(enum device_t device, uint16_t width = c_default_width, uint16_t height = c_default_height);
    DisplayDriverConfig(struct panel_config_t &&panel, struct bus_config_t &&bus, struct light_config_t &&light,
                        struct touch_config_t &&touch, struct input_config_t &&input);

    // class builder when using ctor(void)
    DisplayDriverConfig &device(enum device_t device);
    DisplayDriverConfig &panel(panel_config_t &&cfg);
    DisplayDriverConfig &bus(bus_config_t &&cfg);
    DisplayDriverConfig &touch(touch_config_t &&cfg);
    DisplayDriverConfig &input(input_config_t &&cfg);
    DisplayDriverConfig &light(light_config_t &&cfg);

    uint16_t width(void) const { return _width; }
    uint16_t height(void) const { return _height; }
    const std::string &keyboard(void) const { return _input.keyboardDevice; }
    const std::string &pointer(void) const { return _input.pointerDevice; }

  private:
    friend class DisplayDriverFactory;
    friend class LGFXConfig;

    enum device_t _device;
    panel_config_t _panel;
    bus_config_t _bus;
    touch_config_t _touch;
    input_config_t _input;
    light_config_t _light;
    uint16_t _width;
    uint16_t _height;
};
