#pragma once

#if __has_include(<esp_lcd_mipi_dsi.h>)

#include <esp_lcd_mipi_dsi.h>
#include <esp_lcd_panel_io.h>
#include <esp_ldo_regulator.h>

#include "lgfx/v1/Bus.hpp"
#include "lgfx/v1/platforms/common.hpp"

namespace lgfx
{
inline namespace v1
{
namespace experimental
{

class Bus_DSI : public Bus_ImagePush
{
  public:
    struct config_t {
        uint16_t lane_mbps = 0;
        uint8_t lane_num = 2;
        uint8_t bus_id = 0;
        uint32_t ldo_voltage_mv = 2500;
        uint8_t ldo_chan_id = 3;
        uint8_t lcd_cmd_bits = 8;
        uint8_t lcd_param_bits = 8;
    };

    const config_t &config(void) const { return _cfg; }
    void config(const config_t &config) { _cfg = config; }

    bus_type_t busType(void) const override { return bus_type_t::bus_dsi; }

    bool init(void) override;
    void release(void) override;

    void beginTransaction(void) override {}
    void endTransaction(void) override {}
    void flush(void) override {}

    void setImageBuffer(void *, color_depth_t) override {}
    void setBrightness(uint8_t) override {}
    void setInvert(uint8_t) override {}

    bool writeParams(uint32_t cmd, const uint8_t *param, size_t param_length);
    bool readParams(uint32_t cmd, uint8_t *param, size_t param_length);

    esp_lcd_dsi_bus_handle_t getMipiDsiBus(void) const { return _mipi_dsi_bus; }
    esp_lcd_panel_io_handle_t getPanelIo(void) const { return _io_dbi; }

  private:
    config_t _cfg;
    esp_lcd_dsi_bus_handle_t _mipi_dsi_bus = nullptr;
    esp_ldo_channel_handle_t _phy_pwr_chan = nullptr;
    esp_lcd_panel_io_handle_t _io_dbi = nullptr;
};

} // namespace experimental
} // namespace v1
} // namespace lgfx

#endif
