#include "doctest.h"
#include "graphics/common/MeshtasticView.h"
#include "graphics/common/VisibleNodeIndex.h"

#include <algorithm>
#include <cstring>

namespace
{

meshtastic_User makeUser(const char *id, const char *longName, const char *shortName,
                         meshtastic_Config_DeviceConfig_Role role = meshtastic_Config_DeviceConfig_Role_CLIENT,
                         bool withKey = true)
{
    meshtastic_User u{};
    snprintf(u.id, sizeof(u.id), "%s", id);
    snprintf(u.long_name, sizeof(u.long_name), "%s", longName);
    snprintf(u.short_name, sizeof(u.short_name), "%s", shortName);
    u.role = role;
    if (withKey) {
        u.public_key.size = 32;
        memset(u.public_key.bytes, 0x42, 32);
    }
    return u;
}

NodeStore fixtureStore()
{
    NodeStore store;
    // Node 1: Alpha (recent, channel 0, hops 0, with key, with position)
    store.upsertUser(0x1111, 0, 1000, makeUser("!00001111", "Alpha Node", "ALPH"), false);
    NodePosition pos1{true, 377749000, -1224194000, 10, 8, 1};
    store.updatePosition(0x1111, pos1);
    store.updateHops(0x1111, 0);

    // Node 2: Bravo (older, channel 1, hops 2, unknown/fallback, no key)
    store.upsertUnknown(0x2222, 1, 500, 0, false, false);
    store.updateHops(0x2222, 2);

    // Node 3: Charlie (offline / lastHeard=0, channel 0, hops 1, with key)
    store.upsertUser(0x3333, 0, 0, makeUser("!00003333", "Charlie Station", "CHAR"), false);
    store.updateHops(0x3333, 1);

    // Node 4: Delta (medium age, channel 0, hops -1 / direct, with key)
    store.upsertUser(0x4444, 0, 800, makeUser("!00004444", "Delta Gateway", "DELT"), false);

    // Node 5: Own Node (always included regardless of filters)
    store.upsertUser(0x9999, 0, 1000, makeUser("!00009999", "My Self Node", "SELF"), false);

    return store;
}

} // namespace

