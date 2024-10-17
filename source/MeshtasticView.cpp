#include "MeshtasticView.h"
#include "DisplayDriver.h"
#include "ILog.h"
#include "ViewController.h"
#include "ui.h"
#include "lv_i18n.h"
#include <cstdio>

MeshtasticView::MeshtasticView(const DisplayDriverConfig *cfg, DisplayDriver *driver, ViewController *_controller)
    : DeviceGUI(cfg, driver), controller(_controller), requests(c_request_timeout)
{
}

void MeshtasticView::init(IClientBase *client)
{
    DeviceGUI::init(client);
    //lv_label_set_text(objects.firmware_label, firmware_version);
    controller->init(this, client);
}

void MeshtasticView::task_handler(void)
{
    DeviceGUI::task_handler();
    controller->runOnce();

    time_t curtime;
    time(&curtime);
    if (curtime - lastrun20 >= 20) {
        lastrun20 = curtime;
        // send heartbeat to server every 20s
        if (!displaydriver->isPowersaving()) {
            controller->sendHeartbeat();
        }

        // cleanup queued requests
        requests.task_handler();
    }
};

void MeshtasticView::triggerHeartbeat(void)
{
    controller->sendHeartbeat();
}

bool MeshtasticView::sleep(int16_t pin)
{
    return controller->sleep(pin);
}

void MeshtasticView::setMyInfo(uint32_t nodeNum) {}

void MeshtasticView::setDeviceMetaData(int hw_model, const char *version, bool has_bluetooth, bool has_wifi, bool has_eth,
                                       bool can_shutdown)
{
}

void MeshtasticView::addNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                             eRole role, bool hasKey, bool viaMqtt)
{
}

/**
 * @brief add or update node with unknown user
 *
 */
void MeshtasticView::addOrUpdateNode(uint32_t nodeNum, uint8_t channel, uint32_t lastHeard, eRole role, bool hasKey, bool viaMqtt)
{
    // has_user == false, generate default user name
    char userShort[5], userLong[32];
    sprintf(userShort, "%04x", nodeNum & 0xffff);
    strcpy(userLong, "Meshtastic ");
    strcat(userLong, userShort);
    addOrUpdateNode(nodeNum, channel, (const char *)&userShort[0], (const char *)&userLong[0], lastHeard, role, hasKey, viaMqtt);
}

void MeshtasticView::addOrUpdateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong,
                                     uint32_t lastHeard, eRole role, bool hasKey, bool viaMqtt)
{
}

void MeshtasticView::updateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong,
                                uint32_t lastHeard, eRole role, bool hasKey, bool viaMqtt)
{
}

void MeshtasticView::updatePosition(uint32_t nodeNum, int32_t lat, int32_t lon, int32_t alt, uint32_t sats, uint32_t precision) {}

void MeshtasticView::updateMetrics(uint32_t nodeNum, uint32_t bat_level, float voltage, float chUtil, float airUtil) {}

void MeshtasticView::updateSignalStrength(uint32_t nodeNum, int32_t rssi, float snr) {}

void MeshtasticView::notifyResync(bool show) {}

void MeshtasticView::notifyReboot(bool show) {}

void MeshtasticView::notifyShutdown(void) {}

void MeshtasticView::showMessagePopup(const char *from) {}

void MeshtasticView::updateLastHeard(uint32_t nodeNum) {}

void MeshtasticView::packetReceived(const meshtastic_MeshPacket &p)
{
    // if there's a message from a node we don't know (yet), create it with defaults
    auto it = nodes.find(p.from);
    if (it == nodes.end()) {
        MeshtasticView::addOrUpdateNode(p.from, p.channel, 0, eRole::unknown, false, false);
        updateLastHeard(p.from);
    }
    if (p.to != ownNode && p.to != 0xffffffff) {
        auto it = nodes.find(p.to);
        if (it == nodes.end()) {
            MeshtasticView::addOrUpdateNode(p.to, p.channel, 0, eRole::unknown, false, false);
            updateLastHeard(p.to);
        }
    }
}

void MeshtasticView::newMessage(uint32_t from, uint32_t to, uint8_t channel, const char *msg) {}

void MeshtasticView::removeNode(uint32_t nodeNum) {}

// -------- helpers --------
/**
 * @brief Returns background and foregroud color for a node
 *
 * @param nodeNum
 * @return std::tuple<uint32_t, uint32_t>
 */
std::tuple<uint32_t, uint32_t> MeshtasticView::nodeColor(uint32_t nodeNum)
{
    uint32_t red = (nodeNum & 0xff0000) >> 16;
    uint32_t green = (nodeNum & 0xff00) >> 8;
    uint32_t blue = (nodeNum & 0xff);
    while (red + green + blue < 0xF0) {
        red += red / 3 + 10;
        green += green / 3 + 10;
        blue += blue / 3 + 10;
    }

    return std::make_tuple((red << 16) | (green << 8) | blue, (2 * red + 2 * green + blue) > 600 ? 0x000000 : 0xFFFFFF);
}

/**
 * @brief
 *
 * @param lastHeard
 * @param buf - result string
 * @return true, if heard within 30min
 */
bool MeshtasticView::lastHeardToString(uint32_t lastHeard, char *buf)
{
    time_t curtime;
    time(&curtime);
    time_t timediff = curtime - lastHeard;
    if (timediff < 60)
        strcpy(buf, _("now"));
    else if (timediff < 3600)
        sprintf(buf, "%d min", timediff / 60);
    else if (timediff < 3600 * 24)
        sprintf(buf, "%d h", timediff / 3600);
    else if (timediff < 3600 * 24 * 60)
        sprintf(buf, "%d d", timediff / 86400);
    else // after 60 days
        buf[0] = '\0';

    return timediff <= secs_until_offline;
}

const char *MeshtasticView::deviceRoleToString(enum eRole role)
{
    switch (role) {
    case client:
        return "Client";
    case client_mute:
        return "Client Mute";
    case router:
        return "Router";
    case router_client:
        return "Router Client";
    case repeater:
        return "Repeater";
    case tracker:
        return "Tracker";
    case sensor:
        return "Sensor";
    case tak:
        return "TAK";
    case client_hidden:
        return "Client Hidden";
    case lost_and_found:
        return "Lost & Found";
    case tak_tracker:
        return "TAK Tracker";
    default:
        ILOG_ERROR("Invalid device role");
        return "<unknown>";
    };
}

#include "macaron_Base64.h"
#include <cstring>
std::string MeshtasticView::pskToBase64(const meshtastic_ChannelSettings_psk_t &psk)
{
    if (psk.size > 0) {
        return macaron::Base64::Encode(&psk.bytes[0], psk.size);
    } else {
        return "";
    }
}

bool MeshtasticView::base64ToPsk(const std::string &base64, meshtastic_ChannelSettings_psk_t &psk)
{
    std::string out;
    auto error = macaron::Base64::Decode(base64, out);
    if (!error.empty()) {
        ILOG_ERROR("Cannot decode '%s'", base64);
        return false;
    } else {
        memcpy((char *)&psk.bytes[0], out.data(), out.size());
        psk.size = out.size();
    }
    return true;
}
