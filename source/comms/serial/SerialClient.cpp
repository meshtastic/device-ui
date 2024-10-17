#include "SerialClient.h"
#include "ILog.h"
#include "MeshEnvelope.h"
#include <time.h>
#ifdef ARCH_ESP32
#include "driver/uart.h"
#include "esp_sleep.h"
#include <assert.h>
#endif

#if defined(HAS_FREE_RTOS) || defined(ARCH_ESP32)
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#else
#include <thread>
#endif
#include "Arduino.h"

SerialClient *SerialClient::instance = nullptr;

SerialClient::SerialClient(void) : shutdown(false), connected(false), pb_size(0), bytes_read(0)
{
    buffer = new uint8_t[PB_BUFSIZE + MT_HEADER_SIZE];
    instance = this;
}

void SerialClient::init(void)
{
    ILOG_TRACE("SerialClient::init() creating serial task");
#if defined(HAS_FREE_RTOS) || defined(ARCH_ESP32)
    xTaskCreateUniversal(task_loop, "serial", 8192, NULL, 2, NULL, 0);
#elif defined(ARCH_PORTDUINO)
    std::thread *tft_task = new std::thread([] { instance->task_loop(nullptr); });
#else
// #error "unsupported architecture"
#endif
}

/**
 * @brief Do light sleep, wake on pin GPIO (touch IRQ or button press)
 *
 * @param pin
 * @return true - wake reason was due to pin GPIO
 * @return false - other reason (received serial data)
 */
bool SerialClient::sleep(int16_t pin)
{
#ifdef ARCH_ESP32
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

#if defined(WAKE_ON_SERIAL) && defined(USE_SERIAL0)
    // TODO: check: - can only work if using the refclock, which is limited to about 9600 bps
    assert(uart_set_wakeup_threshold(UART_NUM_0, 3) == ESP_OK);
    assert(esp_sleep_enable_uart_wakeup(UART_NUM_0) == ESP_OK);
#elif defined(WAKE_ON_SERIAL) && defined(USE_SERIAL1)
    assert(uart_set_wakeup_threshold(UART_NUM_1, 3) == ESP_OK);
    assert(esp_sleep_enable_uart_wakeup(UART_NUM_1) == ESP_OK);
#else
    // serial2 is unsupported for wakeup
    // TODO: set flag for possible re-syncronisation with device
#endif

#ifdef BUTTON_PIN
    pin = (gpio_num_t)BUTTON_PIN;
#if defined(BUTTON_NEED_PULLUP)
    gpio_pullup_en((gpio_num_t)pin);
#endif
    gpio_intr_disable((gpio_num_t)pin);
    gpio_wakeup_enable((gpio_num_t)pin, GPIO_INTR_LOW_LEVEL);
#else
    gpio_wakeup_enable((gpio_num_t)pin, GPIO_INTR_LOW_LEVEL);
#endif

    auto res = esp_sleep_enable_gpio_wakeup();
    if (res != ESP_OK) {
        ILOG_ERROR("esp_sleep_enable_gpio_wakeup result %d", res);
    }

    ILOG_INFO("going to sleep, wake up on GPIO%02d", pin);
    delay(5);
    res = esp_light_sleep_start();
    if (res != ESP_OK) {
        ILOG_ERROR("esp_light_sleep_start result %d", res);
    }

    // esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    // ILOG_INFO("Exit light sleep cause: %d, gpio=%d", cause, digitalRead((uint8_t)pin));
    return digitalRead((uint8_t)pin) == 0;
#else
    return false;
#endif
}

bool SerialClient::connect(void)
{
    ILOG_ERROR("SerialClient::connect() not implemented");
    return false;
}

bool SerialClient::disconnect(void)
{
    connected = false;
    return connected;
}

bool SerialClient::isConnected(void)
{
    return connected;
}

bool SerialClient::send(meshtastic_ToRadio &&to)
{
    static uint32_t id = 1;
    ILOG_TRACE("SerialClient::send() push packet %d to queue", id);
    queue.clientSend(DataPacket<meshtastic_ToRadio>(id++, to));
    return false;
}

meshtastic_FromRadio SerialClient::receive(void)
{
    if (queue.serverQueueSize() != 0) {
        ILOG_TRACE("SerialClient::receive() got a packet from queue");
        auto p = queue.clientReceive()->move();
        return static_cast<DataPacket<meshtastic_FromRadio> *>(p.get())->getData();
    }
    return meshtastic_FromRadio();
}

SerialClient::~SerialClient()
{
    shutdown = true;
};

// --- protected part ---

bool SerialClient::send(const uint8_t *buf, size_t len)
{
    ILOG_ERROR("SerialClient::send not implemented");
    return false;
}

size_t SerialClient::receive(uint8_t *buf, size_t space_left)
{
    ILOG_ERROR("SerialClient::receive not implemented");
    return 0;
}

/**
 * @brief put received bytes into envelope and send to client queue
 *
 */
void SerialClient::handlePacketReceived(void)
{
    ILOG_TRACE("SerialClient::handlePacketReceived pb_size=%d", pb_size);

    MeshEnvelope envelope(buffer, pb_size);
    meshtastic_FromRadio fromRadio = envelope.decode();
    if (fromRadio.which_payload_variant != 0) {
        queue.serverSend(DataPacket<meshtastic_FromRadio>(fromRadio.id, fromRadio));
    }
}

void SerialClient::handleSendPacket(void)
{
    auto p = queue.serverReceive()->move();
    meshtastic_ToRadio toRadio = static_cast<DataPacket<meshtastic_ToRadio> *>(p.get())->getData();
    //    meshtastic_ToRadio toRadio{std::move(static_cast<DataPacket<meshtastic_ToRadio> *>(p.get())->getData())};
    MeshEnvelope envelope;
    const std::vector<uint8_t> &pb_buf = envelope.encode(toRadio);
    if (pb_buf.size() > 0) {
        send(&pb_buf[0], pb_buf.size());
    }
}

/**
 * @brief Sending and receiving packets to/from packet queue
 *
 */
void SerialClient::task_loop(void *)
{
    size_t space_left = PB_BUFSIZE - instance->pb_size;
    ILOG_TRACE("SerialClient::task_loop running");

    while (!instance->shutdown) {
        if (instance->connected) {
            size_t bytes_read = instance->receive(&instance->buffer[instance->pb_size], space_left);
            instance->pb_size += bytes_read;
            size_t payload_len;
            bool valid = MeshEnvelope::validate(instance->buffer, instance->pb_size, payload_len);

            if (valid) {
                instance->handlePacketReceived();
                MeshEnvelope::invalidate(instance->buffer, instance->pb_size, payload_len);
            }

            // send a packet if available
            if (instance->queue.clientQueueSize() != 0) {
                instance->handleSendPacket();
            }
        }
#if defined(HAS_FREE_RTOS) || defined(ARCH_ESP32)
        vTaskDelay((TickType_t)5); // yield, do not remove
#else
        delay(5);
#endif
    }
}
