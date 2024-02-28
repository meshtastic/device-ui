#pragma once

#include "IClientBase.h"

class ViewController {
public:
    ViewController();
    virtual void init(IClientBase* _client);
    virtual void runOnce(void);
    virtual ~ViewController();

protected:
    IClientBase* client;
};

