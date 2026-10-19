#include "graphics/LGFX/experimental/esp32p4/Panel_DSI.hpp"

#if SOC_MIPI_DSI_SUPPORTED

#include <esp_idf_version.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>

namespace lgfx
{
inline namespace v1
{
namespace experimental
{

bool Panel_DSI::init_panel(void)
{
    auto bus = getBusDSI();
    if (bus == nullptr) {
        return false;
    }

    const uint8_t *params;
    for (size_t index = 0; nullptr != (params = getInitParams(index)); ++index) {
        size_t length;
        while (0 != (length = params[0])) {
            bus->writeParams(params[1], &params[2], length - 1);
            params += length + 1;
        }
        vTaskDelay(pdMS_TO_TICKS(getInitDelay(index)));
    }

    uint8_t madctl = _cfg.rgb_order ? 0 : 1 << 3;
    uint8_t colmod = _write_bits >= 24 ? 0x77 : 0x55;
    bus->writeParams(CMD_MADCTL, &madctl, 1);
    bus->writeParams(CMD_COLMOD, &colmod, 1);

    return ESP_OK == esp_lcd_panel_init(_disp_panel_handle);
}

bool Panel_DSI::init_dpi(Bus_DSI *bus)
{
    esp_lcd_dpi_panel_config_t dpi_config = {};
    dpi_config.virtual_channel = 0;
    dpi_config.dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT;
    dpi_config.dpi_clock_freq_mhz = _config_detail.dpi_freq_mhz;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(6, 0, 0)
    dpi_config.in_color_format = LCD_COLOR_FMT_RGB565;
    dpi_config.out_color_format = LCD_COLOR_FMT_RGB565;
#else
    dpi_config.pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565;
#endif
    dpi_config.num_fbs = _config_detail.num_fbs;
    dpi_config.video_timing.h_size = _cfg.panel_width;
    dpi_config.video_timing.v_size = _cfg.panel_height;
    dpi_config.video_timing.hsync_back_porch = _config_detail.hsync_back_porch;
    dpi_config.video_timing.hsync_pulse_width = _config_detail.hsync_pulse_width;
    dpi_config.video_timing.hsync_front_porch = _config_detail.hsync_front_porch;
    dpi_config.video_timing.vsync_back_porch = _config_detail.vsync_back_porch;
    dpi_config.video_timing.vsync_pulse_width = _config_detail.vsync_pulse_width;
    dpi_config.video_timing.vsync_front_porch = _config_detail.vsync_front_porch;
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(6, 0, 0)
    dpi_config.flags.use_dma2d = _config_detail.num_fbs == 1;
#endif

    auto result = esp_lcd_new_panel_dpi(bus->getMipiDsiBus(), &dpi_config, &_disp_panel_handle);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(6, 0, 0)
    if (result == ESP_OK && _config_detail.num_fbs == 1) {
        (void)esp_lcd_dpi_panel_enable_dma2d(_disp_panel_handle);
    }
#endif
    if (result != ESP_OK) {
        return false;
    }

    switch (_config_detail.num_fbs) {
    case 1:
        result = esp_lcd_dpi_panel_get_frame_buffer(_disp_panel_handle, 1, &_frame_buffers[0]);
        break;
    case 2:
        result = esp_lcd_dpi_panel_get_frame_buffer(_disp_panel_handle, 2, &_frame_buffers[0], &_frame_buffers[1]);
        break;
    case 3:
        result =
            esp_lcd_dpi_panel_get_frame_buffer(_disp_panel_handle, 3, &_frame_buffers[0], &_frame_buffers[1], &_frame_buffers[2]);
        break;
    default:
        return false;
    }
    if (result != ESP_OK) {
        return false;
    }

    _config_detail.buffer = _frame_buffers[0];
    _config_detail.buffer_length = _cfg.panel_width * _cfg.panel_height * (_write_bits >> 3);

    if (_config_detail.num_fbs > 1) {
        _frame_complete = xSemaphoreCreateBinary();
        if (_frame_complete == nullptr) {
            return false;
        }
        esp_lcd_dpi_panel_event_callbacks_t callbacks = {};
        callbacks.on_frame_buf_complete = frameBufferComplete;
        if (ESP_OK != esp_lcd_dpi_panel_register_event_callbacks(_disp_panel_handle, &callbacks, this)) {
            return false;
        }
    }

    return true;
}

color_depth_t Panel_DSI::setColorDepth(color_depth_t)
{
    _write_depth = color_depth_t::rgb565_nonswapped;
    _read_depth = color_depth_t::rgb565_nonswapped;
    return color_depth_t::rgb565_nonswapped;
}

bool Panel_DSI::init(bool use_reset)
{
    if (_lines_buffer != nullptr || !Panel_FrameBufferBase::init(use_reset)) {
        return false;
    }

    auto bus = getBusDSI();
    if (bus == nullptr || !init_dpi(bus) || !init_panel()) {
        return false;
    }

    auto frame_buffer = static_cast<uint8_t *>(_frame_buffers[0]);
    if (frame_buffer == nullptr) {
        return false;
    }

    const auto height = _cfg.panel_height;
    auto line_array = static_cast<uint8_t **>(heap_alloc_dma(height * sizeof(void *)));
    if (line_array == nullptr) {
        return false;
    }

    const size_t line_length = ((_cfg.panel_width * _write_bits >> 3) + 3) & ~3;
    _lines_buffer = line_array;
    for (int y = 0; y < height; ++y) {
        line_array[y] = frame_buffer;
        frame_buffer += line_length;
    }

    return true;
}

void *Panel_DSI::getFrameBuffer(uint8_t index) const
{
    return index < _config_detail.num_fbs ? _frame_buffers[index] : nullptr;
}

bool Panel_DSI::presentFrameBuffer(const void *frame_buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    bool owned = false;
    for (uint8_t index = 0; index < _config_detail.num_fbs; ++index) {
        owned |= frame_buffer == _frame_buffers[index];
    }
    if (!owned || _frame_complete == nullptr) {
        return false;
    }

    xSemaphoreTake(_frame_complete, 0);
    if (ESP_OK != esp_lcd_panel_draw_bitmap(_disp_panel_handle, x, y, x + width, y + height, frame_buffer)) {
        return false;
    }
    _frame_pending = true;
    return true;
}

void Panel_DSI::waitFrameBuffer()
{
    if (_frame_pending) {
        xSemaphoreTake(_frame_complete, portMAX_DELAY);
        _frame_pending = false;
    }
}

bool IRAM_ATTR Panel_DSI::frameBufferComplete(esp_lcd_panel_handle_t, esp_lcd_dpi_panel_event_data_t *, void *user_ctx)
{
    auto panel = static_cast<Panel_DSI *>(user_ctx);
    BaseType_t task_woken = pdFALSE;
    if (panel->_frame_pending) {
        xSemaphoreGiveFromISR(panel->_frame_complete, &task_woken);
    }
    return task_woken == pdTRUE;
}

bool Panel_DSI::write_params(uint32_t cmd, const uint8_t *data, size_t length)
{
    auto bus = getBusDSI();
    if (bus == nullptr) {
        return false;
    }

    startWrite();
    bool result = bus->writeParams(cmd, data, length);
    bus->flush();
    endWrite();
    return result;
}

void Panel_DSI::setInvert(bool invert)
{
    _invert = invert;
    write_params((invert ^ _cfg.invert) ? CMD_INVON : CMD_INVOFF);
}

void Panel_DSI::setSleep(bool sleep)
{
    write_params(sleep ? CMD_SLPIN : CMD_SLPOUT);
}

void Panel_DSI::setPowerSave(bool power_save)
{
    write_params(power_save ? CMD_IDMON : CMD_IDMOFF);
}

} // namespace experimental
} // namespace v1
} // namespace lgfx

#endif
