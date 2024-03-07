#include "ViewController.h"
#include "MeshtasticView.h"
#include "Arduino.h"

/**
 * @brief mediate between GUI view and client interface
 * 
 */
ViewController::ViewController() : view(nullptr), client(nullptr) {

}

void ViewController::init(MeshtasticView* gui, IClientBase* _client) {
    view = gui;
    client = _client;
    client->init();
    requestConfig();
}

void ViewController::runOnce(void) {
    if (client->isConnected()) {
        meshtastic_FromRadio from = client->receive();
        if (from.id) {
            handleFromRadio(from);
        }
    }
}

void ViewController::requestConfig(void) {
    client->send(meshtastic_ToRadio {
        .which_payload_variant = meshtastic_ToRadio_want_config_id_tag,
        .want_config_id = 1
    });
}

void ViewController::handleFromRadio(const meshtastic_FromRadio& from) {
    switch(from.which_payload_variant) {
    case meshtastic_FromRadio_my_info_tag: {
        const meshtastic_MyNodeInfo& info = from.my_info;
        view->setMyInfo(info.my_node_num);
        break;
    }
    case meshtastic_FromRadio_packet_tag: {
        const meshtastic_MeshPacket& p = from.packet;
        view->packetReceived(p.from, p);
        break;
    }
    case meshtastic_FromRadio_node_info_tag: {
        const meshtastic_NodeInfo& node = from.node_info;
        if (node.has_user) {
            view->addOrUpdateNode(node.num, node.channel, node.user.short_name, node.user.long_name, node.last_heard, (MeshtasticView::eRole)node.user.role);
        }
        else { // show node with default user name (will be replaced on next nodeinfo update)
            char userShort[5], userLong[32];
            sprintf(userShort, "%04x", node.num & 0xffff);
            strcpy(userLong, "Meshtastic "); strcat(userLong, userShort);
            view->addOrUpdateNode(node.num, node.channel, userShort, userLong, node.last_heard, (MeshtasticView::eRole)node.user.role);
        }
        if (node.has_position) {
            view->updatePosition(node.num, node.position.latitude_i, node.position.longitude_i, node.position.altitude, node.position.precision_bits);
        }
        if (node.has_device_metrics) {
            view->updateMetrics(node.num, node.device_metrics.battery_level, node.device_metrics.voltage, node.device_metrics.channel_utilization, node.device_metrics.air_util_tx);
        }
        break;
    }
    case meshtastic_FromRadio_config_tag: {
        const meshtastic_Config& config = from.config;
        switch (config.which_payload_variant) {
        case meshtastic_Config_device_tag: {
            const meshtastic_Config_DeviceConfig& cfg = config.payload_variant.device;
            view->updateDeviceConfig(cfg);
            break;
        }
        case meshtastic_Config_position_tag: {
            const meshtastic_Config_PositionConfig& cfg = config.payload_variant.position;
            view->updatePositionConfig(cfg);
            break;
        }
        case meshtastic_Config_power_tag: {
            const meshtastic_Config_PowerConfig& cfg = config.payload_variant.power;
            view->updatePowerConfig(cfg);
            break;
        }
        case meshtastic_Config_network_tag: {
            const meshtastic_Config_NetworkConfig& cfg = config.payload_variant.network;
            view->updateNetworkConfig(cfg);
            break;
        }
        case meshtastic_Config_display_tag: {
            const meshtastic_Config_DisplayConfig& cfg = config.payload_variant.display;
            view->updateDisplayConfig(cfg);
            break;
        }
        case meshtastic_Config_lora_tag: {
            const meshtastic_Config_LoRaConfig& cfg = config.payload_variant.lora;
            view->updateLoRaConfig(cfg);
            break;
        }
        case meshtastic_Config_bluetooth_tag: {
            const meshtastic_Config_BluetoothConfig& cfg = config.payload_variant.bluetooth;
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
        const meshtastic_ModuleConfig& module = from.moduleConfig;
        switch(module.which_payload_variant) {
        case meshtastic_ModuleConfig_mqtt_tag: {
            const meshtastic_ModuleConfig_MQTTConfig& cfg = module.payload_variant.mqtt;
            view->updateMQTTModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_serial_tag: {
            const meshtastic_ModuleConfig_SerialConfig& cfg = module.payload_variant.serial;
            view->updateSerialModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_external_notification_tag: {
            const meshtastic_ModuleConfig_ExternalNotificationConfig& cfg = module.payload_variant.external_notification;
            view->updateExtNotificationModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_store_forward_tag: {
            const meshtastic_ModuleConfig_StoreForwardConfig& cfg = module.payload_variant.store_forward;
            view->updateStoreForwardModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_range_test_tag: {
            const meshtastic_ModuleConfig_RangeTestConfig& cfg = module.payload_variant.range_test;
            view->updateRangeTestModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_telemetry_tag: {
            const meshtastic_ModuleConfig_TelemetryConfig& cfg = module.payload_variant.telemetry;
            view->updateTelemetryModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_canned_message_tag: {
            const meshtastic_ModuleConfig_CannedMessageConfig& cfg = module.payload_variant.canned_message;
            view->updateCannedMessageModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_audio_tag: {
            const meshtastic_ModuleConfig_AudioConfig& cfg = module.payload_variant.audio;
            view->updateAudioModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_remote_hardware_tag: {
            const meshtastic_ModuleConfig_RemoteHardwareConfig& cfg = module.payload_variant.remote_hardware;
            view->updateRemoteHardwareModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_neighbor_info_tag: {
            const meshtastic_ModuleConfig_NeighborInfoConfig& cfg = module.payload_variant.neighbor_info;
            view->updateNeighborInfoModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_ambient_lighting_tag: {
            const meshtastic_ModuleConfig_AmbientLightingConfig& cfg = module.payload_variant.ambient_lighting;
            view->updateAmbientLightingModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_detection_sensor_tag: {
            const meshtastic_ModuleConfig_DetectionSensorConfig& cfg = module.payload_variant.detection_sensor;
            view->updateDetectionSensorModule(cfg);
            break;
        }
        case meshtastic_ModuleConfig_paxcounter_tag: {
            const meshtastic_ModuleConfig_PaxcounterConfig& cfg = module.payload_variant.paxcounter;
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
        const meshtastic_Channel& ch = from.channel;
        if (ch.has_settings) {
            view->updateChannelConfig(ch.index, ch.settings.name, ch.settings.psk.bytes, ch.settings.psk.size, ch.role);
        }
        break;
    }
    case meshtastic_FromRadio_metadata_tag: {
        const meshtastic_DeviceMetadata& meta = from.metadata;
        view->setDeviceMetaData(meta.hw_model, meta.firmware_version, meta.hasBluetooth, meta.hasWifi, meta.hasEthernet, meta.canShutdown);
        break;
    }
    case meshtastic_FromRadio_config_complete_id_tag: {
        view->configCompleted();
        break;
    }
    case meshtastic_FromRadio_queueStatus_tag: {
        const meshtastic_QueueStatus& q = from.queueStatus;
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

void ViewController::processEvent(void) {

}

ViewController::~ViewController() {

}

