#include "graphics/common/MeshtasticView.h"
#include "graphics/common/NodeStore.h"

#include <doctest/doctest.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <new>

namespace
{
constexpr auto unknownRole = static_cast<meshtastic_Config_DeviceConfig_Role>(15);
thread_local bool countAllocations = false;
thread_local size_t allocationCount = 0;

struct ScopedAllocationCounter {
    ScopedAllocationCounter()
    {
        allocationCount = 0;
        countAllocations = true;
    }

    ~ScopedAllocationCounter() { countAllocations = false; }

    size_t count() const { return allocationCount; }
};

meshtastic_User makeUser(const char *shortName, const char *longName,
                         meshtastic_Config_DeviceConfig_Role role = meshtastic_Config_DeviceConfig_Role_CLIENT)
{
    meshtastic_User user = meshtastic_User_init_default;
    std::snprintf(user.short_name, sizeof(user.short_name), "%s", shortName);
    std::snprintf(user.long_name, sizeof(user.long_name), "%s", longName);
    user.role = role;
    return user;
}
} // namespace

void *operator new(std::size_t size)
{
    if (countAllocations) {
        ++allocationCount;
    }
    void *ptr = std::malloc(size ? size : 1);
    if (!ptr) {
        throw std::bad_alloc();
    }
    return ptr;
}

void operator delete(void *ptr) noexcept
{
    std::free(ptr);
}

void operator delete(void *ptr, std::size_t) noexcept
{
    std::free(ptr);
}

TEST_CASE("node store retains typed user fields without LVGL")
{
    NodeStore store;
    auto user = makeUser("ABCD", "Alpha", meshtastic_Config_DeviceConfig_Role_ROUTER);
    user.public_key.size = 1;
    user.public_key.bytes[0] = 0x42;
    user.has_is_unmessagable = true;
    user.is_unmessagable = true;

    const auto mutation = store.upsertUser(0x12345678, 2, 100, user, true);

    CHECK(mutation.kind == NodeMutationKind::Inserted);
    CHECK(mutation.id == 0x12345678);
    REQUIRE(store.find(0x12345678) != nullptr);
    const auto &record = *store.find(0x12345678);
    CHECK(record.channel == 2);
    CHECK(record.lastHeard == 100);
    CHECK(record.hasUser);
    CHECK(record.user.role == meshtastic_Config_DeviceConfig_Role_ROUTER);
    CHECK(record.user.publicKeySize == 1);
    CHECK(record.user.publicKeyHash != 0);
    CHECK(record.hasKey);
    CHECK(record.unmessagable);
    CHECK(record.viaMqtt);
    CHECK(store.size() == 1);
    CHECK(store.records().size() == 1);
}

TEST_CASE("node record stores compact presentation fields")
{
    CHECK(sizeof(NodeRecord) <= 192);
    CHECK(sizeof(NodeRecord) * MAX_NUM_NODES_VIEW <= 48000);
}

TEST_CASE("node store creates the current fallback identity for an unknown node")
{
    NodeStore store;

    const auto mutation = store.upsertUnknown(0x1234abcd, 4, 250, meshtastic_Config_DeviceConfig_Role_REPEATER, false, true);

    CHECK(mutation.kind == NodeMutationKind::Inserted);
    REQUIRE(store.find(0x1234abcd) != nullptr);
    const auto &record = *store.find(0x1234abcd);
    CHECK_FALSE(record.hasUser);
    CHECK(record.channel == 4);
    CHECK(record.lastHeard == 250);
    CHECK(record.user.short_name == doctest::String("abcd"));
    CHECK(record.user.long_name == doctest::String("Meshtastic abcd"));
    CHECK(record.user.role == meshtastic_Config_DeviceConfig_Role_REPEATER);
    CHECK(record.user.hw_model == meshtastic_HardwareModel_UNSET);
    CHECK_FALSE(record.hasKey);
    CHECK(record.viaMqtt);
}

