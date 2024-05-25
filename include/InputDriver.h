#pragma once

#include "LVGLGraphics.h"
#include <string>
#include <vector>

class InputDriver
{
  public:
    InputDriver(void);
    virtual void init(void);
    virtual void task_handler(void);

    std::vector<std::string> getKeyboardDevices(void);
    std::vector<std::string> getPointerDevices(void);

    bool useKeyboardDevice(const std::string &event);
    bool usePointerDevice(const std::string &event);

    bool hasKeyboardDevice(void);
    bool hasPointerDevice(void);

    bool releaseKeyboardDevice(void);
    bool releasePointerDevice(void);

    lv_indev_t *getKeyboard(void) { return keyboard; }
    lv_indev_t *getPointer(void) { return pointer; }

  private:
    std::vector<std::string> globVector(const std::string &pattern);

    lv_indev_t *keyboard = nullptr;
    lv_indev_t *pointer = nullptr;
    char *mouse_path = nullptr;
    char *keyboard_path = nullptr;
};
