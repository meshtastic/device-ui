#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <TouchDrvCSTXXX.hpp>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#ifndef SPI_FREQUENCY
#define SPI_FREQUENCY 75000000
#endif

#define TOUCH_INT 21
#define TOUCH_RST 13
#define HW_IRQ_TOUCHPAD (_BV(0))

class LGFX_Touch : public lgfx::LGFX_Device
{
  public:
    bool init_impl(bool use_reset, bool use_clear) override
    {
        bool result = LGFX_Device::init_impl(use_reset, use_clear);
        lgfx::pinMode(TOUCH_INT, lgfx::pin_mode_t::input_pullup);
        touchDrv.setPins(TOUCH_RST, TOUCH_INT);
        result |= touchDrv.begin(Wire, 0x5A, SDA, SCL);
        if (!result) {
            ILOG_ERROR("Failed to initialise touch panel!");
        } else {
            eventGrp = xEventGroupCreate();
            attachInterrupt(
                TOUCH_INT,
                []() {
                    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                    xEventGroupSetBitsFromISR(eventGrp, HW_IRQ_TOUCHPAD, &xHigherPriorityTaskWoken);
                },
                FALLING);
        }
        return result;
    }

    LGFX_Touch *touch(void) { return this; }

    int8_t getTouchInt(void) { return TOUCH_INT; }

    // unfortunately not declared as virtual in base class, need to choose a different name
    bool getTouchXY(uint16_t *touchX, uint16_t *touchY)
    {
        EventBits_t bits = xEventGroupGetBits(eventGrp);
        if (bits & HW_IRQ_TOUCHPAD) {
            uint16_t x = 0, y = 0;
            uint8_t tp = touchDrv.getPoint((int16_t *)&x, (int16_t *)&y, 1);
            if (tp) {
                // Rotate coordinates by 90 degrees clockwise (-> landscape)
                *touchX = y;
                *touchY = 222 - x;
            } else {
                xEventGroupClearBits(eventGrp, HW_IRQ_TOUCHPAD);
            }
            return tp;
        }
        return 0;
    }

    void wakeup(void) {}
    void sleep(void)
    { /* touchDrv.sleep(); */
    }

  private:
    TouchDrvCST92xx touchDrv;
    static EventGroupHandle_t eventGrp;
};

EventGroupHandle_t LGFX_Touch::eventGrp = NULL;

class LGFX_TDISPLAY_S3PRO : public LGFX_Touch
{
    lgfx::Panel_ST7796 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;

  public:
    const uint32_t screenWidth = 480;
    const uint32_t screenHeight = 222;

    bool hasButton(void) { return true; }

    LGFX_TDISPLAY_S3PRO(void)
    {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = SPI_FREQUENCY;
            cfg.freq_read = 16000000;
            cfg.spi_3wire = false;
            cfg.use_lock = false;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk = 18;
            cfg.pin_mosi = 17;
            cfg.pin_miso = 8;
            cfg.pin_dc = 9;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {
            auto cfg = _panel_instance.config();

            cfg.pin_cs = 39;
            cfg.pin_rst = 47;
            cfg.pin_busy = -1;

            cfg.panel_width = screenHeight;
            cfg.panel_height = screenWidth;
            cfg.offset_x = 49;
            cfg.offset_y = 0;
            cfg.offset_rotation = 1;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = true;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = false;

            _panel_instance.config(cfg);
        }

        {
            auto cfg = _light_instance.config();

            cfg.pin_bl = 48;
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        setPanel(&_panel_instance);
    }
};
