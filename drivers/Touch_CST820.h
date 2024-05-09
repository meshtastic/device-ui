#ifndef _CST820_H
#define _CST820_H

#include "lgfx/v1/Touch.hpp"


namespace lgfx
{
 inline namespace v1
 {

/**
 * @brief CST820 I2C CTP controller driver
 * 
 */
class Touch_CST820 : public ITouch
{
    enum CS820_GESTURE {
      NONE         = 0x00,
      SWIPE_UP     = 0x01,
      SWIPE_DOWN   = 0x02,
      SWIPE_LEFT   = 0x03,
      SWIPE_RIGHT  = 0x04,
      SINGLE_CLICK = 0x05,
      DOUBLE_CLICK = 0x0B,
      LONG_PRESS   = 0x0C
    };

public:
    Touch_CST820(int8_t sda_pin = -1, int8_t scl_pin = -1, int8_t rst_pin = -1, int8_t int_pin = -1) {
      _sda = sda_pin;
      _scl = scl_pin;
      _rst = rst_pin;
      _int = int_pin;
      _cfg.i2c_addr = 0x15;
      _cfg.x_min = 0;
      _cfg.x_max = 320;
      _cfg.y_min = 0;
      _cfg.y_max = 320;
    };

    bool init(void) override;

    void wakeup(void) override {};
    void sleep(void) override {};

    uint_fast8_t getTouchRaw(touch_point_t* tp, uint_fast8_t count) override;

private:
    int8_t _sda, _scl, _rst, _int;

    uint8_t i2c_read(uint8_t addr);
    uint8_t i2c_read_continuous(uint8_t addr, uint8_t *data, uint32_t length);
    void i2c_write(uint8_t addr, uint8_t data);
    uint8_t i2c_write_continuous(uint8_t addr, const uint8_t *data, uint32_t length);
};

}
}
#endif