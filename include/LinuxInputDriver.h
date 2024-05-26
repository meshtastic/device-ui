#pragma once

#include "InputDriver.h"

class LinuxInputDriver : public InputDriver
{
  public:
    LinuxInputDriver(void);
    virtual void init(void) override;
    virtual void task_handler(void) override;
    virtual ~LinuxInputDriver(void);

    std::vector<std::string> getKeyboardDevices(void) override;
    std::vector<std::string> getPointerDevices(void) override;

    bool useKeyboardDevice(const std::string &name) override;
    bool usePointerDevice(const std::string &name) override;

    bool releaseKeyboardDevice(void) override;
    bool releasePointerDevice(void) override;

  private:
    std::vector<std::string> globVector(const std::string &pattern);
    lv_obj_t *mouse_cursor = nullptr;
};