TEST_CASE("node store reports unchanged and updated duplicate user mutations")
{
    NodeStore store;
    auto user = makeUser("ALPH", "Alpha");
    store.upsertUser(1, 0, 100, user, false);

    CHECK(store.upsertUser(1, 0, 100, user, false).kind == NodeMutationKind::Unchanged);

    user.role = meshtastic_Config_DeviceConfig_Role_SENSOR;
    const auto mutation = store.upsertUser(1, 3, 200, user, true);
    CHECK(mutation.kind == NodeMutationKind::Updated);
    REQUIRE(store.find(1) != nullptr);
    CHECK(store.find(1)->channel == 3);
    CHECK(store.find(1)->lastHeard == 200);
    CHECK(store.find(1)->user.role == meshtastic_Config_DeviceConfig_Role_SENSOR);
    CHECK(store.find(1)->viaMqtt);
}

TEST_CASE("node store updates position telemetry and radio fields on an existing node")
{
    NodeStore store;
    store.upsertUnknown(7, 0, 10, meshtastic_Config_DeviceConfig_Role_CLIENT, false, false);

    NodePosition position{true, 123456789, -987654321, 1234, 9, 22};
    meshtastic_DeviceMetrics device = meshtastic_DeviceMetrics_init_default;
    device.has_battery_level = true;
    device.battery_level = 78;
    meshtastic_EnvironmentMetrics environment = meshtastic_EnvironmentMetrics_init_default;
    environment.has_temperature = true;
    environment.temperature = 22.5f;
    meshtastic_AirQualityMetrics air = meshtastic_AirQualityMetrics_init_default;
    air.has_pm25_standard = true;
    air.pm25_standard = 17;

    CHECK(store.updatePosition(7, position).kind == NodeMutationKind::Updated);
    CHECK(store.updatePosition(7, position).kind == NodeMutationKind::Unchanged);
    CHECK(store.updateDeviceMetrics(7, device).kind == NodeMutationKind::Updated);
    CHECK(store.updateEnvironmentMetrics(7, environment).kind == NodeMutationKind::Updated);
    CHECK(store.updateAirQualityMetrics(7, air).kind == NodeMutationKind::Updated);
    CHECK(store.updateSignal(7, -87, 6.25f).kind == NodeMutationKind::Updated);
    CHECK(store.updateHops(7, 3).kind == NodeMutationKind::Updated);
    CHECK(store.updateLastHeard(7, 999).kind == NodeMutationKind::Updated);
    CHECK(store.setActiveChat(7, true).kind == NodeMutationKind::Updated);

    REQUIRE(store.find(7) != nullptr);
    const auto &record = *store.find(7);
    CHECK(record.position.latitude == 123456789);
    CHECK(record.position.longitude == -987654321);
    CHECK(record.position.altitude == 1234);
    CHECK(record.position.satellites == 9);
    CHECK(record.position.precision == 22);
    CHECK(record.hasDeviceMetrics);
    CHECK(record.deviceMetrics.battery_level == 78);
    CHECK(record.hasEnvironmentMetrics);
    CHECK(record.environmentMetrics.temperature == doctest::Approx(22.5f));
    CHECK(record.hasAirQualityMetrics);
    CHECK(record.airQualityMetrics.pm25_standard == 17);
    CHECK(record.rssi == -87);
    CHECK(record.snr == doctest::Approx(6.25f));
    CHECK(record.hopsAway == 3);
    CHECK(record.lastHeard == 999);
    CHECK(record.hasActiveChat);
}

TEST_CASE("node store carries power telemetry into IAQ environment rows without full protobuf storage")
{
    NodeStore store;
    store.upsertUnknown(7, 0, 10, meshtastic_Config_DeviceConfig_Role_CLIENT, false, false);

    meshtastic_PowerMetrics power = meshtastic_PowerMetrics_init_default;
    power.has_ch1_voltage = true;
    power.ch1_voltage = 4.21f;
    power.has_ch1_current = true;
    power.ch1_current = 12.5f;
    CHECK(store.updatePowerMetrics(7, power).kind == NodeMutationKind::Updated);

    meshtastic_EnvironmentMetrics environment = meshtastic_EnvironmentMetrics_init_default;
    environment.has_temperature = true;
    environment.temperature = 22.5f;
    environment.has_barometric_pressure = true;
    environment.barometric_pressure = 1013.2f;
    environment.has_iaq = true;
    environment.iaq = 88;
    CHECK(store.updateEnvironmentMetrics(7, environment).kind == NodeMutationKind::Updated);

    REQUIRE(store.find(7) != nullptr);
    const auto &record = *store.find(7);
    CHECK(record.environmentMetrics.iaq == 88);
    CHECK(record.environmentMetrics.voltage == doctest::Approx(4.21f));
    CHECK(record.environmentMetrics.current == doctest::Approx(12.5f));

    environment.has_voltage = true;
    environment.voltage = 0.0f;
    environment.has_current = true;
    environment.current = 0.0f;
    CHECK(store.updateEnvironmentMetrics(7, environment).kind == NodeMutationKind::Updated);
    CHECK(store.find(7)->environmentMetrics.voltage == doctest::Approx(0.0f));
    CHECK(store.find(7)->environmentMetrics.current == doctest::Approx(0.0f));
}