TEST_CASE("visible index applies current filters and newest-first ordering")
{
    NodeStore store = fixtureStore();
    VisibleNodeIndex index;
    NodeListFilter filter;
    filter.curTime = 1000;
    filter.secsUntilOffline = 300; // nodes older than 700 are offline
    NodeId ownNode = 0x9999;

    SUBCASE("no filter: sorted newest first, then NodeId ascending")
    {
        index.rebuild(store, filter, ownNode);
        // Order expected: own node 0x9999 pinned first, then
        // 0x1111 (1000), 0x4444 (800), 0x2222 (500), 0x3333 (0)
        const auto &ids = index.ids();
        REQUIRE(ids.size() == 5);
        CHECK(ids[0] == 0x9999);
        CHECK(ids[1] == 0x1111);
        CHECK(ids[2] == 0x4444);
        CHECK(ids[3] == 0x2222);
        CHECK(ids[4] == 0x3333);
    }

    SUBCASE("filter unknown: excludes 0x2222 (unknown)")
    {
        filter.unknown = true;
        index.rebuild(store, filter, ownNode);
        CHECK(index.ids() == std::vector<NodeId>{0x9999, 0x1111, 0x4444, 0x3333});
    }

    SUBCASE("filter offline: excludes 0x2222 (500 < 700) and 0x3333 (0)")
    {
        filter.offline = true;
        index.rebuild(store, filter, ownNode);
        CHECK(index.ids() == std::vector<NodeId>{0x9999, 0x1111, 0x4444});
    }

    SUBCASE("filter public key: excludes 0x2222 (hasKey=false)")
    {
        filter.publicKey = true;
        index.rebuild(store, filter, ownNode);
        CHECK(index.ids() == std::vector<NodeId>{0x9999, 0x1111, 0x4444, 0x3333});
    }

    SUBCASE("filter channel: channel 1 (dropdown selected=2 -> channel=1) selects only 0x2222 plus ownNode")
    {
        filter.channel = 2; // channel index 1
        index.rebuild(store, filter, ownNode);
        CHECK(index.ids() == std::vector<NodeId>{0x9999, 0x2222});
    }

    SUBCASE("filter position: requires known coordinates, selects 0x1111 plus ownNode")
    {
        filter.position = true;
        index.rebuild(store, filter, ownNode);
        CHECK(index.ids() == std::vector<NodeId>{0x9999, 0x1111});
    }

    SUBCASE("filter mqtt: remains disabled for legacy-compatible parity")
    {
        store.upsertUser(0x5555, 0, 950, makeUser("!00005555", "MQTT Node", "MQTT"), true);
        filter.viaMqtt = true;
        index.rebuild(store, filter, ownNode);
        CHECK(index.ids() == std::vector<NodeId>{0x9999, 0x1111, 0x5555, 0x4444, 0x2222, 0x3333});
    }

    SUBCASE("filter hops: hops <= 0 (dropdown 7) selects direct nodes (0x1111)")
    {
        filter.hops = 7; // 7 - 7 = 0 -> hopsAway <= 0
        index.rebuild(store, filter, ownNode);
        CHECK(index.contains(0x1111));
        CHECK_FALSE(index.contains(0x2222)); // hops 2
        CHECK_FALSE(index.contains(0x3333)); // hops 1
    }

    SUBCASE("filter hops: hops >= 2 (dropdown 9) selects 0x2222")
    {
        filter.hops = 9; // 9 - 7 = 2 -> hopsAway >= 2
        index.rebuild(store, filter, ownNode);
        CHECK(index.contains(0x2222));
        CHECK_FALSE(index.contains(0x1111)); // hops 0
        CHECK_FALSE(index.contains(0x3333)); // hops 1
    }

    SUBCASE("filter name: case-insensitive search matching 'alp'")
    {
        filter.name = "alp";
        index.rebuild(store, filter, ownNode);
        CHECK(index.ids() == std::vector<NodeId>{0x9999, 0x1111}); // 0x1111 matches, ownNode always kept
    }

    SUBCASE("filter name: search matching hex node ID of unknown node '2222'")
    {
        filter.name = "2222";
        index.rebuild(store, filter, ownNode);
        CHECK(index.contains(0x2222));
        CHECK_FALSE(index.contains(0x1111));
    }

    SUBCASE("filter name: displayed unknown short ID matches but decimal and full hex IDs do not")
    {
        store.upsertUnknown(0x1234abcd, 0, 900, MeshtasticView::unknown, false, false);

        filter.name = "abcd";
        index.rebuild(store, filter, ownNode);
        CHECK(index.contains(0x1234abcd));

        filter.name = "1234abcd";
        index.rebuild(store, filter, ownNode);
        CHECK_FALSE(index.contains(0x1234abcd));

        filter.name = "305441741";
        index.rebuild(store, filter, ownNode);
        CHECK_FALSE(index.contains(0x1234abcd));
    }

    SUBCASE("filter name negation: bare '!' hides every non-own node")
    {
        filter.name = "!";
        index.rebuild(store, filter, ownNode);
        CHECK(index.ids() == std::vector<NodeId>{ownNode});
    }

    SUBCASE("filter name negation: '!' excludes matches")
    {
        filter.name = "!alpha";
        index.rebuild(store, filter, ownNode);
        // Excludes 0x1111, keeps others
        CHECK(std::find(index.ids().begin(), index.ids().end(), 0x1111) == index.ids().end());
        CHECK(index.contains(0x4444));
        CHECK(index.contains(0x9999));
    }
}

TEST_CASE("visible index preserves stable selection lookup after last-heard reorder")
{
    NodeStore store;
    store.upsertUser(0x1111, 0, 500, makeUser("!00001111", "Alpha", "A"), false);
    store.upsertUser(0x2222, 0, 300, makeUser("!00002222", "Bravo", "B"), false);
    store.upsertUser(0x3333, 0, 100, makeUser("!00003333", "Charlie", "C"), false);

    VisibleNodeIndex index;
    NodeListFilter filter;
    index.rebuild(store, filter, 0);

    REQUIRE(index.ids().size() == 3);
    CHECK(index.indexOf(0x2222) == std::optional<size_t>(1));

    // Update Bravo to be newest
    store.updateLastHeard(0x2222, 900);
    index.rebuild(store, filter, 0);

    CHECK(index.indexOf(0x2222) == std::optional<size_t>(0));
    CHECK(index.indexOf(0x1111) == std::optional<size_t>(1));
    CHECK(index.indexOf(0x3333) == std::optional<size_t>(2));
    CHECK(index.indexOf(0x9999) == std::nullopt);
}

