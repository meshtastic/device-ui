#pragma once

#include "MeshtasticView.h"

/**
 * @brief GUI view for T-Watch-S3
 * Handles creation of display driver and controller.
 * Note: due to static callbacks in lvgl this class is modelled as
 *       a singleton with static callback members
 */
class OLEDView_128x64 : public MeshtasticView {
public:
    static OLEDView_128x64* instance(void);
    virtual void init(void);

    virtual void addNode(void) {};
    virtual void removeNode(void) {};
    virtual void newMessage(const char* msg) {};

protected:
    // add own message to current chat
    virtual void addMessage(char* msg) {};

private:
    OLEDView_128x64();

    static OLEDView_128x64* gui;
};