TEST_CASE("node store retains channel and last-heard metadata when an unknown update fills identity")
{
    NodeStore store;
    constexpr NodeId nodeId = 0x1a2b3c4d;
    store.upsertUnknown(nodeId, 3, 100, meshtastic_Config_DeviceConfig_Role_CLIENT, false, false);

    const NodeMutation mutation = store.upsertUnknown(nodeId, 7, 200, meshtastic_Config_DeviceConfig_Role_ROUTER, true, true);

    CHECK(mutation.kind == NodeMutationKind::Updated);
    REQUIRE(store.find(nodeId) != nullptr);
    CHECK(store.find(nodeId)->channel == 3);
    CHECK(store.find(nodeId)->lastHeard == 100);
    CHECK(store.find(nodeId)->user.role == meshtastic_Config_DeviceConfig_Role_ROUTER);
    CHECK(store.find(nodeId)->hasKey);
    CHECK(store.find(nodeId)->viaMqtt);
}

TEST_CASE("node store preserves legacy-visible position and battery fields across incomplete updates")
{
    NodeStore store;
    store.upsertUnknown(7, 0, 10, meshtastic_Config_DeviceConfig_Role_CLIENT, false, false);

    const NodePosition validPosition{true, 123456789, -987654321, 1234, 9, 22};
    CHECK(store.updatePosition(7, validPosition).kind == NodeMutationKind::Updated);
    CHECK(store.updatePosition(7, {true, 0, 0, 0, 0, 0}).kind == NodeMutationKind::Unchanged);
    CHECK(store.find(7)->position.latitude == validPosition.latitude);
    CHECK(store.find(7)->position.longitude == validPosition.longitude);

    meshtastic_DeviceMetrics metrics = meshtastic_DeviceMetrics_init_default;
    metrics.has_battery_level = true;
    metrics.battery_level = 78;
    metrics.has_voltage = true;
    metrics.voltage = 4.12f;
    metrics.has_channel_utilization = true;
    metrics.channel_utilization = 12.5f;
    metrics.has_air_util_tx = true;
    metrics.air_util_tx = 3.2f;
    CHECK(store.updateDeviceMetrics(7, metrics).kind == NodeMutationKind::Updated);

    meshtastic_DeviceMetrics zeroBattery = meshtastic_DeviceMetrics_init_default;
    zeroBattery.has_channel_utilization = true;
    zeroBattery.channel_utilization = 0.0f;
    zeroBattery.has_air_util_tx = true;
    zeroBattery.air_util_tx = 0.0f;
    CHECK(store.updateDeviceMetrics(7, zeroBattery).kind == NodeMutationKind::Updated);
    REQUIRE(store.find(7) != nullptr);
    CHECK(store.find(7)->deviceMetrics.battery_level == 78);
    CHECK(store.find(7)->deviceMetrics.voltage == doctest::Approx(4.12f));
    CHECK(store.find(7)->deviceMetrics.channel_utilization == doctest::Approx(0.0f));
    CHECK(store.find(7)->deviceMetrics.air_util_tx == doctest::Approx(0.0f));
}