TEST_CASE("visible index membership detection stays linear during a recency-only reorder")
{
    NodeStore store;
    constexpr size_t nodeCount = 128;
    for (size_t i = 0; i < nodeCount; ++i) {
        const NodeId id = static_cast<NodeId>(0x1000 + i);
        store.upsertUser(id, 0, static_cast<uint32_t>(nodeCount - i), makeUser("!00001000", "Node", "NODE"), false);
    }

    VisibleNodeIndex index;
    NodeListFilter filter;
    index.rebuild(store, filter, 0);
    const uint32_t membershipGeneration = index.membershipGeneration();

    store.updateLastHeard(0x107f, 1000);
    index.rebuild(store, filter, 0);

    CHECK(index.ids().front() == 0x107f);
    CHECK(index.membershipGeneration() == membershipGeneration);
    CHECK(index.membershipProbeCountForTesting() == nodeCount);
}

TEST_CASE("visible index membership cache keeps fixed storage at the 250-node limit")
{
    NodeStore store;
    constexpr size_t nodeCount = 250;
    for (size_t i = 0; i < nodeCount; ++i) {
        const NodeId id = static_cast<NodeId>(0x1000 + i);
        store.upsertUser(id, 0, static_cast<uint32_t>(nodeCount - i), makeUser("!00001000", "Node", "NODE"), false);
    }

    VisibleNodeIndex index;
    NodeListFilter filter;
    index.rebuild(store, filter, 0);
    const auto *storage = index.membershipStorageForTesting();

    REQUIRE(index.membershipStorageCapacityForTesting() == nodeCount);
    REQUIRE(index.membershipStorageSizeForTesting() == nodeCount);

    store.remove(0x10f9);
    store.upsertUser(0x2000, 0, 1, makeUser("!00002000", "Replacement", "REPL"), false);
    index.rebuild(store, filter, 0);

    CHECK(index.membershipStorageForTesting() == storage);
    CHECK(index.membershipStorageCapacityForTesting() == nodeCount);
    CHECK(index.membershipStorageSizeForTesting() == nodeCount);
    CHECK(index.contains(0x2000));
    CHECK_FALSE(index.contains(0x10f9));
}

TEST_CASE("visible index selects the top-ranked 250 nodes before enforcing membership capacity")
{
    NodeStore store;
    constexpr NodeId ownNode = 0x7000;
    constexpr NodeId lowestPeer = 0x1000;
    constexpr size_t peerCount = 250;

    store.upsertUser(ownNode, 0, 1, makeUser("!00007000", "Own Node", "OWN"), false);
    for (size_t i = 0; i < peerCount; ++i) {
        const NodeId id = static_cast<NodeId>(lowestPeer + i);
        store.upsertUser(id, 0, static_cast<uint32_t>(1000 + i), makeUser("!00001000", "Peer Node", "PEER"), false);
    }

    VisibleNodeIndex index;
    NodeListFilter filter;
    index.rebuild(store, filter, ownNode);

    CHECK(index.membershipStorageSizeForTesting() == 250);
    CHECK(index.membershipStorageCapacityForTesting() == 250);
    CHECK(index.ids().size() == 250);
    CHECK(index.rebuildScratchCapacityForTesting() == 250);
    REQUIRE(index.ids().size() == 250);
    CHECK(index.ids().front() == ownNode);
    CHECK_FALSE(index.contains(lowestPeer));
    CHECK(index.indexOf(lowestPeer) == std::nullopt);
    for (size_t i = 1; i < peerCount; ++i) {
        const NodeId id = static_cast<NodeId>(lowestPeer + i);
        CHECK(index.contains(id));
        CHECK(index.indexOf(id).has_value());
    }
}

