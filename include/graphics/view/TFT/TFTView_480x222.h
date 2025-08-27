#pragma once

#include "graphics/common/MeshtasticView.h"

/**
 * @brief GUI view for e.g. unPhone or WT32-SC01 Plus
 * Handles creation of display driver and controller.
 * Note: due to static callbacks in lvgl this class is modelled as
 *       a singleton with static callback members
 */
class TFTView_480x222 : public MeshtasticView
{
  public:
    void init(IClientBase *client) override;
    void task_handler(void) override;

    void addOrUpdateNode(uint32_t nodeNum, uint8_t channel, uint32_t lastHeard, const meshtastic_User &cfg) override {}
    void addNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard, eRole role,
                 bool hasKey, bool viaMqtt) override
    {
    }
    void updateNode(uint32_t nodeNum, uint8_t channel, const meshtastic_User &cfg) override {}

  protected:
    virtual void addMessage(char *msg) {}
    virtual void newMessage(uint32_t nodeNum, lv_obj_t *container, uint8_t channel, const char *msg) {}

  private:
    // view creation only via ViewFactory
    friend class ViewFactory;
    static TFTView_480x222 *instance(void);
    static TFTView_480x222 *instance(const DisplayDriverConfig &cfg);
    TFTView_480x222();
    TFTView_480x222(const DisplayDriverConfig *cfg, DisplayDriver *driver);

    static TFTView_480x222 *gui;
};