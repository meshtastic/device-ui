#pragma once
#include "graphics/driver/DisplayDriver.h"

/**
 * @brief For simulation on pc/raspberry
 * This class provides an FB GUI on the local desktop; dimensions are defined
 * in lv_drv_conf.h Usage: define USE_FB=1 for the rasbian/portduino target and
 * link with -lFB
 */
class FBDriver : public DisplayDriver
{
  public:
    static FBDriver &create(uint16_t width, uint16_t height);
    void init(DeviceGUI *gui) override;
    virtual ~FBDriver() {}

  private:
    FBDriver(uint16_t width, uint16_t height);

#if LV_USE_EVDEV
    static void discovery_cb(lv_indev_t *indev, lv_evdev_type_t type, void *user_data);
    static void set_mouse_cursor_icon(lv_indev_t *indev, lv_display_t *display);
    static void indev_deleted_cb(lv_event_t *e);
#endif

    static FBDriver *fbDriver;
    static lv_display_t *display;
};