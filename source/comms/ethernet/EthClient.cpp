#include "comms/EthClient.h"
#include "util/ILog.h"
#include <time.h>

#if defined(HAS_FREE_RTOS) || defined(ARCH_ESP32)
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#else
#include <thread>
#endif
#include "Arduino.h"

const size_t PB_BUFSIZE = 516;

EthClient *EthClient::instance = nullptr;

EthClient::EthClient(void) : shutdown(false), connected(false), pb_size(0), bytes_read(0), client(nullptr)
{
    // create a buffer for the incoming data
    buffer = new uint8_t[PB_BUFSIZE];
    instance = this;
}

void EthClient::init(byte *mac, IPAddress ip, IPAddress server)
{
    ILOG_DEBUG("EthClient::init()");
    client = new EthernetClient();

    // Ethernet.init(SS_PIN); // TODO for ESP32
    Ethernet.begin(mac, ip);
    serverIP = server;

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        ILOG_ERROR("Ethernet device not found");
    }
    if (Ethernet.linkStatus() == LinkOFF) {
        ILOG_WARN("Ethernet cable not connected");
    }

#if defined(HAS_FREE_RTOS) || defined(ARCH_ESP32)
    xTaskCreateUniversal(task_loop, "serial", 4096, NULL, 2, NULL, 0);
#elif defined(ARCH_PORTDUINO)
    new std::thread([] { instance->task_loop(nullptr); });
#else
// #error "unsupported architecture"
#endif
}

bool EthClient::sleep(int16_t pin)
{
    return false;
}

bool EthClient::connect(void)
{
    if (client->connect(serverIP, SERVER_PORT)) {
        ILOG_TRACE("EthClient connected to %d.%d.%d.%d", serverIP[0], serverIP[1], serverIP[2], serverIP[3]);
        connected = true;
    } else {
        ILOG_WARN("EthClient connection failed");
        connected = false;
    }
    return connected;
}

bool EthClient::disconnect(void)
{
    connected = false;
    return connected;
}

bool EthClient::isConnected(void)
{
    connected = client->connected();
    return connected;
}

bool EthClient::send(meshtastic_ToRadio &&to)
{
    static uint32_t id = 1;
    ILOG_TRACE("EthClient::send() push packet %d to queue", id);
    queue.clientSend(DataPacket<meshtastic_ToRadio>(id++, to));
    return false;
}

meshtastic_FromRadio EthClient::receive(void)
{
    if (queue.serverQueueSize() != 0) {
        ILOG_TRACE("EthClient::receive() got a packet from queue");
        auto p = queue.clientReceive()->move();
        return static_cast<DataPacket<meshtastic_FromRadio> *>(p.get())->getData();
    }
    return meshtastic_FromRadio();
}

EthClient::~EthClient()
{
    shutdown = true;
    delete client;
    delete[] buffer;
};

// --- protected part ---

bool EthClient::send(const uint8_t *buf, size_t len)
{
    ILOG_TRACE("sending %d bytes to radio", len);
    size_t wrote = client->write(buf, len);
    if (wrote != len) {
        ILOG_ERROR("only %d bytes were sent this time", wrote);
        return false;
    }
    return wrote == len;
}

size_t EthClient::receive(uint8_t *buf, size_t space_left)
{
    // TODO: handle buffer overflow
    size_t bytes_read = client->read(buf, space_left);
    if (bytes_read > 0) {
        ILOG_TRACE("received %d bytes from radio", bytes_read);
        return bytes_read;
    }
    return bytes_read;
}

/**
 * @brief put received bytes into envelope and send to client queue
 *
 */
void EthClient::handlePacketReceived(void)
{
    ILOG_TRACE("EthClient::handlePacketReceived pb_size=%d", pb_size);

    //    MeshEnvelope envelope(buffer, pb_size);
    //    meshtastic_FromRadio fromRadio = envelope.decode();
    //    if (fromRadio.which_payload_variant != 0) {
    //        queue.serverSend(DataPacket<meshtastic_FromRadio>(fromRadio.id, fromRadio));
    //    }
}

void EthClient::handleSendPacket(void)
{
    auto p = queue.serverReceive()->move();
    meshtastic_ToRadio toRadio = static_cast<DataPacket<meshtastic_ToRadio> *>(p.get())->getData();
    //    MeshEnvelope envelope;
    //    const std::vector<uint8_t> &pb_buf = envelope.encode(toRadio);
    //    if (pb_buf.size() > 0) {
    //        send(&pb_buf[0], pb_buf.size());
    //    }
}

/**
 * @brief Sending and receiving packets to/from packet queue
 *
 */
void EthClient::task_loop(void *)
{
    size_t space_left = PB_BUFSIZE - instance->pb_size;
    ILOG_TRACE("EthClient::task_loop running");

    while (!instance->shutdown) {
        if (instance->connected) {
            size_t bytes_read = instance->receive(&instance->buffer[instance->pb_size], space_left);
            instance->pb_size += bytes_read;
            size_t payload_len;
            bool valid = true; // MeshEnvelope::validate(instance->buffer, instance->pb_size, payload_len);

            if (valid) {
                instance->handlePacketReceived();
                //                MeshEnvelope::invalidate(instance->buffer, instance->pb_size, payload_len);
            }

            // send a packet if available
            if (instance->queue.clientQueueSize() != 0) {
                instance->handleSendPacket();
            }
        }
        delay(5);
    }
}
