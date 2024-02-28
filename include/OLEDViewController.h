#pragma once

#include "ViewController.h"
#include "IClientBase.h"

class OLEDViewController : public ViewController {
public:
    OLEDViewController();
    virtual void init(IClientBase* _client);
    virtual void runOnce(void);
    virtual ~OLEDViewController();

};

