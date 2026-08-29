#include "graphics/common/NodeStore.h"

#include "mesh-pb-constants.h"

#include <cstdio>
#include <cstring>

namespace
{
constexpr uint32_t purgeFreshnessSeconds = 120;

uint32_t effectiveLastHeard(const NodeRecord &record, uint32_t now)
{
    return record.lastHeard > now ? now : record.lastHeard;
}

uint32_t publicKeyHash(const meshtastic_User &user)
{
    uint32_t hash = 2166136261U;
    for (pb_size_t i = 0; i < user.public_key.size; ++i) {
        hash ^= user.public_key.bytes[i];
        hash *= 16777619U;
    }
    return hash;
}

NodeUserSummary summarizeUser(const meshtastic_User &user)
{
    NodeUserSummary summary{};
    std::snprintf(summary.id, sizeof(summary.id), "%s", user.id);
    std::snprintf(summary.long_name, sizeof(summary.long_name), "%s", user.long_name);
    std::snprintf(summary.short_name, sizeof(summary.short_name), "%s", user.short_name);
    std::memcpy(summary.macaddr, user.macaddr, sizeof(summary.macaddr));
    summary.hw_model = user.hw_model;
    summary.role = user.role;
    summary.is_licensed = user.is_licensed;
    summary.publicKeySize = static_cast<uint8_t>(user.public_key.size);
    summary.publicKeyHash = publicKeyHash(user);
    return summary;
}

bool sameUser(const NodeUserSummary &left, const NodeUserSummary &right)
{
    return std::strcmp(left.id, right.id) == 0 && std::strcmp(left.long_name, right.long_name) == 0 &&
           std::strcmp(left.short_name, right.short_name) == 0 &&
           std::memcmp(left.macaddr, right.macaddr, sizeof(left.macaddr)) == 0 && left.hw_model == right.hw_model &&
           left.is_licensed == right.is_licensed && left.role == right.role && left.publicKeySize == right.publicKeySize &&
           left.publicKeyHash == right.publicKeyHash;
}

bool samePosition(const NodePosition &left, const NodePosition &right)
{
    return left.known == right.known && left.latitude == right.latitude && left.longitude == right.longitude &&
           left.altitude == right.altitude && left.satellites == right.satellites && left.precision == right.precision;
}

bool samePublicKey(const NodeUserSummary &left, const NodeUserSummary &right)
{
    return left.publicKeySize == right.publicKeySize && left.publicKeyHash == right.publicKeyHash;
}

NodeDeviceMetrics summarizeDeviceMetrics(const meshtastic_DeviceMetrics &metrics)
{
    return {metrics.battery_level, metrics.voltage, metrics.channel_utilization, metrics.air_util_tx};
}

NodeEnvironmentMetrics summarizeEnvironmentMetrics(const meshtastic_EnvironmentMetrics &metrics)
{
    return {metrics.temperature, metrics.relative_humidity, metrics.barometric_pressure,
            metrics.voltage,     metrics.current,           metrics.iaq};
}

NodeAirQualityMetrics summarizeAirQualityMetrics(const meshtastic_AirQualityMetrics &metrics)
{
    return {static_cast<uint16_t>(metrics.pm25_standard > UINT16_MAX ? UINT16_MAX : metrics.pm25_standard)};
}

bool sameDeviceMetrics(const NodeDeviceMetrics &left, const NodeDeviceMetrics &right)
{
    return left.battery_level == right.battery_level && left.voltage == right.voltage &&
           left.channel_utilization == right.channel_utilization && left.air_util_tx == right.air_util_tx;
}

bool sameEnvironmentMetrics(const NodeEnvironmentMetrics &left, const NodeEnvironmentMetrics &right)
{
    return left.temperature == right.temperature && left.relative_humidity == right.relative_humidity &&
           left.barometric_pressure == right.barometric_pressure && left.voltage == right.voltage &&
           left.current == right.current && left.iaq == right.iaq;
}

bool sameAirQualityMetrics(const NodeAirQualityMetrics &left, const NodeAirQualityMetrics &right)
{
    return left.pm25_standard == right.pm25_standard;
}

NodeMutation unchanged(NodeId id)
{
    return {NodeMutationKind::Unchanged, id, NodeFieldNone};
}

NodeMutation updated(NodeId id, uint32_t fields)
{
    return {fields == NodeFieldNone ? NodeMutationKind::Unchanged : NodeMutationKind::Updated, id, fields};
}
} // namespace

