#pragma once

#include "meshtastic/mesh.pb.h"
#include "meshtastic/telemetry.pb.h"

#include <cstddef>
#include <cstdint>
#include <unordered_map>

#ifndef MAX_NUM_NODES_VIEW
#define MAX_NUM_NODES_VIEW 250
#endif

using NodeId = uint32_t;

struct NodePosition {
    bool known = false;
    int32_t latitude = 0;
    int32_t longitude = 0;
    int32_t altitude = 0;
    uint32_t satellites = 0;
    uint32_t precision = 0;

    bool hasCoordinates() const { return known && (latitude != 0 || longitude != 0); }
};

enum class NodeSignalDisplayKind : uint8_t { None, Rssi, Hops };

struct NodeUserSummary {
    char id[16]{};
    char long_name[40]{};
    char short_name[5]{};
    uint8_t macaddr[6]{};
    meshtastic_HardwareModel hw_model = meshtastic_HardwareModel_UNSET;
    meshtastic_Config_DeviceConfig_Role role = meshtastic_Config_DeviceConfig_Role_CLIENT;
    bool is_licensed = false;
    uint8_t publicKeySize = 0;
    uint32_t publicKeyHash = 0;
};

struct NodeDeviceMetrics {
    uint32_t battery_level = 0;
    float voltage = 0.0f;
    float channel_utilization = 0.0f;
    float air_util_tx = 0.0f;
};

struct NodeEnvironmentMetrics {
    float temperature = 0.0f;
    float relative_humidity = 0.0f;
    float barometric_pressure = 0.0f;
    float voltage = 0.0f;
    float current = 0.0f;
    uint16_t iaq = 0;
};

struct NodeAirQualityMetrics {
    uint16_t pm25_standard = 0;
};

struct NodeRecord {
    uint64_t recencyOrder = 0;
    NodeUserSummary user{};
    NodePosition position{};
    NodeDeviceMetrics deviceMetrics{};
    NodeEnvironmentMetrics environmentMetrics{};
    NodeAirQualityMetrics airQualityMetrics{};
    NodeId id = 0;
    uint32_t lastHeard = 0;
    int32_t rssi = 0;
    float snr = 0;
    uint8_t channel = 0;
    int8_t hopsAway = -1;
    NodeSignalDisplayKind signalDisplay = NodeSignalDisplayKind::None;
    bool hasUser = false;
    bool hasKey = false;
    bool hasBadKey = false;
    bool unmessagable = false;
    bool viaMqtt = false;
    bool recencyPromoted = false;
    bool hasDeviceMetrics = false;
    bool hasEnvironmentMetrics = false;
    bool hasAirQualityMetrics = false;
    bool hasActiveChat = false;
};

static_assert(sizeof(NodeRecord) <= 192, "NodeRecord must remain compact for non-PSRAM targets");

enum class NodeMutationKind { Inserted, Updated, Removed, Unchanged };

enum NodeChangedField : uint32_t {
    NodeFieldNone = 0,
    NodeFieldUser = 1U << 0,
    NodeFieldChannel = 1U << 1,
    NodeFieldFlags = 1U << 2,
    NodeFieldLastHeard = 1U << 3,
    NodeFieldPosition = 1U << 4,
    NodeFieldDeviceMetrics = 1U << 5,
    NodeFieldEnvironmentMetrics = 1U << 6,
    NodeFieldAirQualityMetrics = 1U << 7,
    NodeFieldSignal = 1U << 8,
    NodeFieldHops = 1U << 9,
    NodeFieldActiveChat = 1U << 10,
    NodeFieldPowerMetrics = 1U << 11,
};

struct NodeMutation {
    NodeMutationKind kind = NodeMutationKind::Unchanged;
    NodeId id = 0;
    uint32_t changedFields = NodeFieldNone;
};

class NodeStore
{
  public:
    using Records = std::unordered_map<NodeId, NodeRecord>;

    NodeStore();

    const NodeRecord *find(NodeId id) const;
    const Records &records() const { return nodes; }
    size_t size() const { return nodes.size(); }

    NodeMutation upsertUser(NodeId id, uint8_t channel, uint32_t lastHeard, const meshtastic_User &user, bool viaMqtt);
    NodeMutation upsertUnknown(NodeId id, uint8_t channel, uint32_t lastHeard, uint8_t role, bool hasKey, bool viaMqtt);
    NodeMutation updatePosition(NodeId id, const NodePosition &position);
    NodeMutation updateDeviceMetrics(NodeId id, const meshtastic_DeviceMetrics &metrics);
    NodeMutation updateEnvironmentMetrics(NodeId id, const meshtastic_EnvironmentMetrics &metrics);
    NodeMutation updateAirQualityMetrics(NodeId id, const meshtastic_AirQualityMetrics &metrics);
    NodeMutation updatePowerMetrics(NodeId id, const meshtastic_PowerMetrics &metrics);
    NodeMutation updateSignal(NodeId id, int32_t rssi, float snr);
    NodeMutation updateHops(NodeId id, int8_t hopsAway);
    NodeMutation updateLastHeard(NodeId id, uint32_t now);
    NodeMutation markBadKey(NodeId id);
    NodeMutation setActiveChat(NodeId id, bool active);
    NodeMutation remove(NodeId id);

    NodeId selectPurgeCandidate(NodeId incoming, NodeId ownNode, uint32_t now) const;

  private:
    void touchRecency(NodeRecord &record);

    Records nodes;
    uint64_t nextRecencyOrder = 1;
};
