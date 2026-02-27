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
    // virtual void newMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t &msgtime);

    // widget factories for plugins
    lv_obj_t *createAddMessageWidget(lv_obj_t *parent, uint32_t msgTime, uint32_t requestId, const char *msg) override;
    lv_obj_t *createNewMessageWidget(lv_obj_t *parent, uint32_t nodeNum, uint8_t channel, const char *msg) override;
    lv_obj_t *createChatWidget(lv_obj_t *parent, uint32_t from, uint32_t to, uint8_t ch) override;

  private:
    // view creation only via ViewFactory
    friend class ViewFactory;
    static TFTView_480x222 *instance(void);
    static TFTView_480x222 *instance(const DisplayDriverConfig &cfg);
    TFTView_480x222();
    TFTView_480x222(const DisplayDriverConfig *cfg, DisplayDriver *driver);

    void ui_events_init(void);
};