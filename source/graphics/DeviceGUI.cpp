#include "graphics/DeviceGUI.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "input/I2CKeyboardScanner.h"
#include "input/InputDriver.h"
#include <thread>

#include "input/I2CKeyboardInputDriver.h"
static I2CKeyboardInputDriver *keyboardDriver = nullptr;

#if LV_USE_LIBINPUT
#include "input/LinuxInputDriver.h"
static LinuxInputDriver *linuxInputDriver = nullptr;
#ifdef USE_X11
#include "graphics/driver/X11Driver.h"
#endif
#else
#if defined(INPUTDRIVER_ENCODER_TYPE)
#include "input/EncoderInputDriver.h"
static EncoderInputDriver *encoderDriver = nullptr;
#endif
#if defined(INPUTDRIVER_ROTARY_TYPE)
#include "input/RotaryInputDriver.h"
static RotaryInputDriver *rotaryDriver = nullptr;
#endif
#if defined(INPUTDRIVER_MATRIX_TYPE)
#include "input/KeyMatrixInputDriver.h"
static KeyMatrixInputDriver *keyMatrixDriver = nullptr;
#endif
#if defined(INPUTDRIVER_BUTTON_TYPE)
#include "input/ButtonInputDriver.h"
static ButtonInputDriver *buttonDriver = nullptr;
#endif
#endif
#include "lvgl.h"
#include "ui.h"
#include "util/ILog.h"

DeviceGUI::DeviceGUI(const DisplayDriverConfig *cfg, DisplayDriver *driver) : displaydriver(driver), inputdriver(nullptr)
{

#if LV_USE_LIBINPUT
    if (cfg)
        linuxInputDriver = new LinuxInputDriver(cfg->keyboard(), cfg->pointer());
//    else
//        linuxInputDriver = InputDriver::instance();
#else
#if defined(INPUTDRIVER_ENCODER_TYPE)
    encoderDriver = new EncoderInputDriver;
#endif
#if defined(INPUTDRIVER_ROTARY_TYPE)
    rotaryDriver = new RotaryInputDriver;
#endif
#if defined(INPUTDRIVER_MATRIX_TYPE)
    keyMatrixDriver = new KeyMatrixInputDriver;
#endif
#if defined(INPUTDRIVER_BUTTON_TYPE)
    buttonDriver = new ButtonInputDriver;
#endif
#endif
    if (!inputdriver) {
        ILOG_DEBUG("Create default InputDriver instance");
        inputdriver = InputDriver::instance();
    }
}

void DeviceGUI::init(IClientBase *client)
{
    ILOG_DEBUG("Display driver init...");
    displaydriver->init(this);

    ILOG_DEBUG("Input driver init...");
    I2CKeyboardScanner scanner;
    keyboardDriver = scanner.scan();
    if (keyboardDriver)
        keyboardDriver->init();
#if LV_USE_LIBINPUT
    if (linuxInputDriver) {
        linuxInputDriver->init();
#ifdef USE_X11
        // check if X11 driver is valid, if so get the assigned X11 keyboard
        if (X11Driver::isInitialized()) {
            lv_indev_t *kbd = X11Driver::getKeyboard();
            linuxInputDriver->setKeyboard(kbd);
        }
#endif
    }
#endif
#if defined(INPUTDRIVER_ENCODER_TYPE)
    if (encoderDriver)
        encoderDriver->init();
#endif
#if defined(INPUTDRIVER_ROTARY_TYPE)
    if (rotaryDriver)
        rotaryDriver->init();
#endif
#if defined(INPUTDRIVER_MATRIX_TYPE)
    if (keyMatrixDriver)
        keyMatrixDriver->init();
#endif
#if defined(INPUTDRIVER_BUTTON_TYPE)
    if (buttonDriver)
        buttonDriver->init();
#endif
    if (inputdriver)
        inputdriver->init();

    displaydriver->printConfig();
}

/**
 * Linux: measure how long it takes to call displaydriver->task_handler().
 * Then tell the lvgl library how long it took via lv_tick_inc().
 */
void DeviceGUI::task_handler(void)
{
#if defined(ARCH_PORTDUINO)
    int ms = 10;
    auto start = std::chrono::high_resolution_clock::now();
    displaydriver->task_handler();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    if (duration.count() < ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms - duration.count()));
        lv_tick_inc(ms);
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        lv_tick_inc(duration.count() + 1);
    }
#else
    displaydriver->task_handler();
#endif
};

DeviceGUI::~DeviceGUI()
{
    delete inputdriver;
}