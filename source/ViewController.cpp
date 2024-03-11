#include "ViewController.h"
#include "Arduino.h"
#include "MeshtasticView.h"
#include "assert.h"
#include <algorithm>

/**
 * @brief mediate between GUI view and client interface
 *
 */
ViewController::ViewController() : view(nullptr), client(nullptr) {}

void ViewController::init(MeshtasticView *gui, IClientBase *_client)
{
    view = gui;
    client = _client;
    client->init();
    requestConfig();
}

void ViewController::runOnce(void)
{
    if (client->isConnected()) {
        meshtastic_FromRadio from = client->receive();
        if (from.id) {
            handleFromRadio(from);
        }
    }
}

void ViewController::requestConfig(void)
{
    client->send(meshtastic_ToRadio{.which_payload_variant = meshtastic_ToRadio_want_config_id_tag, .want_config_id = 1});
}

void ViewController::handleFromRadio(const meshtastic_FromRadio &from)
{
    switch (from.which_payload_variant) {
    case meshtastic_FromRadio_my_info_tag: {
        const meshtastic_MyNodeInfo &info = from.my_info;
        view->setMyInfo(info.my_node_num);
        break;
    }
    case meshtastic_FromRadio_packet_tag: {
        const meshtastic_MeshPacket &p = from.packet;
        if (p.which_payload_variant == meshtastic_MeshPacket_decoded_tag) {
            view->packetReceived(p.from, p);
        } else {
            Serial.println("ERROR  | ??:??:?? encoded packet from radio!");
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
            view->updatePosition(node.num, node.position.latitude_i, node.position.longitude_i, node.position.altitude,
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
            Serial.println("ERROR | ??:??:?? unsupported device Config");
            break;
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
            Serial.println("ERROR | ??:??:?? unsupported module Config");
            break;
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
        break;
    }
    case meshtastic_FromRadio_queueStatus_tag: {
        const meshtastic_QueueStatus &q = from.queueStatus;
        if (q.free == 0) {
            Serial.println("ERROR | ??:??:?? meshqueue full");
        }
        break;
    }
    default: {
        Serial.print("WARN  | ??:??:?? unhandled packet from radio: ");
        Serial.println(from.which_payload_variant);
        break;
    }
    }
}

void ViewController::processEvent(void) {}

#define MSG(i) pb_byte_t(textmsg[i])

void ViewController::sendText(uint32_t to, uint8_t ch, const char *textmsg)
{
    assert(strlen(textmsg) <= (size_t)meshtastic_Constants_DATA_PAYLOAD_LEN);
    // send requires movable lvalue, i.e. a temporary object
    client->send(meshtastic_ToRadio{
        .which_payload_variant = meshtastic_ToRadio_packet_tag,
        .packet{
            .from = 0,
            .to = to,
            .channel = ch,
            .which_payload_variant = meshtastic_MeshPacket_decoded_tag,
            .decoded{
                .portnum = meshtastic_PortNum_TEXT_MESSAGE_APP,
                .payload{
                    .size = (uint16_t)strlen(textmsg),
                    .bytes = {MSG(0),   MSG(1),   MSG(2),   MSG(3),   MSG(4),   MSG(5),   MSG(6),   MSG(7),   MSG(8),   MSG(9),
                              MSG(10),  MSG(11),  MSG(12),  MSG(13),  MSG(14),  MSG(15),  MSG(16),  MSG(17),  MSG(18),  MSG(19),
                              MSG(20),  MSG(21),  MSG(22),  MSG(23),  MSG(24),  MSG(25),  MSG(26),  MSG(27),  MSG(28),  MSG(29),
                              MSG(30),  MSG(31),  MSG(32),  MSG(33),  MSG(34),  MSG(35),  MSG(36),  MSG(37),  MSG(38),  MSG(39),
                              MSG(40),  MSG(41),  MSG(42),  MSG(43),  MSG(44),  MSG(45),  MSG(46),  MSG(47),  MSG(48),  MSG(49),
                              MSG(50),  MSG(51),  MSG(52),  MSG(53),  MSG(54),  MSG(55),  MSG(56),  MSG(57),  MSG(58),  MSG(59),
                              MSG(60),  MSG(61),  MSG(62),  MSG(63),  MSG(64),  MSG(65),  MSG(66),  MSG(67),  MSG(68),  MSG(69),
                              MSG(70),  MSG(71),  MSG(72),  MSG(73),  MSG(74),  MSG(75),  MSG(76),  MSG(77),  MSG(78),  MSG(79),
                              MSG(80),  MSG(81),  MSG(82),  MSG(83),  MSG(84),  MSG(85),  MSG(86),  MSG(87),  MSG(88),  MSG(89),
                              MSG(90),  MSG(91),  MSG(92),  MSG(93),  MSG(94),  MSG(95),  MSG(96),  MSG(97),  MSG(98),  MSG(99),
                              MSG(100), MSG(101), MSG(102), MSG(103), MSG(104), MSG(105), MSG(106), MSG(107), MSG(108), MSG(109),
                              MSG(110), MSG(111), MSG(112), MSG(113), MSG(114), MSG(115), MSG(116), MSG(117), MSG(118), MSG(119),
                              MSG(120), MSG(121), MSG(122), MSG(123), MSG(124), MSG(125), MSG(126), MSG(127), MSG(128), MSG(129),
                              MSG(130), MSG(131), MSG(132), MSG(133), MSG(134), MSG(135), MSG(136), MSG(137), MSG(138), MSG(139),
                              MSG(140), MSG(141), MSG(142), MSG(143), MSG(144), MSG(145), MSG(146), MSG(147), MSG(148), MSG(149),
                              MSG(150), MSG(151), MSG(152), MSG(153), MSG(154), MSG(155), MSG(156), MSG(157), MSG(158), MSG(159),
                              MSG(160), MSG(161), MSG(162), MSG(163), MSG(164), MSG(165), MSG(166), MSG(167), MSG(168), MSG(169),
                              MSG(170), MSG(171), MSG(172), MSG(173), MSG(174), MSG(175), MSG(176), MSG(177), MSG(178), MSG(179),
                              MSG(180), MSG(181), MSG(182), MSG(183), MSG(184), MSG(185), MSG(186), MSG(187), MSG(188), MSG(189),
                              MSG(190), MSG(191), MSG(192), MSG(193), MSG(194), MSG(195), MSG(196), MSG(197), MSG(198), MSG(199),
                              MSG(200), MSG(201), MSG(202), MSG(203), MSG(204), MSG(205), MSG(206), MSG(207), MSG(208), MSG(209),
                              MSG(210), MSG(211), MSG(212), MSG(213), MSG(214), MSG(215), MSG(216), MSG(217), MSG(218), MSG(219),
                              MSG(220), MSG(221), MSG(222), MSG(223), MSG(224), MSG(225), MSG(226), MSG(227), MSG(228), MSG(229),
                              MSG(230), MSG(231), MSG(232), MSG(233), MSG(234), MSG(235), MSG(236)}},
            },
            .want_ack = to != UINT32_MAX}});
}

ViewController::~ViewController() {}
