#include "comms/EthClient.h"
#include "Arduino.h"
#include "comms/MeshEnvelope.h"
#include "util/ILog.h"

#if defined(ARCH_PORTDUINO)
#include "WiFi.h"
#include "WiFiClient.h"
// as of now Portduino implements the ethernet functionality via WiFiClient
class EthernetClient : public WiFiClient
{
  public:
    EthernetClient() : WiFiClient(0) {}
    virtual ~EthernetClient() {}
};
#elif HAS_ETHERNET
#include "Ethernet.h"
#include "EthernetClient.h"
#endif

#define MAX_PACKET_SIZE 284

extern const uint8_t MT_MAGIC_0;

EthClient::EthClient(const char *serverName, uint16_t port)
    : SerialClient("eth"), client(nullptr), mac{}, server(serverName), serverPort(port)
{
}

EthClient::EthClient(IPAddress server, uint16_t port)
    : SerialClient("eth"), client(nullptr), mac{}, serverIP(server), server(nullptr), serverPort(port)
{
}

EthClient::EthClient(uint8_t *mac, IPAddress ip, IPAddress server, uint16_t port)
    : SerialClient("eth"), client(nullptr), localIP(ip), serverIP(server), server(nullptr), serverPort(port)
{
    memcpy(this->mac, mac, sizeof(this->mac));
    ILOG_DEBUG("EthClient mac=%02X:%02X:%02X:%02X:%02X:%02X localIP=%d.%d.%d.%d, serverIP=%d.%d.%d.%d", mac[0], mac[1], mac[2],
               mac[3], mac[4], mac[5], localIP[0], localIP[1], localIP[2], localIP[3], serverIP[0], serverIP[1], serverIP[2],
               serverIP[3]);
}

void EthClient::init(void)
{
    ILOG_DEBUG("EthClient::init()");
#if HAS_ETHERNET
    client = new EthernetClient();

    // Ethernet.init(SS_PIN); // TODO for ESP32
    Ethernet.begin(mac, localIP);

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        ILOG_ERROR("Ethernet device not found!");
    } else if (Ethernet.linkStatus() == LinkOFF) {
        ILOG_WARN("Ethernet cable not connected!");
    }
#elif defined(ARCH_PORTDUINO)
    client = new EthernetClient();
#elif HAS_WIFI
    // client = new WiFiClient();
    // WiFi.begin(ssid);
    // if (WiFi.status() != WL_CONNECTED) {
    //     ILOG_ERROR("WiFi/Eth device not found!");
    // }
#endif
    SerialClient::init();
}

/**
 * @brief Connect to the server via ethernet socket communication links
 *
 * @return true - connected
 * @return false - not connected
 */
bool EthClient::connect(void)
{
    if (!connected) {
        bool result = false;
        if (server != nullptr) {
            ILOG_INFO("EthClient connecting to %s:%d ...", server, serverPort);
            result = client->connect(server, SERVER_PORT);
        } else {
            ILOG_INFO("EthClient connecting to %d.%d.%d.%d:%d ...", serverIP[0], serverIP[1], serverIP[2], serverIP[3],
                      serverPort);
            result = client->connect(serverIP, SERVER_PORT);
        }
        if (result) {
            ILOG_INFO("EthClient connected!");
            setConnectionStatus(true);
        } else {
            ILOG_WARN("EthClient connection failed!");
        }
    }
    return connected;
}

bool EthClient::disconnect(void)
{
    ILOG_DEBUG("EthClient disconnecting...");
    client->stop();
    return SerialClient::disconnect();
}

bool EthClient::isConnected(void)
{
    bool result = connected && client->connected();
    if (result != connected) {
        ILOG_DEBUG("EthClient connection status changed: %d", result);
        setConnectionStatus(result);
    }
    return result;
}

meshtastic_FromRadio EthClient::receive(void)
{
    return SerialClient::receive();
}

EthClient::~EthClient()
{
    disconnect();
    delete client;
};

// --- protected part ---

/**
 * @brief Send a packet to the server
 *
 * @param buf - pointer to the buffer
 * @param len - length of the buffer
 * @return true - send ok
 * @return false - send failed
 */
bool EthClient::send(const uint8_t *buf, size_t len)
{
    ILOG_TRACE("sending %d bytes to radio", len);
    int32_t wrote = 0;
#ifdef ARCH_PORTDUINO
    try {
        wrote = client->write(buf, len);
    } catch (const std::exception &e) {
        ILOG_ERROR("caught exception: %s", e.what());
    }
#else
    wrote = client->write(buf, len);
#endif

    if (wrote != (int32_t)len) {
        if (wrote < 0) {
            ILOG_ERROR("send failed, disconnecting!");
            setConnectionStatus(false);
            return false;
        }
        ILOG_ERROR("only %d bytes were sent this time", wrote);
        return false;
    }
    return wrote == (int32_t)len;
}

/**
 * @brief Receive a packet from the server if available
 *
 * @param buf - pointer to the buffer
 * @param space_left - space left in the buffer
 * @return size_t - number of bytes read
 */
size_t EthClient::receive(uint8_t *buf, size_t space_left)
{
    int bytes_read = 0;
    while (client->available() && bytes_read < MAX_PACKET_SIZE) {
        int read = client->read();
        if (read >= 0) {
            *buf++ = read & 0xff;
            if (++bytes_read >= (int)space_left) {
                ILOG_WARN("buffer overflow! (%d / %d)", bytes_read, space_left);
                break;
            }
        } else
            break; // error reading
    }
    if (bytes_read > 0) {
        ILOG_TRACE("received %d bytes via tcp", bytes_read);
    }
    return bytes_read;
}