NodeStore::NodeStore()
{
    nodes.reserve(MAX_NUM_NODES_VIEW);
}

const NodeRecord *NodeStore::find(NodeId id) const
{
    const auto it = nodes.find(id);
    return it == nodes.end() ? nullptr : &it->second;
}

void NodeStore::touchRecency(NodeRecord &record)
{
    record.recencyPromoted = true;
    record.recencyOrder = nextRecencyOrder++;
}

NodeMutation NodeStore::upsertUser(NodeId id, uint8_t channel, uint32_t lastHeard, const meshtastic_User &user, bool viaMqtt)
{
    auto [it, inserted] = nodes.try_emplace(id);
    auto &record = it->second;
    uint32_t changed = NodeFieldNone;
    const NodeUserSummary userSummary = summarizeUser(user);
    const bool keyChanged = record.hasUser && !samePublicKey(record.user, userSummary);

    if (inserted) {
        record.id = id;
        record.recencyOrder = nextRecencyOrder++;
        changed = NodeFieldUser | NodeFieldFlags | NodeFieldLastHeard;
        if (channel < MAX_NUM_CHANNELS) {
            changed |= NodeFieldChannel;
        }
    }
    if (!record.hasUser || !sameUser(record.user, userSummary)) {
        record.hasUser = true;
        record.user = userSummary;
        changed |= NodeFieldUser;
    }
    if (channel < MAX_NUM_CHANNELS) {
        if (record.channel != channel) {
            record.channel = channel;
            changed |= NodeFieldChannel;
        }
    } else if (inserted) {
        // New record with invalid channel keeps default 0 without dirtying.
        record.channel = 0;
    }
    const bool hasKey = user.public_key.size != 0;
    const bool unmessagable = user.has_is_unmessagable && user.is_unmessagable;
    const bool keyPresenceChanged = record.hasKey != hasKey;
    if (keyPresenceChanged || record.unmessagable != unmessagable || record.viaMqtt != viaMqtt || keyChanged) {
        record.hasKey = hasKey;
        if (keyPresenceChanged || keyChanged) {
            record.hasBadKey = false;
        }
        record.unmessagable = unmessagable;
        record.viaMqtt = viaMqtt;
        changed |= NodeFieldFlags;
    }
    if (record.lastHeard != lastHeard) {
        record.lastHeard = lastHeard;
        changed |= NodeFieldLastHeard;
    }

    return inserted ? NodeMutation{NodeMutationKind::Inserted, id, changed} : updated(id, changed);
}

NodeMutation NodeStore::upsertUnknown(NodeId id, uint8_t channel, uint32_t lastHeard, uint8_t role, bool hasKey, bool viaMqtt)
{
    meshtastic_User fallback = meshtastic_User_init_default;
    std::snprintf(fallback.short_name, sizeof(fallback.short_name), "%04x", id & 0xffff);
    std::snprintf(fallback.long_name, sizeof(fallback.long_name), "Meshtastic %s", fallback.short_name);
    fallback.role = static_cast<meshtastic_Config_DeviceConfig_Role>(role);
    fallback.hw_model = meshtastic_HardwareModel_UNSET;
    const NodeUserSummary fallbackSummary = summarizeUser(fallback);

    auto [it, inserted] = nodes.try_emplace(id);
    auto &record = it->second;
    uint32_t changed = NodeFieldNone;

    if (inserted) {
        record.id = id;
        record.recencyOrder = nextRecencyOrder++;
        changed = NodeFieldUser | NodeFieldFlags | NodeFieldLastHeard;
        if (channel < MAX_NUM_CHANNELS) {
            changed |= NodeFieldChannel;
        }
    }
    if (record.hasUser || !sameUser(record.user, fallbackSummary)) {
        record.hasUser = false;
        record.user = fallbackSummary;
        changed |= NodeFieldUser;
    }
    if (inserted && channel < MAX_NUM_CHANNELS && record.channel != channel) {
        record.channel = channel;
        changed |= NodeFieldChannel;
    } else if (inserted && channel >= MAX_NUM_CHANNELS) {
        record.channel = 0;
    }
    if (record.hasKey != hasKey || record.unmessagable || record.viaMqtt != viaMqtt) {
        record.hasKey = hasKey;
        record.hasBadKey = false;
        record.unmessagable = false;
        record.viaMqtt = viaMqtt;
        changed |= NodeFieldFlags;
    }
    if (inserted && record.lastHeard != lastHeard) {
        record.lastHeard = lastHeard;
        changed |= NodeFieldLastHeard;
    }

    return inserted ? NodeMutation{NodeMutationKind::Inserted, id, changed} : updated(id, changed);
}

