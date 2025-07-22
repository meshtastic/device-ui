#include "graphics/common/MeshtasticView.h"
#include "graphics/common/ViewController.h"
#include "graphics/driver/DisplayDriver.h"
#include "lv_i18n.h"
#include "ui.h"
#include "util/ILog.h"
#include <cstdio>

MeshtasticView::MeshtasticView(const DisplayDriverConfig *cfg, DisplayDriver *driver, ViewController *_controller)
    : DeviceGUI(cfg, driver), controller(_controller), requests(c_request_timeout)
{
}

void MeshtasticView::init(IClientBase *client)
{
    DeviceGUI::init(client);
    // lv_label_set_text(objects.firmware_label, firmware_version);
    controller->init(this, client);
    time(&lastrun20);
}

void MeshtasticView::task_handler(void)
{
    DeviceGUI::task_handler();
    controller->runOnce();

    time(&curtime);
    if (curtime - lastrun20 >= 20) {
        lastrun20 = curtime;
        // send heartbeat to server every 20s
        if (!displaydriver->isPowersaving() || state == eProgrammingMode) {
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
                             eRole role, bool hasKey, bool unmessagable)
{
}

/**
 * @brief add or update node with unknown user
 *
 */
void MeshtasticView::addOrUpdateNode(uint32_t nodeNum, uint8_t channel, uint32_t lastHeard, eRole role, bool hasKey, bool viaMqtt)
{
    // has_user == false, generate default user name
    meshtastic_User user{};
    sprintf(user.short_name, "%04x", nodeNum & 0xffff);
    strcpy(user.long_name, "Meshtastic ");
    strcat(user.long_name, user.short_name);
    user.role = (meshtastic_Config_DeviceConfig_Role)role;
    user.hw_model = meshtastic_HardwareModel_UNSET;
    addOrUpdateNode(nodeNum, channel, lastHeard, user);
}

void MeshtasticView::addOrUpdateNode(uint32_t nodeNum, uint8_t channel, uint32_t lastHeard, const meshtastic_User &cfg) {}

void MeshtasticView::updateNode(uint32_t nodeNum, uint8_t channel, const meshtastic_User &cfg) {}

void MeshtasticView::updatePosition(uint32_t nodeNum, int32_t lat, int32_t lon, int32_t alt, uint32_t sats, uint32_t precision) {}

void MeshtasticView::updateMetrics(uint32_t nodeNum, uint32_t bat_level, float voltage, float chUtil, float airUtil) {}

void MeshtasticView::updateSignalStrength(uint32_t nodeNum, int32_t rssi, float snr) {}

void MeshtasticView::notifyMessagesRestored(void)
{
    messagesRestored = true;
    state = eRunning;
}

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
        sprintf(buf, "%ld min", timediff / 60);
    else if (timediff < 3600 * 24)
        sprintf(buf, "%ld h", timediff / 3600);
    else if (timediff < 3600 * 24 * 60)
        sprintf(buf, "%ld d", timediff / 86400);
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
        ILOG_ERROR("Invalid device role: %d", role);
        return "<unknown>";
    };
}

#include "util/macaron_Base64.h"
#include <cstring>
std::string MeshtasticView::pskToBase64(uint8_t *bytes, uint32_t size)
{
    if (size > 0) {
        return macaron::Base64::Encode(bytes, size);
    } else {
        return "";
    }
}

bool MeshtasticView::base64ToPsk(const std::string &base64, uint8_t *bytes, uint16_t &size)
{
    std::string out;
    auto error = macaron::Base64::Decode(base64, out);
    if (!error.empty()) {
        ILOG_ERROR("Cannot decode '%s'", base64);
        return false;
    } else {
        memcpy((char *)bytes, out.data(), out.size());
        size = out.size();
    }
    return true;
}
