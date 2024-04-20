#include "ViewController.h"
#include "ILog.h"
#include "MeshtasticView.h"
#include "assert.h"
#include <algorithm>

/**
 * @brief mediate between GUI view and client interface
 *
 */
ViewController::ViewController() : view(nullptr), client(nullptr), sendId(1), requestConfigRequired(true) {}

void ViewController::init(MeshtasticView *gui, IClientBase *_client)
{
    time(&lastrun10);
    lastrun10 += 10;
    view = gui;
    client = _client;
    if (client) {
        client->init();
        client->connect();
    }
}

/**
 * @brief runOnce need to be called periodically to process send/receive queues
 *
 */
void ViewController::runOnce(void)
{
    if (client) {
        requestConfig();
        receive();

        // executed every 10s:
        time_t curtime;
        time(&curtime);

        if (curtime - lastrun10 >= 10) {
            lastrun10 = curtime;
            requestDeviceConnectionStatus();
            if (!client->isConnected())
                client->connect();
        }
    }
}

bool ViewController::sleep(int16_t pin)
{
    if (client)
        return client->sleep(pin);
    else
        return false;
}

void ViewController::processEvent(void) {}

void ViewController::sendText(uint32_t to, uint8_t ch, const char *textmsg)
{
    assert(strlen(textmsg) <= (size_t)meshtastic_Constants_DATA_PAYLOAD_LEN);
    send(to, ch, meshtastic_PortNum_TEXT_MESSAGE_APP, (const uint8_t *)textmsg, strlen(textmsg));
}

void ViewController::sendConfig(void) {}

void ViewController::sendHeartbeat(void)
{
    if (client->isConnected()) {
        client->send(meshtastic_ToRadio{.which_payload_variant = meshtastic_ToRadio_heartbeat_tag});
    }
}

void ViewController::setConfigRequested(bool required)
{
    requestConfigRequired = required;
}

/**
 * generic send method for sending meshpackets with encoded payload
 */
