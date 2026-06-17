/*
 * Panel_EK79007D - LovyanGFX Panel_DSI subclass for the EK79007AD MIPI-DSI controller
 *
 * Initialization sequence extracted from the Espressif esp_lcd_ek79007 component
 * (Apache-2.0), https://github.com/espressif/esp-iot-solution
 *
 * The EK79007 needs to be initialised via MIPI DBI (command mode) before DPI
 * (video mode) streaming begins.  Without this the panel stays black even though
 * the backlight is on and all the DPI timing parameters are correct.
 * Note, you may need a delay(250) after power-up / bus init to settle.
 *
 * Sequence (2-lane, applied in order):
 *   0xB2 = 0x10   PAD_CONTROL – select 2-lane DSI
 *   0x80 – 0x86   Vendor-specific analogue trim registers
 *   0x11          Sleep Out  (+120 ms)
 *
 */
#pragma once

#include "lgfx/v1/platforms/esp32p4/Panel_DSI.hpp"

#if SOC_MIPI_DSI_SUPPORTED

namespace lgfx
{
inline namespace v1
{
//----------------------------------------------------------------------------

struct Panel_EK79007D : public Panel_DSI {
  protected:
    const uint8_t *getInitParams(size_t listno) const override
    {
        static constexpr uint8_t list0[] = {
            2,    0xB2, 0x10, // PAD_CONTROL: 2-lane DSI
            2,    0x80, 0x8B, // Vendor analogue trim
            2,    0x81, 0x78, 2,    0x82, 0x84, 2,    0x83, 0x88, 2,
            0x84, 0xA8, 2,    0x85, 0xE3, 2,    0x86, 0x88, 1,    0x11, // SLEEP OUT (no parameters)
            0,                                                          // end-of-list
        };

        switch (listno) {
        case 0:
            return list0;
        default:
            return nullptr;
        }
    }

    size_t getInitDelay(size_t listno) const override
    {
        switch (listno) {
        case 0:
            return 120; // 120 ms after Sleep Out
        default:
            return 0;
        }
    }
};

//----------------------------------------------------------------------------
} // namespace v1
} // namespace lgfx

#endif // SOC_MIPI_DSI_SUPPORTED
