#pragma once

#include "IClientBase.h"

class MeshtasticView;

class ViewController
{
  public:
    ViewController();
    virtual void init(MeshtasticView *gui, IClientBase *_client);
    virtual void runOnce(void);
    virtual void processEvent(void);
    virtual void sendText(uint32_t to, uint8_t channel, const char *textmsg);
    virtual ~ViewController();

  protected:
    void requestConfig(void);
    void handleFromRadio(const meshtastic_FromRadio &from);

    MeshtasticView *view;
    IClientBase *client;
};