bool ViewController::send(uint32_t to, uint8_t ch, meshtastic_PortNum portnum, const unsigned char *bytes, size_t len)
{
    ILOG_DEBUG("sending meshpacket to radio to=0x%08x(%u), ch=%u, portnum=%u, len=%u\n", to, to, (unsigned int)ch, portnum, len);
    // send requires movable lvalue, i.e. a temporary object
    return client->send(meshtastic_ToRadio{
        .which_payload_variant = meshtastic_ToRadio_packet_tag,
        .packet{
            .to = to,
            .channel = ch,
            .which_payload_variant = meshtastic_MeshPacket_decoded_tag,
            .decoded{
                .portnum = portnum,
                .payload{.size = (unsigned short)len,
                         .bytes = {bytes[0],   bytes[1],   bytes[2],   bytes[3],   bytes[4],   bytes[5],   bytes[6],   bytes[7],
                                   bytes[8],   bytes[9],   bytes[10],  bytes[11],  bytes[12],  bytes[13],  bytes[14],  bytes[15],
                                   bytes[16],  bytes[17],  bytes[18],  bytes[19],  bytes[20],  bytes[21],  bytes[22],  bytes[23],
                                   bytes[24],  bytes[25],  bytes[26],  bytes[27],  bytes[28],  bytes[29],  bytes[30],  bytes[31],
                                   bytes[32],  bytes[33],  bytes[34],  bytes[35],  bytes[36],  bytes[37],  bytes[38],  bytes[39],
                                   bytes[40],  bytes[41],  bytes[42],  bytes[43],  bytes[44],  bytes[45],  bytes[46],  bytes[47],
                                   bytes[48],  bytes[49],  bytes[50],  bytes[51],  bytes[52],  bytes[53],  bytes[54],  bytes[55],
                                   bytes[56],  bytes[57],  bytes[58],  bytes[59],  bytes[60],  bytes[61],  bytes[62],  bytes[63],
                                   bytes[64],  bytes[65],  bytes[66],  bytes[67],  bytes[68],  bytes[69],  bytes[70],  bytes[71],
                                   bytes[72],  bytes[73],  bytes[74],  bytes[75],  bytes[76],  bytes[77],  bytes[78],  bytes[79],
                                   bytes[80],  bytes[81],  bytes[82],  bytes[83],  bytes[84],  bytes[85],  bytes[86],  bytes[87],
                                   bytes[88],  bytes[89],  bytes[90],  bytes[91],  bytes[92],  bytes[93],  bytes[94],  bytes[95],
                                   bytes[96],  bytes[97],  bytes[98],  bytes[99],  bytes[100], bytes[101], bytes[102], bytes[103],
                                   bytes[104], bytes[105], bytes[106], bytes[107], bytes[108], bytes[109], bytes[110], bytes[111],
                                   bytes[112], bytes[113], bytes[114], bytes[115], bytes[116], bytes[117], bytes[118], bytes[119],
                                   bytes[120], bytes[121], bytes[122], bytes[123], bytes[124], bytes[125], bytes[126], bytes[127],
                                   bytes[128], bytes[129], bytes[130], bytes[131], bytes[132], bytes[133], bytes[134], bytes[135],
                                   bytes[136], bytes[137], bytes[138], bytes[139], bytes[140], bytes[141], bytes[142], bytes[143],
                                   bytes[144], bytes[145], bytes[146], bytes[147], bytes[148], bytes[149], bytes[150], bytes[151],
                                   bytes[152], bytes[153], bytes[154], bytes[155], bytes[156], bytes[157], bytes[158], bytes[159],
                                   bytes[160], bytes[161], bytes[162], bytes[163], bytes[164], bytes[165], bytes[166], bytes[167],
                                   bytes[168], bytes[169], bytes[170], bytes[171], bytes[172], bytes[173], bytes[174], bytes[175],
                                   bytes[176], bytes[177], bytes[178], bytes[179], bytes[180], bytes[181], bytes[182], bytes[183],
                                   bytes[184], bytes[185], bytes[186], bytes[187], bytes[188], bytes[189], bytes[190], bytes[191],
                                   bytes[192], bytes[193], bytes[194], bytes[195], bytes[196], bytes[197], bytes[198], bytes[199],
                                   bytes[200], bytes[201], bytes[202], bytes[203], bytes[204], bytes[205], bytes[206], bytes[207],
                                   bytes[208], bytes[209], bytes[210], bytes[211], bytes[212], bytes[213], bytes[214], bytes[215],
                                   bytes[216], bytes[217], bytes[218], bytes[219], bytes[220], bytes[221], bytes[222], bytes[223],
                                   bytes[224], bytes[225], bytes[226], bytes[227], bytes[228], bytes[229], bytes[230], bytes[231],
                                   bytes[232], bytes[233], bytes[234], bytes[235], bytes[236]}},
                .want_response = true},
            .want_ack = (to != 0 && to != UINT32_MAX)}});
}

bool ViewController::receive(void)
{
    bool gotPacket = false;
    if (client->isConnected()) {
        do {
            meshtastic_FromRadio from = client->receive();
            if (from.which_payload_variant) {
                handleFromRadio(from);
            }
            gotPacket = from.which_payload_variant != 0;
        } while (gotPacket);
        return true;
    }
    return false;
}

/**
 * request full upload of all config data from the radio
 */
void ViewController::requestConfig(void)
{
    if (client->isConnected() && requestConfigRequired) {
        client->send(meshtastic_ToRadio{.which_payload_variant = meshtastic_ToRadio_want_config_id_tag, .want_config_id = 1});
        requestConfigRequired = false;
    }
}

/**
 * request connection status of WLAN/BT/MQTT
 */
void ViewController::requestDeviceConnectionStatus(void)
{
    return; // FIXME this encoding leads to crash
    meshtastic_AdminMessage msg{.which_payload_variant = meshtastic_AdminMessage_get_device_connection_status_request_tag,
                                .get_device_connection_status_request = true};
    uint8_t encoded[meshtastic_Constants_DATA_PAYLOAD_LEN];
    size_t len = pb_encode_to_bytes(encoded, sizeof(encoded), &meshtastic_AdminMessage_msg, &msg);
    this->send(view->getMyNodeNum(), 0, meshtastic_PortNum_ADMIN_APP, encoded, len);
}