NodeMutation NodeStore::updatePosition(NodeId id, const NodePosition &position)
{
    auto it = nodes.find(id);
    if (it == nodes.end() || !position.hasCoordinates() || samePosition(it->second.position, position))
        return unchanged(id);
    it->second.position = position;
    return updated(id, NodeFieldPosition);
}

NodeMutation NodeStore::updateDeviceMetrics(NodeId id, const meshtastic_DeviceMetrics &metrics)
{
    auto it = nodes.find(id);
    if (it == nodes.end())
        return unchanged(id);
    auto retainedMetrics = summarizeDeviceMetrics(metrics);
    if (it->second.hasDeviceMetrics && retainedMetrics.battery_level == 0 && retainedMetrics.voltage == 0.0f) {
        retainedMetrics.battery_level = it->second.deviceMetrics.battery_level;
        retainedMetrics.voltage = it->second.deviceMetrics.voltage;
    }
    if (it->second.hasDeviceMetrics && sameDeviceMetrics(it->second.deviceMetrics, retainedMetrics))
        return unchanged(id);
    it->second.hasDeviceMetrics = true;
    it->second.deviceMetrics = retainedMetrics;
    return updated(id, NodeFieldDeviceMetrics);
}

NodeMutation NodeStore::updateEnvironmentMetrics(NodeId id, const meshtastic_EnvironmentMetrics &metrics)
{
    auto it = nodes.find(id);
    if (it == nodes.end())
        return unchanged(id);
    auto retainedMetrics = summarizeEnvironmentMetrics(metrics);
    if (!metrics.has_voltage && it->second.environmentMetrics.voltage != 0.0f) {
        retainedMetrics.voltage = it->second.environmentMetrics.voltage;
    }
    if (!metrics.has_current && it->second.environmentMetrics.current != 0.0f) {
        retainedMetrics.current = it->second.environmentMetrics.current;
    }
    if (it->second.hasEnvironmentMetrics && (retainedMetrics.iaq == 0 || retainedMetrics.iaq >= 1000) &&
        it->second.environmentMetrics.iaq > 0 && it->second.environmentMetrics.iaq < 1000) {
        retainedMetrics.iaq = it->second.environmentMetrics.iaq;
        retainedMetrics.voltage = it->second.environmentMetrics.voltage;
        retainedMetrics.current = it->second.environmentMetrics.current;
    }
    if (it->second.hasEnvironmentMetrics && sameEnvironmentMetrics(it->second.environmentMetrics, retainedMetrics))
        return unchanged(id);
    it->second.hasEnvironmentMetrics = true;
    it->second.environmentMetrics = retainedMetrics;
    return updated(id, NodeFieldEnvironmentMetrics);
}

NodeMutation NodeStore::updateAirQualityMetrics(NodeId id, const meshtastic_AirQualityMetrics &metrics)
{
    auto it = nodes.find(id);
    if (it == nodes.end() || !metrics.has_pm25_standard)
        return unchanged(id);
    const auto retainedMetrics = summarizeAirQualityMetrics(metrics);
    if (it->second.hasAirQualityMetrics && sameAirQualityMetrics(it->second.airQualityMetrics, retainedMetrics))
        return unchanged(id);
    it->second.hasAirQualityMetrics = true;
    it->second.airQualityMetrics = retainedMetrics;
    return updated(id, NodeFieldAirQualityMetrics);
}

NodeMutation NodeStore::updatePowerMetrics(NodeId id, const meshtastic_PowerMetrics &metrics)
{
    auto it = nodes.find(id);
    if (it == nodes.end() || (!metrics.has_ch1_voltage && !metrics.has_ch1_current))
        return unchanged(id);

    auto retainedMetrics = it->second.environmentMetrics;
    if (metrics.has_ch1_voltage) {
        retainedMetrics.voltage = metrics.ch1_voltage;
    }
    if (metrics.has_ch1_current) {
        retainedMetrics.current = metrics.ch1_current;
    }
    if (sameEnvironmentMetrics(it->second.environmentMetrics, retainedMetrics))
        return unchanged(id);

    it->second.environmentMetrics = retainedMetrics;
    return updated(id, it->second.hasEnvironmentMetrics ? NodeFieldEnvironmentMetrics : NodeFieldPowerMetrics);
}

