#pragma once

#include "LVGLGraphics.h"
#include <string>
#include <vector>

class InputDriver
{
  public:
    InputDriver(void) {}
    virtual void init(void) {}
    virtual void task_handler(void) {}
    virtual ~InputDriver(void);

    virtual std::vector<std::string> getKeyboardDevices(void) { return std::vector<std::string>(); }
    virtual std::vector<std::string> getPointerDevices(void) { return std::vector<std::string>(); }

    virtual bool useKeyboardDevice(const std::string &name) { return false; }
    virtual bool usePointerDevice(const std::string &name) { return false; }

    virtual bool hasKeyboardDevice(void) { return keyboard != nullptr; }
    virtual bool hasPointerDevice(void) { return pointer != nullptr; }

    virtual bool releaseKeyboardDevice(void) { return keyboard == nullptr; }
    virtual bool releasePointerDevice(void) { return pointer == nullptr; }

    virtual lv_indev_t *getKeyboard(void) { return keyboard; }
    virtual lv_indev_t *getPointer(void) { return pointer; }

  protected:
    lv_indev_t *keyboard = nullptr;
    lv_indev_t *pointer = nullptr;
};