bool ViewController::handleFromRadio(const meshtastic_FromRadio &from)
{
    ILOG_DEBUG("handleFromRadio variant %u\n", from.which_payload_variant);
    switch (from.which_payload_variant) {
    case meshtastic_FromRadio_my_info_tag: {
        const meshtastic_MyNodeInfo &info = from.my_info;
        view->setMyInfo(info.my_node_num);
        break;
    }
    case meshtastic_FromRadio_packet_tag: {
        const meshtastic_MeshPacket &p = from.packet;
        if (p.which_payload_variant == meshtastic_MeshPacket_decoded_tag) {
            packetReceived(p);
        } else {
            // FIXME: needs implementation when not using PacketClient interface
            ILOG_ERROR("dropping encoded meshpacket id=%u from radio!\n", from.id);
        }
        break;
    }
    case meshtastic_FromRadio_node_info_tag: {
        const meshtastic_NodeInfo &node = from.node_info;
        if (node.has_user) {
            view->addOrUpdateNode(node.num, node.channel, node.user.short_name, node.user.long_name, node.last_heard,
                                  (MeshtasticView::eRole)node.user.role);
        } else {
            view->addOrUpdateNode(node.num, node.channel, node.last_heard, (MeshtasticView::eRole)node.user.role);
        }
        if (node.has_position) {
            view->updatePosition(node.num, node.position.latitude_i, node.position.longitude_i, node.position.altitude, 0,
                                 node.position.precision_bits);
        }
        if (node.has_device_metrics) {
            view->updateMetrics(node.num, node.device_metrics.battery_level, node.device_metrics.voltage,
                                node.device_metrics.channel_utilization, node.device_metrics.air_util_tx);
        }
        break;
    }
    case meshtastic_FromRadio_config_tag: {
        const meshtastic_Config &config = from.config;
        switch (config.which_payload_variant) {
        case meshtastic_Config_device_tag: {
            const meshtastic_Config_DeviceConfig &cfg = config.payload_variant.device;
            view->updateDeviceConfig(cfg);
            break;
        }
        case meshtastic_Config_position_tag: {
            const meshtastic_Config_PositionConfig &cfg = config.payload_variant.position;
            view->updatePositionConfig(cfg);
            break;
        }
        case meshtastic_Config_power_tag: {
            const meshtastic_Config_PowerConfig &cfg = config.payload_variant.power;
            view->updatePowerConfig(cfg);
            break;
        }
        case meshtastic_Config_network_tag: {
            const meshtastic_Config_NetworkConfig &cfg = config.payload_variant.network;
            view->updateNetworkConfig(cfg);
            break;
        }
        case meshtastic_Config_display_tag: {
            const meshtastic_Config_DisplayConfig &cfg = config.payload_variant.display;
            view->updateDisplayConfig(cfg);
            break;
        }
        case meshtastic_Config_lora_tag: {
            const meshtastic_Config_LoRaConfig &cfg = config.payload_variant.lora;
            view->updateLoRaConfig(cfg);
            break;
        }
        case meshtastic_Config_bluetooth_tag: {
            const meshtastic_Config_BluetoothConfig &cfg = config.payload_variant.bluetooth;
            view->updateBluetoothConfig(cfg);
            break;
        }
        default:
            ILOG_ERROR("unsupported device config variant: %u\n", config.which_payload_variant);
            return false;
        }
        break;
    }
    case meshtastic_FromRadio_moduleConfig_tag: {
        const meshtastic_ModuleConfig &module = from.moduleConfig;
        switch (module.which_payload_variant) {
        case meshtastic_ModuleConfig_mqtt_tag: {
            const meshtastic_ModuleConfig_MQTTConfig &cfg = module.payload_variant.mqtt;
            view->updateMQTTModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_serial_tag: {
            const meshtastic_ModuleConfig_SerialConfig &cfg = module.payload_variant.serial;
            view->updateSerialModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_external_notification_tag: {
            const meshtastic_ModuleConfig_ExternalNotificationConfig &cfg = module.payload_variant.external_notification;
            view->updateExtNotificationModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_store_forward_tag: {
            const meshtastic_ModuleConfig_StoreForwardConfig &cfg = module.payload_variant.store_forward;
            view->updateStoreForwardModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_range_test_tag: {
            const meshtastic_ModuleConfig_RangeTestConfig &cfg = module.payload_variant.range_test;
            view->updateRangeTestModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_telemetry_tag: {
            const meshtastic_ModuleConfig_TelemetryConfig &cfg = module.payload_variant.telemetry;
            view->updateTelemetryModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_canned_message_tag: {
            const meshtastic_ModuleConfig_CannedMessageConfig &cfg = module.payload_variant.canned_message;
            view->updateCannedMessageModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_audio_tag: {
            const meshtastic_ModuleConfig_AudioConfig &cfg = module.payload_variant.audio;
            view->updateAudioModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_remote_hardware_tag: {
            const meshtastic_ModuleConfig_RemoteHardwareConfig &cfg = module.payload_variant.remote_hardware;
            view->updateRemoteHardwareModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_neighbor_info_tag: {
            const meshtastic_ModuleConfig_NeighborInfoConfig &cfg = module.payload_variant.neighbor_info;
            view->updateNeighborInfoModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_ambient_lighting_tag: {
            const meshtastic_ModuleConfig_AmbientLightingConfig &cfg = module.payload_variant.ambient_lighting;
            view->updateAmbientLightingModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_detection_sensor_tag: {
            const meshtastic_ModuleConfig_DetectionSensorConfig &cfg = module.payload_variant.detection_sensor;
            view->updateDetectionSensorModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_paxcounter_tag: {
            const meshtastic_ModuleConfig_PaxcounterConfig &cfg = module.payload_variant.paxcounter;
            view->updatePaxCounterModule(cfg);
            break;
        }
        default:
            ILOG_ERROR("unsupported module config variant: %u\n", module.which_payload_variant);
            return false;
        }
        break;
    }
    case meshtastic_FromRadio_channel_tag: {
        const meshtastic_Channel &ch = from.channel;
        if (ch.has_settings) {
            view->updateChannelConfig(ch.index, ch.settings.name, ch.settings.psk.bytes, ch.settings.psk.size, ch.role);
        }
        break;
    }
    case meshtastic_FromRadio_metadata_tag: {
        const meshtastic_DeviceMetadata &meta = from.metadata;
        view->setDeviceMetaData(meta.hw_model, meta.firmware_version, meta.hasBluetooth, meta.hasWifi, meta.hasEthernet,
                                meta.canShutdown);
        break;
    }
    case meshtastic_FromRadio_config_complete_id_tag: {
        view->configCompleted();
        view->notifyResync(false);
        break;
    }
    case meshtastic_FromRadio_queueStatus_tag: {
        const meshtastic_QueueStatus &q = from.queueStatus;
        if (q.free == 0) {
            ILOG_CRIT("meshqueue full!?\n");
        }
        break;
    }
    case meshtastic_FromRadio_rebooted_tag: {
        view->notifyResync(true);
        setConfigRequested(true);
        break;
    }
    default: {
        ILOG_ERROR("unhandled fromRadio packet variant: %u\n", from.which_payload_variant);
        return false;
    }
    }
    return true;
}

bool ViewController::packetReceived(const meshtastic_MeshPacket &p)
{
    ILOG_DEBUG("received packet from 0x%08x(%u), portnum=%u\n", p.from, p.from, p.decoded.portnum);
    view->packetReceived(p);

    // only for direct neighbors print rssi/snr
    if (p.hop_limit == p.hop_start) {
        view->updateSignalStrength(p.from, p.rx_rssi, p.rx_snr);
    }
    view->updateLastHeard(p.from);

    switch (p.decoded.portnum) {
    case meshtastic_PortNum_TEXT_MESSAGE_APP: {
        ILOG_INFO("received text message from 0x%08x(%u): '%s'\n", p.from, p.from, (const char *)p.decoded.payload.bytes);
        view->newMessage(p.from, p.to, p.channel, (const char *)p.decoded.payload.bytes);
        break;
    }
    case meshtastic_PortNum_POSITION_APP: {
        meshtastic_Position position;
        if (pb_decode_from_bytes(p.decoded.payload.bytes, p.decoded.payload.size, &meshtastic_Position_msg, &position)) {
            view->updatePosition(p.from, position.latitude_i, position.longitude_i, position.altitude, position.sats_in_view,
                                 position.precision_bits);
            // TODO: updateTime(position.time);
        } else {
            ILOG_ERROR("Error decoding protobuf meshtastic_Position!\n");
            return false;
        }
        break;
    }
    case meshtastic_PortNum_NODEINFO_APP: {
        meshtastic_User user;
        if (pb_decode_from_bytes(p.decoded.payload.bytes, p.decoded.payload.size, &meshtastic_User_msg, &user)) {
            view->updateNode(p.from, -1, user.short_name, user.long_name, 0, (MeshtasticView::eRole)user.role);
        } else {
            ILOG_ERROR("Error decoding protobuf meshtastic_User (nodeinfo)!\n");
            return false;
        }
        break;
    }
    case meshtastic_PortNum_TELEMETRY_APP: {
        meshtastic_Telemetry telemetry;
        if (pb_decode_from_bytes(p.decoded.payload.bytes, p.decoded.payload.size, &meshtastic_Telemetry_msg, &telemetry)) {
            switch (telemetry.which_variant) {
            case meshtastic_Telemetry_device_metrics_tag: {
                view->updateMetrics(
                    p.from, telemetry.variant.device_metrics.battery_level, telemetry.variant.device_metrics.voltage,
                    telemetry.variant.device_metrics.channel_utilization, telemetry.variant.device_metrics.air_util_tx);
                break;
            }
            case meshtastic_Telemetry_environment_metrics_tag: {
                ILOG_WARN("meshtastic_Telemetry_environment_metrics_tag not implemented\n");
                return false;
                break;
            }
            case meshtastic_Telemetry_power_metrics_tag: {
                ILOG_WARN("meshtastic_Telemetry_power_metrics_tag not implemented\n");
                return false;
                break;
            }
            default:
                ILOG_ERROR("unhandled telemetry variant: %u\n", telemetry.which_variant);
                return false;
                break;
            }
        } else {
            ILOG_ERROR("Error decoding protobuf meshtastic_Telemetry!\n");
            return false;
        }
        break;
    }
    case meshtastic_PortNum_ROUTING_APP:
        ILOG_WARN("meshtastic_PortNum_ROUTING_APP not implemented\n");
        break;
    case meshtastic_PortNum_ADMIN_APP: {
        meshtastic_AdminMessage admin;
        if (pb_decode_from_bytes(p.decoded.payload.bytes, p.decoded.payload.size, &meshtastic_AdminMessage_msg, &admin)) {
            switch (admin.which_payload_variant) {
            case meshtastic_AdminMessage_get_device_connection_status_response_tag: {
                meshtastic_DeviceConnectionStatus &status = admin.get_device_connection_status_response;
                view->updateConnectionStatus(status);
                break;
            }
            case meshtastic_AdminMessage_set_config_tag: {
                ILOG_WARN("meshtastic_AdminMessage_set_config_tag not implemented\n");
                return false;
                break;
            }
            default:
                ILOG_ERROR("unhandled AdminMessage variant: %u\n", admin.which_payload_variant);
                return false;
                break;
            }
        } else {
            ILOG_ERROR("Error decoding protobuf meshtastic_AdminMessage!\n");
            return false;
        }
        break;
    }
    case meshtastic_PortNum_SIMULATOR_APP:
        break;
    default:
        ILOG_ERROR("unhandled meshpacket portnum: %u\n", p.decoded.portnum);
        return false;
    }
    return true;
}

ViewController::~ViewController() {}
