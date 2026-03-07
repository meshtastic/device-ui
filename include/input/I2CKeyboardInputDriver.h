#pragma once

#include "input/InputDriver.h"
#include <list>
#include <memory>
#include <string>

// Callback type for navigation events (e.g., backspace -> focus home button)
typedef void (*NavigationCallback)(void);

class I2CKeyboardInputDriver : public InputDriver
{
  public:
    I2CKeyboardInputDriver(void);
    virtual void init(void) override;
    virtual void task_handler(void) override{};
    virtual void readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data) = 0;
    void onScreenSleep(void) override;
    void onScreenWake(void) override;
    virtual ~I2CKeyboardInputDriver(void) {}

    struct KeyboardDefinition {
        I2CKeyboardInputDriver *driver; // Pointer to the driver instance
        std::string name;               // Name of the keyboard
        uint8_t address;                // I2C address of the keyboard
    };

    using KeyboardList = std::list<std::unique_ptr<KeyboardDefinition>>;
    static KeyboardList &getI2CKeyboardList(void) { return i2cKeyboardList; }

    // Navigation callback for backspace when not in a text field
    static void setNavigateHomeCallback(NavigationCallback cb) { navigateHomeCallback = cb; }

  protected:
    bool registerI2CKeyboard(I2CKeyboardInputDriver *driver, std::string name, uint8_t address);
    void initKeyboardBacklight(uint8_t pin, uint8_t channel = 4);
    void setKeyboardBacklight(uint8_t brightness);
    uint8_t kbBlBrightness = 0;

    static NavigationCallback navigateHomeCallback;

  private:
    static void keyboard_read(lv_indev_t *indev, lv_indev_data_t *data);

    static KeyboardList i2cKeyboardList; // list of registered I2C keyboards
    uint8_t kbBlPin = 0;
    uint8_t kbBlChannel = 4;
    uint8_t kbBlSavedBrightness = 0;
};

class TDeckKeyboardInputDriver : public I2CKeyboardInputDriver
{
  public:
    TDeckKeyboardInputDriver(uint8_t address);
    void readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data) override;
    virtual ~TDeckKeyboardInputDriver(void) {}
};

class TCA8418KeyboardInputDriver : public I2CKeyboardInputDriver
{
  public:
    TCA8418KeyboardInputDriver(uint8_t address);
    void init(void) override;
    void readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data) override;
    virtual ~TCA8418KeyboardInputDriver(void) {}
};

class TLoraPagerKeyboardInputDriver : public TCA8418KeyboardInputDriver
{
  public:
    TLoraPagerKeyboardInputDriver(uint8_t address);
    void init(void) override;
    void readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data) override;
    virtual ~TLoraPagerKeyboardInputDriver(void) {}

  private:
    uint8_t kbBlStep = 0;
};

class TDeckProKeyboardInputDriver : public TCA8418KeyboardInputDriver
{
  public:
    TDeckProKeyboardInputDriver(uint8_t address);
    void init(void) override;
    void readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data) override;
    virtual ~TDeckProKeyboardInputDriver(void) {}
};

class BBQ10KeyboardInputDriver : public I2CKeyboardInputDriver
{
  public:
    BBQ10KeyboardInputDriver(uint8_t address);
    void init(void) override;
    void readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data) override;
    virtual ~BBQ10KeyboardInputDriver(void) {}
};

class CardKBInputDriver : public I2CKeyboardInputDriver
{
  public:
    CardKBInputDriver(uint8_t address);
    void readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data) override;
    virtual ~CardKBInputDriver(void) {}
};

class MPR121KeyboardInputDriver : public I2CKeyboardInputDriver
{
  public:
    MPR121KeyboardInputDriver(uint8_t address);
    void init(void) override;
    void readKeyboard(uint8_t address, lv_indev_t *indev, lv_indev_data_t *data) override;
    virtual ~MPR121KeyboardInputDriver(void) {}
};
