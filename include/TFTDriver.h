#pragma once

#include "DisplayDriver.h"

template <class TFT> class TFTDriver : public DisplayDriver
{
  public:
    TFTDriver(TFT *tft, uint16_t width, uint16_t height);
    virtual void init(DeviceGUI *gui);

  protected:
    TFT *tft;
};

template <class TFT> TFTDriver<TFT>::TFTDriver(TFT *tft, uint16_t width, uint16_t height) : DisplayDriver(width, height), tft(tft)
{
}

template <class TFT> void TFTDriver<TFT>::init(DeviceGUI *gui)
{
    DisplayDriver::init(gui);

#ifdef ARCH_ESP32
#ifdef DONT_USE_ESP_TIMER
    lv_tick_set_cb(xTaskGetTickCount);
#else
    // Create esp timer to call lvgl lv_tick_inc()
    const esp_timer_create_args_t lvgl_tick_timer_args = {.callback = [](void *arg) { lv_tick_inc(20); }, .name = "lvgl_tick"};
    esp_timer_handle_t lvgl_tick_timer = nullptr;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 20000));
#endif
#elif defined(ARCH_PORTDUINO)
    lv_tick_set_cb([]() -> uint32_t { return millis(); });
#endif
}