TEST_CASE("node store accepts positions on the equator and prime meridian")
{
    NodeStore store;
    store.upsertUnknown(7, 0, 10, meshtastic_Config_DeviceConfig_Role_CLIENT, false, false);

    CHECK(store.updatePosition(7, {true, 0, -987654321, 1234, 9, 22}).kind == NodeMutationKind::Updated);
    CHECK(store.find(7)->position.latitude == 0);
    CHECK(store.find(7)->position.longitude == -987654321);

    CHECK(store.updatePosition(7, {true, 123456789, 0, 1234, 9, 22}).kind == NodeMutationKind::Updated);
    CHECK(store.find(7)->position.latitude == 123456789);
    CHECK(store.find(7)->position.longitude == 0);

    CHECK(store.updatePosition(7, {true, 0, 0, 1234, 9, 22}).kind == NodeMutationKind::Unchanged);
}

TEST_CASE("node store treats RSSI after hops as direct and records bad PKI keys")
{
    NodeStore store;
    store.upsertUser(7, 0, 10, makeUser("NODE", "Node"), false);

    store.updateHops(7, 4);
    auto signal = store.updateSignal(7, -87, 6.25f);
    REQUIRE(store.find(7) != nullptr);
    CHECK((signal.changedFields & NodeFieldHops) != 0U);
    CHECK(store.find(7)->hopsAway == 0);
    CHECK(store.find(7)->signalDisplay == NodeSignalDisplayKind::Rssi);

    CHECK(store.markBadKey(7).kind == NodeMutationKind::Updated);
    CHECK(store.find(7)->hasBadKey);
    CHECK(store.markBadKey(7).kind == NodeMutationKind::Unchanged);
}

TEST_CASE("node store clears a bad-key marker when a replacement public key arrives")
{
    NodeStore store;
    auto user = makeUser("NODE", "Node");
    user.public_key.size = 32;
    user.public_key.bytes[0] = 0x11;
    store.upsertUser(7, 0, 10, user, false);
    store.markBadKey(7);

    user.public_key.bytes[0] = 0x22;
    const NodeMutation mutation = store.upsertUser(7, 0, 10, user, false);

    CHECK(mutation.kind == NodeMutationKind::Updated);
    CHECK((mutation.changedFields & NodeFieldFlags) != 0U);
    CHECK_FALSE(store.find(7)->hasBadKey);
}

TEST_CASE("node store retains a bad-key marker when same-key flags change")
{
    NodeStore store;
    auto user = makeUser("NODE", "Node");
    user.public_key.size = 32;
    user.public_key.bytes[0] = 0x11;
    store.upsertUser(7, 0, 10, user, false);
    store.markBadKey(7);

    user.has_is_unmessagable = true;
    user.is_unmessagable = true;
    store.upsertUser(7, 0, 10, user, false);
    REQUIRE(store.find(7) != nullptr);
    CHECK(store.find(7)->hasBadKey);

    store.upsertUser(7, 0, 10, user, true);
    CHECK(store.find(7)->hasBadKey);
}

TEST_CASE("node store field updates do not create incomplete nodes")
{
    NodeStore store;
    const NodePosition position{true, 1, 2, 3, 4, 5};

    CHECK(store.updatePosition(42, position).kind == NodeMutationKind::Unchanged);
    CHECK(store.updateSignal(42, -90, 1.0f).kind == NodeMutationKind::Unchanged);
    CHECK(store.setActiveChat(42, true).kind == NodeMutationKind::Unchanged);
    CHECK(store.find(42) == nullptr);
    CHECK(store.size() == 0);
}

TEST_CASE("node store removes records with explicit mutation results")
{
    NodeStore store;
    store.upsertUnknown(9, 0, 0, meshtastic_Config_DeviceConfig_Role_CLIENT, false, false);

    CHECK(store.remove(9).kind == NodeMutationKind::Removed);
    CHECK(store.find(9) == nullptr);
    CHECK(store.remove(9).kind == NodeMutationKind::Unchanged);
}