TEST_CASE("visible index promotes same-second last-heard refreshes ahead of older ties")
{
    NodeStore store;
    store.upsertUser(0x9000, 0, 500, makeUser("!00009000", "High Id", "HIGH"), false);
    store.upsertUser(0x1000, 0, 500, makeUser("!00001000", "Low Id", "LOW"), false);

    VisibleNodeIndex index;
    NodeListFilter filter;
    index.rebuild(store, filter, 0);
    REQUIRE(index.ids().size() == 2);
    CHECK(index.ids()[0] == 0x9000);
    CHECK(index.ids()[1] == 0x1000);

    store.updateLastHeard(0x9000, 500);
    index.rebuild(store, filter, 0);

    CHECK(index.ids()[0] == 0x9000);
    CHECK(index.ids()[1] == 0x1000);
}

TEST_CASE("visible index clamps future last-heard timestamps before ordering")
{
    NodeStore store;
    store.upsertUser(0x9000, 0, 2000, makeUser("!00009000", "Future High Id", "FUTR"), false);
    store.upsertUser(0x1000, 0, 1000, makeUser("!00001000", "Current Low Id", "CURR"), false);

    VisibleNodeIndex index;
    NodeListFilter filter;
    filter.curTime = 1000;
    index.rebuild(store, filter, 0);

    REQUIRE(index.ids().size() == 2);
    CHECK(index.ids()[0] == 0x9000);
    CHECK(index.ids()[1] == 0x1000);
}

TEST_CASE("visible index keeps reusable scratch capacity after changed rebuilds")
{
    NodeStore store;
    store.upsertUser(0x1111, 0, 300, makeUser("!00001111", "Alpha", "A"), false);
    store.upsertUser(0x2222, 0, 200, makeUser("!00002222", "Bravo", "B"), false);
    store.upsertUser(0x3333, 0, 100, makeUser("!00003333", "Charlie", "C"), false);

    VisibleNodeIndex index;
    NodeListFilter filter;
    index.rebuild(store, filter, 0);
    const size_t reusableCapacity = index.ids().capacity();
    REQUIRE(reusableCapacity >= 3);

    store.updateLastHeard(0x3333, 400);
    index.rebuild(store, filter, 0);

    CHECK(index.ids() == std::vector<NodeId>{0x3333, 0x1111, 0x2222});
    CHECK(index.rebuildScratchCapacityForTesting() >= reusableCapacity);
}

TEST_CASE("visible index public-key filtering tracks key validity transitions")
{
    NodeStore store;
    store.upsertUser(0x1111, 0, 900, makeUser("!00001111", "Keyed Node", "KEY"), false);
    VisibleNodeIndex index;
    NodeListFilter filter;
    filter.publicKey = true;

    index.rebuild(store, filter, 0);
    CHECK(index.contains(0x1111));

    // Key loss drops membership.
    meshtastic_User keyless{};
    std::strcpy(keyless.short_name, "KEY");
    std::strcpy(keyless.long_name, "Keyed Node");
    store.upsertUser(0x1111, 0, 900, keyless, false);
    index.rebuild(store, filter, 0);
    CHECK_FALSE(index.contains(0x1111));

    // Bad-key marking also drops membership while the record still has a key.
    store.upsertUser(0x1111, 0, 900, makeUser("!00001111", "Keyed Node", "KEY"), false);
    store.markBadKey(0x1111);
    index.rebuild(store, filter, 0);
    CHECK_FALSE(index.contains(0x1111));
}

TEST_CASE("visible index treats fully-zero coordinates as unknown position")
{
    NodeStore store;
    store.upsertUser(0x2222, 0, 800, makeUser("!00002222", "Null Island", "NULL"), false);
    store.updatePosition(0x2222, {true, 0, 0, 0, 0, 0});
    NodeListFilter filter;
    filter.position = true;
    VisibleNodeIndex index;
    index.rebuild(store, filter, 0);
    CHECK_FALSE(index.contains(0x2222));
    // Documented edge: a single-zero coordinate still counts as positioned at
    // the model layer, though the panel renderer requires both axes non-zero.
}

TEST_CASE("visible index capacity shares MAX_NUM_NODES_VIEW")
{
    VisibleNodeIndex index;
    CHECK(index.membershipStorageCapacityForTesting() == MAX_NUM_NODES_VIEW);
}
