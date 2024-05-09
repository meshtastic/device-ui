//TODO: preliminary version; needs to be adapted to lovyanGFX layer and maybe merged as PR

#include "Touch_CST820.h"
#include "Arduino.h"
#include "Wire.h"

#define I2C_ADDR_CST820 0x15

namespace lgfx
{
 inline namespace v1
 {

bool Touch_CST820::init(void)
{
    // Initialize I2C
    if (_sda != -1 && _scl != -1) {
        Wire.begin(_sda, _scl);
    }
    else {
        Wire.begin();
    }

    // Int Pin Configuration
    if (_int != -1) {
        pinMode(_int, OUTPUT);
        digitalWrite(_int, HIGH);
        delay(1);
        digitalWrite(_int, LOW);
        delay(1);
    }

    // Reset Pin Configuration
    if (_rst != -1) {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, LOW);
        delay(10);
        digitalWrite(_rst, HIGH);
        delay(300);
    }

    // Initialize Touch
    i2c_write(0xFE, 0XFF);
    return true;
}

uint_fast8_t Touch_CST820::getTouchRaw(touch_point_t* tp, uint_fast8_t count)
{
    uint8_t FingerIndex = i2c_read(0x02);
    uint8_t gesture = i2c_read(0x01);
    //if (!(gesture == SWIPE_UP || gesture == SWIPE_DOWN)) {
    //    gesture = NONE;
    //}

    uint8_t readdata[4];
    i2c_read_continuous(0x03, readdata, 4);
    uint16_t x = ((readdata[0] & 0x0f) << 8) | readdata[1];
    uint16_t y = ((readdata[2] & 0x0f) << 8) | readdata[3];

    tp[0].id = 0;
    tp[0].size = 1;
    tp[0].x = x; // readdata[2] | (readdata[1] & 0x0F) << 8;
    tp[0].y = y; //readdata[4] | (readdata[3] & 0x0F) << 8;

    return FingerIndex;
}

uint8_t Touch_CST820::i2c_read(uint8_t addr)
{
    uint8_t rdData = 0;
    uint8_t rdDataCount;
    uint8_t loopCount = 0;
    do {
        Wire.beginTransmission(I2C_ADDR_CST820);
        Wire.write(addr);
        Wire.endTransmission(false); // Restart
        rdDataCount = Wire.requestFrom(I2C_ADDR_CST820, 1);
    } while (rdDataCount == 0 && loopCount++ < 10);

    while (Wire.available()) {
        rdData = Wire.read();
    }
    return rdData;
}

uint8_t Touch_CST820::i2c_read_continuous(uint8_t addr, uint8_t *data, uint32_t length)
{
  Wire.beginTransmission(I2C_ADDR_CST820);
  Wire.write(addr);
  if (Wire.endTransmission(true)) return -1;
  Wire.requestFrom(I2C_ADDR_CST820, length);

  for (int i = 0; i < length; i++) {
      *data++ = Wire.read();
  }
  return 0;
}

void Touch_CST820::i2c_write(uint8_t addr, uint8_t data)
{
    Wire.beginTransmission(I2C_ADDR_CST820);
    Wire.write(addr);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t Touch_CST820::i2c_write_continuous(uint8_t addr, const uint8_t *data, uint32_t length)
{
  Wire.beginTransmission(I2C_ADDR_CST820);
  Wire.write(addr);
  for (int i = 0; i < length; i++) {
      Wire.write(*data++);
  }
  if (Wire.endTransmission(true)) return -1;
  return 0;
}


}

}