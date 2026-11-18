#pragma once

#include "graphics/driver/DisplayDriverConfig.h"
#include "input/InputDriver.h"
#include <memory>

namespace input_policy
{
class InputPipeline;
}

class LinuxInputDriver : public InputDriver
{
  public:
    LinuxInputDriver(const std::string &kbdDevice, const std::string &ptrDevice, bool keyboardNavigation = true,
                     DisplayDriverConfig::cursor_mode_t cursorMode = DisplayDriverConfig::cursor_mode_t::AUTO);
    void init(void) override;
    void task_handler(void) override;
    virtual ~LinuxInputDriver(void);

    std::vector<std::string> getKeyboardDevices(void) override;
    std::vector<std::string> getPointerDevices(void) override;

    std::string getCurrentKeyboardDevice(void) { return keyboardDevice; }
    std::string getCurrentPointerDevice(void) { return pointerDevice; }

    bool useKeyboardDevice(const std::string &name) override;
    bool usePointerDevice(const std::string &name) override;

    bool releaseKeyboardDevice(void) override;
    bool releasePointerDevice(void) override;

  private:
    static void keyboard_read(lv_indev_t *indev, lv_indev_data_t *data);
    void initKeyboardPolicy();
    std::vector<std::string> globVector(const std::string &pattern);
    std::shared_ptr<input_policy::InputPipeline> keyboardPipeline;
    lv_indev_read_cb_t keyboardRead = nullptr;
    uint32_t pressedRawKey = 0;
    uint32_t pressedOutputKey = 0;
    bool keyPressed = false;
    const bool keyboardNavigation;
    const DisplayDriverConfig::cursor_mode_t cursorMode;
    lv_obj_t *mouse_cursor = nullptr;
};
