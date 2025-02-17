#include "graphics/DeviceScreen.h"
#include "Arduino.h"
#include "graphics/common/ViewFactory.h"
#include "util/ILog.h"

#if defined(ARDUINO_ARCH_ESP32)
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t xSemaphore = nullptr;
#endif

DeviceScreen &DeviceScreen::create(void)
{
    return *new DeviceScreen(nullptr);
}

DeviceScreen &DeviceScreen::create(const DisplayDriverConfig *cfg)
{
    return *new DeviceScreen(cfg);
}

DeviceScreen &DeviceScreen::create(DisplayDriverConfig &&cfg)
{
    return *new DeviceScreen(std::move(cfg));
}

DeviceScreen::DeviceScreen(const DisplayDriverConfig *cfg)
{
    if (cfg) {
        gui = ViewFactory::create(*cfg);
    } else {
        gui = ViewFactory::create();
    }
#if defined(ARDUINO_ARCH_ESP32)
    xSemaphore = xSemaphoreCreateMutex();
    if (!xSemaphore)
        ILOG_ERROR("DeviceScreen: xSemaphoreCreateMutex() failed");
#endif
}

DeviceScreen::DeviceScreen(DisplayDriverConfig &&cfg)
{
    gui = ViewFactory::create(cfg);
}

void DeviceScreen::init(IClientBase *client)
{
    ILOG_DEBUG("DeviceScreen::init()...");
    gui->init(client);

    // #ifdef TFT_BL
    //     digitalWrite(TFT_BL, HIGH);
    //     pinMode(TFT_BL, OUTPUT);
    // #endif

#ifdef VTFT_CTRL
    digitalWrite(VTFT_CTRL, LOW);
    pinMode(VTFT_CTRL, OUTPUT);
#endif
    ILOG_DEBUG("DeviceScreen::init() done.");
}

void DeviceScreen::task_handler(void)
{
    gui->task_handler();
}

#if defined(ARDUINO_ARCH_ESP32)
int DeviceScreen::prepareSleep(void *)
{
    if (xSemaphore)
        return xSemaphoreTake(xSemaphore, pdMS_TO_TICKS(1000)) == pdTRUE ? 0 : 1;
    else
        return 1;
}

int DeviceScreen::wakeUp(esp_sleep_wakeup_cause_t cause)
{
    if (xSemaphore)
        return xSemaphoreGive(xSemaphore) == pdTRUE ? 0 : 1;
    else
        return 1;
}
#endif

/**
 * @brief synchronisation point: here we sleep after prepareSleep() was called
 */
void DeviceScreen::sleep(void)
{
#if defined(ARDUINO_ARCH_ESP32)
    if (xSemaphore && xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
        xSemaphoreGive(xSemaphore);
#endif
    delay(5);
}
