#include "UARTClient.h"
#include "Arduino.h"
#include "Log.h"

#ifndef SERIAL_BAUD
#define SERIAL_BAUD 38400
#endif

#define RX_BUFFER 1024
#define TIMEOUT 250
#define ACK 1

extern const uint8_t MT_MAGIC_0;

UARTClient::UARTClient(void) : _serial(nullptr) {}

/**
 * @brief init serial interface
 *   Defaults:
 *   ESP32:    RX0=3,  TX0=1,  RX1=9,  TX1=10, RX2=16, TX2=17
 *   ESP32S2:  RX0=44, TX0=43, RX1=18, TX1=17
 *   ESP32C3:  RX0=20, TX0=21, RX1=18, TX1=19
 *   ESP32S3:  RX0=44, TX0=43, RX1=15, TX1=16, RX2=19, TX2=20
 */
void UARTClient::init(void)
{
#if SOC_UART_NUM > 2 && defined(USE_SERIAL2)
    ILOG_INFO("UARTClient::init SERIAL2 baud=%d\n", SERIAL_BAUD);
    _serial = &Serial2;
#elif SOC_UART_NUM > 1 && defined(USE_SERIAL1)
    ILOG_INFO("UARTClient::init SERIAL1 baud=%d\n", SERIAL_BAUD);
    _serial = &Serial1;
#elif defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT
    _serial = &Serial0;
#elif !defined(PORTDUINO)
    ILOG_INFO("UARTClient::init SERIAL1 baud=%dn", SERIAL_BAUD);
    _serial = &Serial;
#else
    _serial = nullptr;
#endif

#if defined(ARCH_ESP32)
    _serial->setTimeout(TIMEOUT);
    _serial->setRxBufferSize(RX_BUFFER);
    _serial->begin(SERIAL_BAUD);
#else
    // not supported
#endif
#ifdef SERIAL_RX
    _serial->setPins(SERIAL_RX, SERIAL_TX);
    ILOG_INFO("UARTClient::setPins rx=%d, tx=%d with %d baud\n", SERIAL_RX, SERIAL_TX, SERIAL_BAUD);
#endif
    SerialClient::init();
}

bool UARTClient::connect(void)
{
    if (!connected) {
        time_t timeout = millis();
        while (!*_serial) {
            if ((millis() - timeout) > 5) {
                connected = false;
                return false;
            }
        }

        // skip some available bytes until we see magic header
        int skipped = 0;
        while (_serial->available()) {
            if (_serial->peek() != MT_MAGIC_0) {
                _serial->read();
                skipped++;
                delay(1);
            }
        }

        connected = true;
        ILOG_TRACE("UARTClient::connected, skipped %d bytes\n", skipped);
    }
    return true;
}

bool UARTClient::disconnect(void)
{
    return SerialClient::disconnect();
}

bool UARTClient::isConnected(void)
{
    return *_serial && connected;
}

meshtastic_FromRadio UARTClient::receive(void)
{
    return SerialClient::receive();
}

UARTClient::~UARTClient(){

};

// --- protected part ---

// raw write to serial UART interface
bool UARTClient::send(const uint8_t *buf, size_t len)
{
    ILOG_TRACE("sending %d bytes to radio\n", len);
    size_t wrote = _serial->write(buf, len);
    if (wrote != len) {
        ILOG_ERROR("only %d bytes were sent this time\n", wrote);
    }
    return wrote == len;
}

// raw read from serial UART interface
size_t UARTClient::receive(uint8_t *buf, size_t space_left)
{
    size_t bytes_read = 0;
    while (_serial->available()) {
        uint8_t byte = _serial->read();
        *buf++ = byte;
        if (++bytes_read >= space_left) {
            ILOG_ERROR("Serial overflow!\n");
            break;
        }
    }
    if (bytes_read > 0) {
        ILOG_TRACE("received %d bytes from serial\n", bytes_read);
    }
    return bytes_read;
}
