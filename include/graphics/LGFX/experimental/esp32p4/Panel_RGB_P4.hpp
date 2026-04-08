#pragma once

#if defined(ESP_PLATFORM) && (defined(CONFIG_IDF_TARGET_ESP32P4) || defined(__INTELLISENSE__))

#include <cstring>

#include "lgfx/v1/Bus.hpp"
#include "lgfx/v1/panel/Panel_FrameBufferBase.hpp"
#include "lgfx/v1/platforms/common.hpp"

namespace lgfx
{
inline namespace v1
{

class Panel_RGB_P4 : public Panel_FrameBufferBase
{
  public:
    struct config_detail_t
    {
      int8_t pin_cs = -1;
      int8_t pin_sclk = -1;
      int8_t pin_mosi = -1;
      uint8_t use_psram = 2;
    };

    Panel_RGB_P4(void)
    {
      _write_depth = color_depth_t::rgb565_2Byte;
      _read_depth = color_depth_t::rgb565_2Byte;
    }

    virtual ~Panel_RGB_P4(void)
    {
      if (_lines_buffer) {
        heap_free(_lines_buffer);
        _lines_buffer = nullptr;
      }
    }

    const config_detail_t& config_detail(void) const { return _config_detail; }
    void config_detail(const config_detail_t& config_detail) { _config_detail = config_detail; }

    color_depth_t setColorDepth(color_depth_t) override { return _write_depth; }

    bool init(bool use_reset) override
    {
      if (_lines_buffer != nullptr) {
        return false;
      }
      if (!Panel_FrameBufferBase::init(use_reset)) {
        return false;
      }

      auto frame_buffer = _bus->getDMABuffer(0);
      if (frame_buffer == nullptr) {
        return false;
      }

      size_t line_array_size = _cfg.panel_height * sizeof(void*);
      auto line_array = static_cast<uint8_t**>(heap_alloc_dma(line_array_size));
      if (line_array == nullptr) {
        return false;
      }
      memset(line_array, 0, line_array_size);

      const size_t line_length = (((_cfg.panel_width * _write_bits) >> 3) + 3) & ~3;
      for (uint16_t y = 0; y < _cfg.panel_height; ++y) {
        line_array[y] = frame_buffer;
        frame_buffer += line_length;
      }

      _lines_buffer = line_array;

      int32_t pin_cs = _config_detail.pin_cs;
      if (pin_cs >= 0) {
        lgfx::gpio_hi(pin_cs);
        lgfx::pinMode(pin_cs, pin_mode_t::output);
      }

      int32_t pin_sclk = _config_detail.pin_sclk;
      if (pin_sclk >= 0) {
        lgfx::gpio_lo(pin_sclk);
        lgfx::pinMode(pin_sclk, pin_mode_t::output);
      }

      int32_t pin_mosi = _config_detail.pin_mosi;
      if (pin_mosi >= 0) {
        lgfx::gpio_lo(pin_mosi);
        lgfx::pinMode(pin_mosi, pin_mode_t::output);
      }

      return true;
    }


    void writeCommand(uint32_t, uint_fast8_t) override {}
    void writeData(uint32_t, uint_fast8_t) override {}

  private:
    config_detail_t _config_detail;
};

}  // namespace v1
}  // namespace lgfx

#endif
