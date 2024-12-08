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
#if 1
    lv_tick_set_cb(xTaskGetTickCount);
    lv_delay_set_cb([](uint32_t ms) { vTaskDelay(ms / portTICK_PERIOD_MS); });
#else // since lvgl 9.2.2: ***ERROR*** A stack overflow in task esp_timer has been detected.
    // Create esp timer to call lvgl lv_tick_inc()
    const esp_timer_create_args_t lvgl_tick_timer_args = {.callback = [](void *arg) { lv_tick_inc(1); }, .name = "lvgl_tick"};
    esp_timer_handle_t lvgl_tick_timer = nullptr;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 1000));
#endif
#elif defined(ARCH_PORTDUINO)
    auto tick_get_cb = []() -> uint32_t { return millis(); };
    lv_tick_set_cb(tick_get_cb);
#endif
}
