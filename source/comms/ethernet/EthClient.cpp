#include "comms/EthClient.h"
#include "comms/MeshEnvelope.h"
#include "util/ILog.h"
#include "Arduino.h"

#if defined(ARCH_PORTDUINO)
#include "WiFi.h"
#include "WiFiClient.h"
// as of now Portduino implements the ethernet functionality via WiFiClient
class EthernetClient : public WiFiClient
{
  public:
    EthernetClient() : WiFiClient(0) {}
};
#elif HAS_ETHERNET
#include "Ethernet.h"
#include "EthernetClient.h"
#endif

extern const uint8_t MT_MAGIC_0;

EthClient::EthClient(const char *serverName, uint16_t port) : client(nullptr), mac{}, server(serverName), serverPort(port) {}

EthClient::EthClient(IPAddress server, uint16_t port)
    : client(nullptr), mac{}, serverIP(server), server(nullptr), serverPort(port)
{
}

EthClient::EthClient(uint8_t *mac, IPAddress ip, IPAddress server, uint16_t port)
    : client(nullptr), localIP(ip), serverIP(server), server(nullptr), serverPort(port)
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
    }
    else if (Ethernet.linkStatus() == LinkOFF) {
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

bool EthClient::connect(void)
{
    if (!connected) {
        if (server != nullptr) {
            ILOG_INFO("EthClient connecting to %s:%d ...", server, serverPort);
            connected = client->connect(server, SERVER_PORT);
        } else {
            ILOG_INFO("EthClient connecting to %d.%d.%d.%d:%d ...", serverIP[0], serverIP[1], serverIP[2], serverIP[3], serverPort);
            connected = client->connect(serverIP, SERVER_PORT);
        }
        if (connected) {
            ILOG_TRACE("EthClient connected!");

            // skip some available bytes until we see magic header
            int skipped = 0;
            while (client->available()) {
                if (client->peek() != MT_MAGIC_0) {
                    client->read();
                    skipped++;
                }
            }
            ILOG_TRACE("EthClient::connect skipped %d bytes", skipped);
        } else {
            ILOG_WARN("EthClient connection failed!");
        }
    }
    return connected;
}

bool EthClient::disconnect(void)
{
    return SerialClient::disconnect();
}

bool EthClient::isConnected(void)
{
    connected = client->connected();
    return connected;
}

meshtastic_FromRadio EthClient::receive(void)
{
    return SerialClient::receive();
}

EthClient::~EthClient()
{
    delete client;
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
#if 1
    int bytes_read = 0;
    while (client->available()) {
        uint8_t byte = client->read();
        *buf++ = byte;
        if (++bytes_read >= space_left) {
            ILOG_ERROR("buffer overflow! (%d too small)", space_left);
            break;
        }
        if (client->peek() == MT_MAGIC_0)
            break; // stop reading if we (potentially!) see next magic header
    }
    if (bytes_read > 0) {
        ILOG_TRACE("received %d bytes from tcp", bytes_read);
    }
    return bytes_read;
#else // TODO: read all available bytes
    if (client->available()) {
        int bytes_read = client->read(buf, space_left);
        if (bytes_read == 0) {
            ILOG_TRACE("received %d bytes from tcp", space_left);
            return space_left;
        } else if (bytes_read > 0) {
            ILOG_TRACE("received %d bytes from tcp", bytes_read);
            return bytes_read;
        }
    }
    return 0;
#endif
}
