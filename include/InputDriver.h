#pragma once

#include "LVGLGraphics.h"

class InputDriver
{
  public:
    InputDriver(void);
    virtual void init(void);
    virtual void task_handler(void);

    bool hasKeyboard(void) { return keyboard != nullptr; }
    bool hasPointer(void) { return pointer != nullptr; }

    lv_indev_t* getKeyboard(void) { return keyboard; }
    lv_indev_t* getPointer(void) { return pointer; }

  private:
    lv_indev_t* keyboard = nullptr;
    lv_indev_t* pointer = nullptr;
};
