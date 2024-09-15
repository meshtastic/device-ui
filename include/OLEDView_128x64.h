#pragma once

#include "DisplayDriverConfig.h"
#include "MeshtasticView.h"

/**
 * @brief GUI view for T-Watch-S3
 * Handles creation of display driver and controller.
 * Note: due to static callbacks in lvgl this class is modelled as
 *       a singleton with static callback members
 */
class OLEDView_128x64 : public MeshtasticView
{
  public:
    void init(IClientBase *client) override;
    void task_handler(void) override;

    void addOrUpdateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                         eRole role, bool hasKey, bool viaMqtt) override
    {
    }
    void addNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard, eRole role,
                 bool hasKey, bool viaMqtt) override
    {
    }
    void updateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                    eRole role, bool hasKey, bool viaMqtt) override
    {
    }

  protected:
    virtual void addMessage(char *msg) {}
    virtual void newMessage(uint32_t nodeNum, lv_obj_t *container, uint8_t channel, const char *msg) {}

  private:
    // view creation only via ViewFactory
    friend class ViewFactory;
    static OLEDView_128x64 *instance(void);
    static OLEDView_128x64 *instance(const DisplayDriverConfig &cfg);
    OLEDView_128x64();
    OLEDView_128x64(const DisplayDriverConfig *cfg, DisplayDriver *driver);

    static OLEDView_128x64 *gui;
};
