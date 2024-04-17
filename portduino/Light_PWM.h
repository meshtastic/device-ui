#include "LovyanGFX.hpp"

namespace portduino
{

class Light_PWM : public lgfx::v1::ILight
{
  public:
    struct config_t {
        uint32_t freq = 1200;
        int16_t pin_bl = -1;
        uint8_t offset = 0;
        int8_t pwm_channel = -1;

        bool invert = false;
    };

    const config_t &config(void) const { return _cfg; }
    void config(const config_t &cfg) { _cfg = cfg; }

    bool init(uint8_t brightness) override;
    void setBrightness(uint8_t brightness) override;

  private:
    config_t _cfg;
};

} // namespace portduino
