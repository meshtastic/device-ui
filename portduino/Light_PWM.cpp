#include "Light_PWM.h"

namespace portduino
{

/**
 * Configure PWM, fallback to BL on/off if no pwm available
 */
bool Light_PWM::init(uint8_t brightness)
{
    if (_cfg.pin_bl >= 0) {
        if (_cfg.pwm_channel == -1) {
            setBrightness(brightness);
        } else {
            // ledcAttachChannel(_cfg.pin_bl, _cfg.freq, _cfg.pwm_channel);
        }
    }
    return true;
}

void Light_PWM::setBrightness(uint8_t brightness)
{
    if (_cfg.pwm_channel == -1) {
        if (brightness == 0) {
            digitalWrite(_cfg.pin_bl, _cfg.invert);
        } else {
            digitalWrite(_cfg.pin_bl, !_cfg.invert);
        }
    } else {
        uint32_t duty_cycle = _cfg.freq * 1000 * brightness / 255;
        // ledcWrite(_cfg.pin_bl, duty_cycle);
    }
}

} // namespace portduino