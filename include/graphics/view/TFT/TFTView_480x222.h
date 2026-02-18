#pragma once

#include "graphics/view/TFT/PluggableView.h"

/**
 * @brief GUI view for e.g. T-LoRa Pager
 * Handles creation of display driver and controller.
 * Note: due to static callbacks in lvgl this class is modelled as
 *       a singleton with static callback members
 */
class TFTView_480x222 : public PluggableView
{
  public:
    void init(IClientBase *client) override;
    bool setupUIConfig(const meshtastic_DeviceUIConfig &uiconfig) override;
    void task_handler(void) override;

  protected:
    // initialize all ui screens
    virtual void ui_init(void);
    virtual void init_screens(void);
    virtual void addMessage(char *msg);
    virtual void newMessage(uint32_t nodeNum, lv_obj_t *container, uint8_t channel, const char *msg);

  private:
    // view creation only via ViewFactory
    friend class ViewFactory;
    static TFTView_480x222 *instance(void);
    static TFTView_480x222 *instance(const DisplayDriverConfig &cfg);
    TFTView_480x222();
    TFTView_480x222(const DisplayDriverConfig *cfg, DisplayDriver *driver);

    void ui_events_init(void);
};