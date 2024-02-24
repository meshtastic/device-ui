#pragma once

#include "DeviceGUI.h"
#include "lvgl.h"

/**
 * @brief GUI view for T-Deck
 * Handles creation of display driver and controller.
 * Note: due to static callbacks in lvgl this class is modelled as
 *       a singleton with static callback members
 */
class TFTView_320x240 : public DeviceGUI {
public:
    static TFTView_320x240* instance(void);
    virtual void init(void);
    virtual void task_handler(void);
    void populate_nodes(void);

    virtual void addNode(void) {};
    virtual void removeNode(void) {};
    virtual void newMessage(const char* msg) {};

protected:
    // add own message to current chat
    virtual void addMessage(char* msg);

private:
    TFTView_320x240();

    void ui_events_init(void);
    void ui_set_active(lv_obj_t* b, lv_obj_t* p, lv_obj_t* tp);

    // lvgl event callbacks
    //static void ui_event_HomeButton(lv_event_t * e);
    static void ui_event_NodesButton( lv_event_t * e);
    static void ui_event_GroupsButton( lv_event_t * e);
    static void ui_event_MessagesButton( lv_event_t * e);
    static void ui_event_MapButton( lv_event_t * e);
    static void ui_event_SettingsButton( lv_event_t * e);

    static void ui_event_NodeButtonClicked(lv_event_t * e);
    static void ui_event_ChannelButtonClicked(lv_event_t * e);
    static void ui_event_Keyboard(lv_event_t * e);

    lv_obj_t *activeButton = nullptr;
    lv_obj_t *activePanel = nullptr;
    lv_obj_t *activeTopPanel = nullptr;

    static TFTView_320x240* gui;
};

