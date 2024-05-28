#pragma once

#include "DisplayDriverConfig.h"
#include "MeshtasticView.h"

/**
 * @brief GUI view for e.g. Heltec-Tracker
 * Handles creation of display driver and controller.
 * Note: due to static callbacks in lvgl this class is modelled as
 *       a singleton with static callback members
 */
class TFTView_160x80 : public MeshtasticView
{
  public:
    void init(IClientBase *client) override;
    void task_handler(void) override;

    void addOrUpdateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                         eRole role) override
    {
    }
    void addNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                 eRole role) override
    {
    }
    void updateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                    eRole role) override
    {
    }

  protected:
    virtual void addMessage(char *msg) {}
    virtual void newMessage(uint32_t nodeNum, lv_obj_t *container, uint8_t channel, const char *msg) {}

  private:
    // view creation only via ViewFactory
    friend class ViewFactory;
    static TFTView_160x80 *instance(void);
    static TFTView_160x80 *instance(const DisplayDriverConfig &cfg);
    TFTView_160x80();
    TFTView_160x80(const DisplayDriverConfig *cfg, DisplayDriver *driver);

    static TFTView_160x80 *gui;
};