TEST_CASE("node store purge candidate prefers a stale unknown from the oldest population")
{
    constexpr uint32_t now = 100000;
    constexpr NodeId incoming = 0x99;
    constexpr NodeId ownNode = 0x01;
    constexpr NodeId oldestKnown = 0x10;
    constexpr NodeId oldestEligibleUnknown = 0x20;
    NodeStore store;

    store.upsertUser(oldestKnown, 0, now - 10000, makeUser("KNWN", "Known"), false);
    store.upsertUnknown(ownNode, 0, now - 9000, meshtastic_Config_DeviceConfig_Role_CLIENT, false, false);
    store.upsertUnknown(0x30, 0, now - 8000, static_cast<uint8_t>(unknownRole), false, false);
    store.setActiveChat(0x30, true);
    store.upsertUnknown(incoming, 0, now - 7000, static_cast<uint8_t>(unknownRole), false, false);
    store.upsertUnknown(oldestEligibleUnknown, 0, now - 5000, static_cast<uint8_t>(unknownRole), false, false);
    store.upsertUnknown(0x40, 0, now - 30, static_cast<uint8_t>(unknownRole), false, false);

    CHECK(store.selectPurgeCandidate(incoming, ownNode, now) == oldestEligibleUnknown);
}

TEST_CASE("node store purge candidate falls back to the oldest removable node")
{
    constexpr uint32_t now = 100000;
    NodeStore store;
    store.upsertUser(0x10, 0, now - 10000, makeUser("OLD", "Old known"), false);
    store.upsertUnknown(0x20, 0, now - 30, static_cast<uint8_t>(unknownRole), false, false);

    CHECK(store.selectPurgeCandidate(0x99, 0x01, now) == 0x10);
}

TEST_CASE("node store purge candidate keeps retained legacy same-timestamp ordering and protection")
{
    constexpr uint32_t now = 1000;
    NodeStore store;

    store.upsertUnknown(0x30000000, 0, 900, static_cast<uint8_t>(unknownRole), false, false);
    store.upsertUnknown(0x10000000, 0, 900, static_cast<uint8_t>(unknownRole), false, false);
    store.upsertUnknown(0x20000000, 0, 900, static_cast<uint8_t>(unknownRole), false, false);

    CHECK(store.selectPurgeCandidate(0x40000000, 0, now) == 0x20000000);

    store.setActiveChat(0x20000000, true);
    CHECK(store.selectPurgeCandidate(0x40000000, 0, now) == 0x10000000);
}

TEST_CASE("node store purge candidate limits stale-unknown preference to the oldest population")
{
    constexpr uint32_t now = 1000;
    NodeStore store;

    store.upsertUser(0x10000000, 0, 100, makeUser("OLD", "Old Named"), false);
    store.upsertUser(0x20000000, 0, 200, makeUser("MID", "Middle Named"), false);
    store.upsertUser(0x30000000, 0, 300, makeUser("NEW", "New Named"), false);
    store.upsertUser(0x40000000, 0, 400, makeUser("NWR", "Newer Named"), false);
    store.upsertUnknown(0x50000000, 0, 800, static_cast<uint8_t>(unknownRole), false, false);

    CHECK(store.selectPurgeCandidate(0x60000000, 0, now) == 0x10000000);
}

TEST_CASE("node store purge candidate includes the stale unknown at the preferred-population boundary")
{
    constexpr uint32_t now = 10000;
    NodeStore store;

    for (uint32_t i = 0; i < 7; ++i) {
        store.upsertUser(0x10000000U + i, 0, 100U + i, makeUser("OLD", "Old Named"), false);
    }
    store.upsertUnknown(0x20000000U, 0, 107U, static_cast<uint8_t>(unknownRole), false, false);
    store.upsertUser(0x30000000U, 0, 108U, makeUser("NEW", "New Named"), false);
    store.upsertUser(0x40000000U, 0, 109U, makeUser("NEW", "Newest Named"), false);

    CHECK(store.selectPurgeCandidate(0x50000000U, 0, now) == 0x20000000U);
}

TEST_CASE("node store purge candidate selection does not allocate at the 250-node limit")
{
    constexpr uint32_t now = 10000;
    constexpr NodeId expected = 0x10000000U;
    NodeStore store;

    for (uint32_t i = 0; i < 250; ++i) {
        store.upsertUnknown(expected + i, 0, 1000U + i, static_cast<uint8_t>(unknownRole), false, false);
    }

    NodeId candidate = 0;
    size_t allocations = 0;
    {
        ScopedAllocationCounter counter;
        candidate = store.selectPurgeCandidate(0x20000000U, 0, now);
        allocations = counter.count();
    }

    CHECK(candidate == expected);
    CHECK(allocations == 0);
}

