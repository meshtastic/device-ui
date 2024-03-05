#pragma once

#include "MeshtasticView.h"

/**
 * @brief GUI view for T-Watch-S3
 * Handles creation of display driver and controller.
 * Note: due to static callbacks in lvgl this class is modelled as
 *       a singleton with static callback members
 */
class TFTView_480x320 : public MeshtasticView {
public:
    static TFTView_480x320* instance(void);
    virtual void init(IClientBase* client);
    virtual void task_handler(void);
    void populate_nodes(void);

    virtual void addNode(void) {};
    virtual void removeNode(void) {};
    virtual void newMessage(const char* msg) {};

protected:
    // add own message to current chat
    virtual void addMessage(char* msg) {};

private:
    TFTView_480x320();

    static TFTView_480x320* gui;
};