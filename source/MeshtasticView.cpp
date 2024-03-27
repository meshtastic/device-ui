#include "MeshtasticView.h"
#include "ILog.h"
#include "ViewController.h"
#include "ui.h"
#include <cstdio>

extern const char *firmware_version;

MeshtasticView::MeshtasticView(DisplayDriver *driver, ViewController *_controller) : DeviceGUI(driver), controller(_controller) {}

void MeshtasticView::init(IClientBase *client)
{
    DeviceGUI::init(client);
    lv_label_set_text(ui_FirmwareLabel, firmware_version);
    controller->init(this, client);
}

void MeshtasticView::task_handler(void)
{
    DeviceGUI::task_handler();
    controller->runOnce();
};

void MeshtasticView::setMyInfo(uint32_t nodeNum) {}

void MeshtasticView::setDeviceMetaData(int hw_model, const char *version, bool has_bluetooth, bool has_wifi, bool has_eth,
                                       bool can_shutdown)
{
}

void MeshtasticView::addNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong, uint32_t lastHeard,
                             eRole role)
{
}

/**
 * @brief add or update node with unknown user
 *
 */
void MeshtasticView::addOrUpdateNode(uint32_t nodeNum, uint8_t channel, uint32_t lastHeard, eRole role)
{
    // has_user == false, generate default user name
    char userShort[5], userLong[32];
    sprintf(userShort, "%04x", nodeNum & 0xffff);
    strcpy(userLong, "Meshtastic ");
    strcat(userLong, userShort);
    addOrUpdateNode(nodeNum, channel, (const char *)&userShort[0], (const char *)&userLong[0], lastHeard, role);
}

void MeshtasticView::addOrUpdateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong,
                                     uint32_t lastHeard, eRole role)
{
}

void MeshtasticView::updateNode(uint32_t nodeNum, uint8_t channel, const char *userShort, const char *userLong,
                                uint32_t lastHeard, eRole role)
{
}

void MeshtasticView::updatePosition(uint32_t nodeNum, int32_t lat, int32_t lon, int32_t alt, uint32_t sats, uint32_t precision) {}

void MeshtasticView::updateMetrics(uint32_t nodeNum, uint32_t bat_level, float voltage, float chUtil, float airUtil) {}

void MeshtasticView::updateSignalStrength(uint32_t nodeNum, int32_t rssi, float snr) {}

void MeshtasticView::notifyReboot(bool show) {}

void MeshtasticView::showMessagePopup(const char *from) {}

void MeshtasticView::updateNodesOnline(const char *str) {}

void MeshtasticView::updateLastHeard(uint32_t nodeNum) {}

void MeshtasticView::packetReceived(const meshtastic_MeshPacket &p) {}

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
 * @return true, if heard within 15min
 */
bool MeshtasticView::lastHeartToString(uint32_t lastHeard, char *buf)
{
    time_t curtime;
    time(&curtime);
    time_t timediff = curtime - lastHeard;
    if (timediff < 60)
        strcpy(buf, "now");
    else if (timediff < 3600)
        sprintf(buf, "%d min", timediff / 60);
    else if (timediff < 3600 * 24)
        sprintf(buf, "%d h", timediff / 3600);
    else if (timediff < 3600 * 24 * 60)
        sprintf(buf, "%d d", timediff / 86400);
    else // after 60 days
        buf[0] = '\0';

    return timediff <= 910; // 15min + some tolerance
}
