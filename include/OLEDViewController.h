#pragma once

#include "IClientBase.h"
#include "ViewController.h"

class DeviceGUI;

class OLEDViewController : public ViewController {
public:
  OLEDViewController();
  virtual void init(MeshtasticView *gui, IClientBase *_client);
  virtual void runOnce(void){};
  virtual ~OLEDViewController(){};
};
