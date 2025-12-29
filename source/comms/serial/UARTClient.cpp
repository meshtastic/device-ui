
#include "comms/UARTClient.h"
#include "Arduino.h"
#include "util/ILog.h"

#ifndef SERIAL_BAUD
#define SERIAL_BAUD 38400
#endif

#define CONNECTION_TIMEOUT 60 // seconds
#define RX_BUFFER 1024
#define TIMEOUT 250 // ms

#define MAX_PACKET_SIZE 284

extern const uint8_t MT_MAGIC_0;

UARTClient::UARTClient(const char *name) : SerialClient(name), isActive(false), _serial(nullptr) {}

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
    ILOG_INFO("UARTClient::init SERIAL2 baud=%d", SERIAL_BAUD);
    _serial = &Serial2;
#elif SOC_UART_NUM > 1 && defined(USE_SERIAL1)
    ILOG_INFO("UARTClient::init SERIAL1 baud=%d", SERIAL_BAUD);
    _serial = &Serial1;
#elif defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT
    _serial = &Serial0;
#elif !defined(ARCH_PORTDUINO)
    ILOG_INFO("UARTClient::init SERIAL1 baud=%d", SERIAL_BAUD);
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
    ILOG_INFO("UARTClient::setPins rx=%d, tx=%d with %d baud", SERIAL_RX, SERIAL_TX, SERIAL_BAUD);
#endif
    time(&lastReceived);
    SerialClient::init();
}

bool UARTClient::connect(void)
{
    if (clientStatus != eConnected) {
        ILOG_DEBUG("serial client connecting...");
        setConnectionStatus(eConnecting, "Connecting...");
        time_t timeout = millis();
        while (!*_serial) {
            if ((millis() - timeout) > 5) {
                setConnectionStatus(eError, "Connection failed!");
                ILOG_WARN("serial client connection failed!");
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
            isActive = true;
        }

        if (isActive) {
            setConnectionStatus(eConnected, "Connected!");
            ILOG_INFO("serial client connected! (skipped %d bytes)", skipped);
        } else {
            // pretend to be connected and start sending data
            clientStatus = eConnected;
        }
    }
    return clientStatus == eConnected;
}

bool UARTClient::disconnect(void)
{
    ILOG_DEBUG("serial client disconnecting...");
    isActive = false;
    setConnectionStatus(eDisconnected, "Disconnected");
    return SerialClient::disconnect();
}

bool UARTClient::isConnected(void)
{
    time_t now;
    time(&now);
    if (now - lastReceived > CONNECTION_TIMEOUT && isActive) {
        isActive = false;
        setConnectionStatus(eDisconnected, "Disconnected");
    }
    return *_serial && (clientStatus == eConnected || isActive);
}

meshtastic_FromRadio UARTClient::receive(void)
{
    return SerialClient::receive();
}

UARTClient::~UARTClient()
{
    if (_serial) {
        _serial->end();
    }
};

// --- protected part ---

// raw write to serial UART interface
bool UARTClient::send(const uint8_t *buf, size_t len)
{
    ILOG_TRACE("sending %d bytes to radio", len);
    size_t wrote = _serial->write(buf, len);
    if (wrote != len) {
        ILOG_ERROR("only %d bytes were sent this time", wrote);
    }
    return wrote == len;
}

// raw read from serial UART interface
size_t UARTClient::receive(uint8_t *buf, size_t space_left)
{
    size_t bytes_read = 0;
    while (_serial->available() && bytes_read < MAX_PACKET_SIZE) {
        uint8_t byte = _serial->read();
        *buf++ = byte;
        if (++bytes_read >= space_left) {
            // no error, but serial thread is too slow (-> reduce sleep_time)
            ILOG_WARN("Serial overflow!");
            break;
        }
    }
    if (bytes_read > 0) {
        ILOG_TRACE("received %d bytes from serial", bytes_read);
        time(&lastReceived);
        isActive = true;
    }
    return bytes_read;
}
