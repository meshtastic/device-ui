#pragma once

#if __has_include(<soc/soc_caps.h>)
#include <soc/soc_caps.h>
#if SOC_MIPI_DSI_SUPPORTED

#include <esp_lcd_mipi_dsi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "../Panel_FrameBufferBase.hpp"
#include "Bus_DSI.hpp"

namespace lgfx
{
inline namespace v1
{
namespace experimental
{

struct Panel_DSI : public Panel_FrameBufferBase {
  public:
    struct config_detail_t {
        void *buffer = nullptr;
        uint32_t buffer_length = 0;
        uint8_t num_fbs = 1;

        uint16_t dpi_freq_mhz = 60;
        uint16_t hsync_back_porch = 1;
        uint16_t hsync_pulse_width = 1;
        uint16_t hsync_front_porch = 1;
        uint16_t vsync_back_porch = 1;
        uint16_t vsync_pulse_width = 1;
        uint16_t vsync_front_porch = 1;
    };

    bool init(bool use_reset) override;
    color_depth_t setColorDepth(color_depth_t) override;

    const config_detail_t &config_detail(void) const { return _config_detail; }
    void config_detail(const config_detail_t &config_detail) { _config_detail = config_detail; }

    void setInvert(bool invert) override;
    void setSleep(bool sleep) override;
    void setPowerSave(bool power_save) override;

    void *getFrameBuffer(uint8_t index) const override;
    bool presentFrameBuffer(const void *frame_buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;
    void waitFrameBuffer() override;

    Bus_DSI *getBusDSI(void) const
    {
        auto bus = getBus();
        return bus && bus->busType() == bus_type_t::bus_dsi ? static_cast<Bus_DSI *>(bus) : nullptr;
    }

  protected:
    static constexpr uint8_t CMD_SLPIN = 0x10;
    static constexpr uint8_t CMD_SLPOUT = 0x11;
    static constexpr uint8_t CMD_INVOFF = 0x20;
    static constexpr uint8_t CMD_INVON = 0x21;
    static constexpr uint8_t CMD_DISPOFF = 0x28;
    static constexpr uint8_t CMD_DISPON = 0x29;
    static constexpr uint8_t CMD_MADCTL = 0x36;
    static constexpr uint8_t CMD_IDMOFF = 0x38;
    static constexpr uint8_t CMD_IDMON = 0x39;
    static constexpr uint8_t CMD_COLMOD = 0x3A;

    virtual const uint8_t *getInitParams(size_t) const { return nullptr; }
    virtual size_t getInitDelay(size_t) const { return 0; }

    bool write_params(uint32_t cmd, const uint8_t *data = nullptr, size_t length = 0);
    bool init_dpi(Bus_DSI *bus);
    bool init_panel(void);

    config_detail_t _config_detail;
    esp_lcd_panel_handle_t _disp_panel_handle = nullptr;

  private:
    static bool IRAM_ATTR frameBufferComplete(esp_lcd_panel_handle_t, esp_lcd_dpi_panel_event_data_t *, void *user_ctx);

    void *_frame_buffers[3] = {};
    SemaphoreHandle_t _frame_complete = nullptr;
    volatile bool _frame_pending = false;
};

} // namespace experimental
} // namespace v1
} // namespace lgfx

#endif
#endif
