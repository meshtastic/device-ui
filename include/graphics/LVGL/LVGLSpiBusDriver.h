#pragma once

#ifdef LVGL_DRIVER

#include "SPI.h"
#include "drivers/display/lcd/lv_lcd_generic_mipi.h"
#include "graphics/driver/DisplayDeviceDriver.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include <functional>

class LVGLSpiBusDriver
{
  public:
    using CreateCB =
        std::function<lv_display_t *(uint32_t, uint32_t, lv_lcd_flag_t, lv_lcd_send_cmd_cb_t, lv_lcd_send_color_cb_t)>;

    LVGLSpiBusDriver(uint16_t width, uint16_t height, CreateCB createDisplayFunc);
    void init(int16_t spiBus, int16_t sclk, int16_t mosi, int16_t miso, int16_t dc, int16_t rst, int16_t cs);
    lv_display_t *create(uint32_t hor_res, uint32_t ver_res);
    virtual ~LVGLSpiBusDriver()
    {
        if (spi)
            delete spi;
    }

  protected:
    LVGLSpiBusDriver() = default;
    virtual void connect(void);

  private:
    LVGLSpiBusDriver(const LVGLSpiBusDriver &) = delete;
    LVGLSpiBusDriver &operator=(const LVGLSpiBusDriver &) = delete;

    static void lcd_send_cmd_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size);
    static void lcd_send_color_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size);

    int8_t rst;
    int8_t dc;
    int8_t cs;
    int8_t sclk;
    int8_t mosi;
    int8_t miso;
    int8_t spiBus;
    SPISettings spiSettings;
    static SPIClass *spi;
    static LVGLSpiBusDriver *instance;
    CreateCB createDisplay;
};

#endif