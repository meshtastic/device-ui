#include "DeviceScreen.h"

#if defined (VIEW_320x240)
#include "TFTView_320x240.h"
#elif defined (VIEW_240x240)
#include "TFTView_240x240.h"
#elif defined (VIEW_128x64)
#include "OLEDView_128x64.h"
#elif defined(WT32_SC01)
#include "TFTView_480x320.h"
#elif defined (HELTEC_TRACKER)
#include "TFTView_160x80.h"
#elif defined(USE_X11)
#include "X11Driver.h"
#else
#error "Undefined device view!"
#endif

#include "Arduino.h"

// ###################################
#if 0
static bool pmuIrq = false;

void clearPMU()
{
#ifdef USING_TWATCH_S3
    XPowersAXP2101::clearIrqStatus();
#else
    XPowersAXP202::clearIrqStatus();
#endif
}


uint64_t LilyGoLib::readPMU()
{
#ifdef USING_TWATCH_S3
    return XPowersAXP2101::getIrqStatus();
#else
    return XPowersAXP202::getIrqStatus();
#endif
}


void setPMUFlag()
{
    pmuIrq = true;
}


void settingPMU()
{
    clearPMU();

    watch.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    // Enable the required interrupt function
    watch.enableIRQ(
        // XPOWERS_AXP2101_BAT_INSERT_IRQ    | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
        XPOWERS_AXP2101_VBUS_INSERT_IRQ   | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
        XPOWERS_AXP2101_PKEY_SHORT_IRQ    | XPOWERS_AXP2101_PKEY_LONG_IRQ       |  //POWER KEY
        XPOWERS_AXP2101_BAT_CHG_DONE_IRQ  | XPOWERS_AXP2101_BAT_CHG_START_IRQ       //CHARGE
        // XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ | XPOWERS_AXP2101_PKEY_POSITIVE_IRQ   |   //POWER KEY
    );
    watch.attachPMU(setPMUFlag);

}


void PMUHandler()
{
    if (pmuIrq) {
        pmuIrq = false;
        readPMU();
        if (watch.isVbusInsertIrq()) {
            Serial.println("isVbusInsert");
            createChargeUI();
            watch.incrementalBrightness(brightnessLevel);
            usbPlugIn = true;
        }
        if (watch.isVbusRemoveIrq()) {
            Serial.println("isVbusRemove");
            destoryChargeUI();
            watch.incrementalBrightness(brightnessLevel);
            usbPlugIn = false;
        }
        if (watch.isBatChagerDoneIrq()) {
            Serial.println("isBatChagerDone");
        }
        if (watch.isBatChagerStartIrq()) {
            Serial.println("isBatChagerStart");
        }
        // Clear watch Interrupt Status Register
        watch.clearPMU();
    }
}

#endif
//####################################


DeviceScreen& DeviceScreen::create(void) {
    return *new DeviceScreen;
}

DeviceScreen::DeviceScreen() {
    Serial.println("DeviceScreen()...");
    gui = GUI_VIEW::instance();
}

#define LEDC_BACKLIGHT_CHANNEL      3
#define LEDC_BACKLIGHT_BIT_WIDTH    8
#define LEDC_BACKLIGHT_FREQ         1000
#define BOARD_TFT_BL                45

void DeviceScreen::init(void) {
    Serial.println("DeviceScreen::init()...");
    gui->init();

#ifdef TFT_BL
    digitalWrite(TFT_BL, HIGH);
    pinMode(TFT_BL, OUTPUT);
#endif

#ifdef VTFT_CTRL
    digitalWrite(VTFT_CTRL, LOW);
    pinMode(VTFT_CTRL, OUTPUT);
#endif

}

void DeviceScreen::task_handler(void) {
    gui->task_handler();
}
