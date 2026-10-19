#if defined(ESP_PLATFORM)
#include <sdkconfig.h>
#if defined(CONFIG_IDF_TARGET_ESP32P4)

#include "graphics/LGFX/experimental/esp32p4/Bus_DSI.hpp"

namespace lgfx
{
inline namespace v1
{
namespace experimental
{

bool Bus_DSI::init(void)
{
    if (_mipi_dsi_bus) {
        return true;
    }

    esp_lcd_dsi_bus_config_t bus_config = {};
    bus_config.bus_id = _cfg.bus_id;
    bus_config.num_data_lanes = _cfg.lane_num;
    bus_config.phy_clk_src = static_cast<typeof(bus_config.phy_clk_src)>(MIPI_DSI_PHY_CLK_SRC_DEFAULT);
    bus_config.lane_bit_rate_mbps = _cfg.lane_mbps;

    esp_ldo_channel_config_t ldo_config = {};
    ldo_config.chan_id = _cfg.ldo_chan_id;
    ldo_config.voltage_mv = _cfg.ldo_voltage_mv;

    esp_lcd_dbi_io_config_t dbi_config = {};
    dbi_config.virtual_channel = 0;
    dbi_config.lcd_cmd_bits = _cfg.lcd_cmd_bits;
    dbi_config.lcd_param_bits = _cfg.lcd_param_bits;

    if (ESP_OK == esp_ldo_acquire_channel(&ldo_config, &_phy_pwr_chan) &&
        ESP_OK == esp_lcd_new_dsi_bus(&bus_config, &_mipi_dsi_bus) &&
        ESP_OK == esp_lcd_new_panel_io_dbi(_mipi_dsi_bus, &dbi_config, &_io_dbi)) {
        return true;
    }

    release();
    return false;
}

void Bus_DSI::release(void)
{
    if (_io_dbi) {
        esp_lcd_panel_io_del(_io_dbi);
        _io_dbi = nullptr;
    }
    if (_mipi_dsi_bus) {
        esp_lcd_del_dsi_bus(_mipi_dsi_bus);
        _mipi_dsi_bus = nullptr;
    }
    if (_phy_pwr_chan) {
        esp_ldo_release_channel(_phy_pwr_chan);
        _phy_pwr_chan = nullptr;
    }
}

bool Bus_DSI::writeParams(uint32_t cmd, const uint8_t *param, size_t param_length)
{
    return _io_dbi != nullptr && ESP_OK == esp_lcd_panel_io_tx_param(_io_dbi, cmd, param, param_length);
}

bool Bus_DSI::readParams(uint32_t cmd, uint8_t *param, size_t param_length)
{
    return _io_dbi != nullptr && ESP_OK == esp_lcd_panel_io_rx_param(_io_dbi, cmd, param, param_length);
}

} // namespace experimental
} // namespace v1
} // namespace lgfx

#endif
#endif