TEST_CASE("node store purge candidate does not ignore records above the 250-node limit")
{
    constexpr uint32_t now = 10000;
    NodeStore store;

    for (uint32_t i = 0; i < 300; ++i) {
        store.upsertUnknown(0x10000000U + i, 0, 1000U + i, static_cast<uint8_t>(unknownRole), false, false);
    }

    size_t seen = 0;
    NodeId expected = 0;
    for (const auto &[id, record] : store.records()) {
        if (seen++ < 250) {
            store.setActiveChat(id, true);
        } else if (!expected || record.lastHeard < store.find(expected)->lastHeard) {
            expected = id;
        }
    }
    REQUIRE(expected != 0);
    store.updateLastHeard(expected, 1);

    CHECK(store.selectPurgeCandidate(0x40000000U, 0, now) == expected);
}

TEST_CASE("node store purge candidate returns zero when every node is protected")
{
    constexpr uint32_t now = 100000;
    constexpr NodeId incoming = 0x99;
    constexpr NodeId ownNode = 0x01;
    NodeStore store;
    store.upsertUnknown(ownNode, 0, now - 5000, static_cast<uint8_t>(unknownRole), false, false);
    store.upsertUnknown(incoming, 0, now - 4000, static_cast<uint8_t>(unknownRole), false, false);
    store.upsertUnknown(0x30, 0, now - 3000, static_cast<uint8_t>(unknownRole), false, false);
    store.setActiveChat(0x30, true);

    CHECK(store.selectPurgeCandidate(incoming, ownNode, now) == 0);
}

TEST_CASE("node store retains channel validity on invalid NodeInfo updates")
{
    NodeStore store;
    constexpr NodeId existingId = 0x1234;
    constexpr NodeId newId = 0x5678;

    // Existing record keeps its channel when refreshed with sentinel value.
    store.upsertUser(existingId, 2, 100, makeUser("ABCD", "Alpha"), false);
    const auto invalidExisting = store.upsertUser(existingId, c_max_channels, 100, makeUser("EFGH", "Beta"), false);
    REQUIRE(store.find(existingId) != nullptr);
    CHECK(store.find(existingId)->channel == 2);
    CHECK((invalidExisting.changedFields & NodeFieldChannel) == 0U);
    CHECK(std::string(store.find(existingId)->user.long_name) == "Beta");

    // New record with sentinel channel defaults to 0 without dirtying channel.
    const auto invalidNew = store.upsertUser(newId, c_max_channels, 100, makeUser("WXYZ", "Gamma"), false);
    REQUIRE(store.find(newId) != nullptr);
    CHECK(store.find(newId)->channel == 0);
    CHECK((invalidNew.changedFields & NodeFieldChannel) == 0U);
}

TEST_CASE("node store normalizes invalid channels in upsertUnknown as in upsertUser")
{
    NodeStore store;
    constexpr NodeId id = 0x9999;
    // Insert unknown with invalid channel retains default 0
    const auto inserted = store.upsertUnknown(id, c_max_channels, 100, 0, false, false);
    REQUIRE(store.find(id) != nullptr);
    CHECK(store.find(id)->channel == 0);
    CHECK((inserted.changedFields & NodeFieldChannel) == 0U);

    // Unknown refresh retains channel even with valid value (consistent with
    // existing retain test above) and invalid preserves.
    const auto retained = store.upsertUnknown(id, 3, 100, 0, false, false);
    CHECK(store.find(id)->channel == 0);
    CHECK((retained.changedFields & NodeFieldChannel) == 0U);
    const auto invalidRefresh = store.upsertUnknown(id, c_max_channels, 100, 0, false, false);
    CHECK(store.find(id)->channel == 0);
    CHECK((invalidRefresh.changedFields & NodeFieldChannel) == 0U);
}

TEST_CASE("node store never purges its sole record")
{
    NodeStore store;
    store.upsertUnknown(7, 0, 1, static_cast<uint8_t>(unknownRole), false, false);

    CHECK(store.selectPurgeCandidate(99, 1, 1000) == 0);
}