NodeMutation NodeStore::updateSignal(NodeId id, int32_t rssi, float snr)
{
    auto it = nodes.find(id);
    if (it == nodes.end())
        return unchanged(id);
    const bool changed = it->second.rssi != rssi || it->second.snr != snr || it->second.hopsAway != 0 ||
                         it->second.signalDisplay != NodeSignalDisplayKind::Rssi;
    it->second.rssi = rssi;
    it->second.snr = snr;
    it->second.hopsAway = 0;
    it->second.signalDisplay = NodeSignalDisplayKind::Rssi;
    if (!changed)
        return unchanged(id);
    return updated(id, NodeFieldSignal | NodeFieldHops);
}

NodeMutation NodeStore::updateHops(NodeId id, int8_t hopsAway)
{
    auto it = nodes.find(id);
    if (it == nodes.end())
        return unchanged(id);
    const bool changed = it->second.hopsAway != hopsAway || it->second.signalDisplay != NodeSignalDisplayKind::Hops;
    it->second.hopsAway = hopsAway;
    it->second.signalDisplay = NodeSignalDisplayKind::Hops;
    if (!changed)
        return unchanged(id);
    return updated(id, NodeFieldHops);
}

NodeMutation NodeStore::updateLastHeard(NodeId id, uint32_t now)
{
    auto it = nodes.find(id);
    if (it == nodes.end())
        return unchanged(id);
    if (it->second.lastHeard == now) {
        touchRecency(it->second);
        return updated(id, NodeFieldLastHeard);
    }
    it->second.lastHeard = now;
    touchRecency(it->second);
    return updated(id, NodeFieldLastHeard);
}

NodeMutation NodeStore::markBadKey(NodeId id)
{
    auto it = nodes.find(id);
    if (it == nodes.end() || it->second.hasBadKey)
        return unchanged(id);
    it->second.hasBadKey = true;
    return updated(id, NodeFieldFlags);
}

NodeMutation NodeStore::setActiveChat(NodeId id, bool active)
{
    auto it = nodes.find(id);
    if (it == nodes.end() || it->second.hasActiveChat == active)
        return unchanged(id);
    it->second.hasActiveChat = active;
    return updated(id, NodeFieldActiveChat);
}

NodeMutation NodeStore::remove(NodeId id)
{
    if (nodes.erase(id) == 0)
        return unchanged(id);
    return {NodeMutationKind::Removed, id, NodeFieldNone};
}

NodeId NodeStore::selectPurgeCandidate(NodeId incoming, NodeId ownNode, uint32_t now) const
{
    if (nodes.size() <= 1)
        return 0;

    const auto older = [now](const NodeRecord *left, const NodeRecord *right) {
        const uint32_t leftHeard = effectiveLastHeard(*left, now);
        const uint32_t rightHeard = effectiveLastHeard(*right, now);
        if (leftHeard != rightHeard)
            return leftHeard < rightHeard;
        if (left->recencyPromoted != right->recencyPromoted)
            return !left->recencyPromoted;
        if (left->recencyOrder != right->recencyOrder)
            return left->recencyPromoted ? left->recencyOrder < right->recencyOrder : left->recencyOrder > right->recencyOrder;
        return left->id < right->id;
    };

    const auto removable = [incoming, ownNode](const NodeRecord &record) {
        return record.id != incoming && record.id != ownNode && !record.hasActiveChat;
    };
    const auto staleUnknown = [now](const NodeRecord &record) {
        return !record.hasUser && now >= record.lastHeard && now - record.lastHeard >= purgeFreshnessSeconds;
    };

    const NodeRecord *oldestRemovable = nullptr;
    const NodeRecord *oldestStaleUnknown = nullptr;
    for (const auto &[id, record] : nodes) {
        if (removable(record)) {
            if (!oldestRemovable || older(&record, oldestRemovable)) {
                oldestRemovable = &record;
            }
            if (staleUnknown(record) && (!oldestStaleUnknown || older(&record, oldestStaleUnknown))) {
                oldestStaleUnknown = &record;
            }
        }
    }

    if (oldestStaleUnknown) {
        const size_t preferredPopulation = (nodes.size() * 4 + 4) / 5;
        size_t staleUnknownRank = 1;
        for (const auto &[id, record] : nodes) {
            if (older(&record, oldestStaleUnknown)) {
                ++staleUnknownRank;
            }
        }
        if (staleUnknownRank <= preferredPopulation) {
            return oldestStaleUnknown->id;
        }
    }

    return oldestRemovable ? oldestRemovable->id : 0;
}
