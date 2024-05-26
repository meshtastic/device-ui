#pragma once

#include "InputDriver.h"

class TDeckInputDriver : public InputDriver
{
  public:
    TDeckInputDriver(void);
    virtual void init(void) override;
    virtual void task_handler(void) override;
    virtual ~TDeckInputDriver(void);

  private:
};
