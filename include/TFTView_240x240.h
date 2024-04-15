#pragma once

#include "DisplayDriverConfig.h"
#include "MeshtasticView.h"

/**
 * @brief GUI view for e.g. T-Watch-S3
 * Handles creation of display driver and controller.
 * Note: due to static callbacks in lvgl this class is modelled as
 *       a singleton with static callback members
 */
class TFTView_240x240 : public MeshtasticView
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
    static TFTView_240x240 *instance(void);
    static TFTView_240x240 *instance(const DisplayDriverConfig &cfg);
    TFTView_240x240();
    TFTView_240x240(DisplayDriver *driver);

    void ui_events_init(void);
    void ui_set_active(lv_obj_t *b, lv_obj_t *p, lv_obj_t *tp);

    // lvgl event callbacks
    // static void ui_event_HomeButton(lv_event_t * e);
    static void ui_event_NodesButton(lv_event_t *e);
    static void ui_event_GroupsButton(lv_event_t *e);
    static void ui_event_MessagesButton(lv_event_t *e);
    static void ui_event_SettingsButton(lv_event_t *e);

    static void ui_event_NodeButtonClicked(lv_event_t *e);
    static void ui_event_ChannelButtonClicked(lv_event_t *e);
    static void ui_event_Keyboard(lv_event_t *e);

    lv_obj_t *activeButton = nullptr;
    lv_obj_t *activePanel = nullptr;
    lv_obj_t *activeTopPanel = nullptr;

    static TFTView_240x240 *gui;
};
