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

DeviceScreen &DeviceScreen::create(ISpiLock &spiLock)
{
    // Installed before anything is constructed, so even panel init is guarded.
    ISpiLock::install(&spiLock);
    return *new DeviceScreen(nullptr);
}

DeviceScreen &DeviceScreen::create(const DisplayDriverConfig *cfg, ISpiLock *spiLock)
{
    ILOG_DEBUG("creating DeviceScreen %dx%d ...", cfg ? cfg->width() : 0, cfg ? cfg->height() : 0);
    ISpiLock::install(spiLock);
    return *new DeviceScreen(cfg);
}

DeviceScreen &DeviceScreen::create(DisplayDriverConfig &&cfg, ISpiLock *spiLock)
{
    ISpiLock::install(spiLock);
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

void DeviceScreen::toggleDisplay(void)
{
    gui->toggleDisplay();
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
void DeviceScreen::sleep(uint32_t sleepTime)
{
#if defined(ARCH_ESP32)
    if (xSemaphore && xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
        xSemaphoreGive(xSemaphore);
    vTaskDelay((TickType_t)sleepTime); // yield, do not remove
#endif
}
