#pragma once

#include "graphics/common/NodeStore.h"

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

struct NodeListFilter {
    bool unknown = false;
    bool offline = false;
    bool publicKey = false;
    uint8_t channel = 0; // 0 = all channels, 1..8 = channel + 1
    int hops = 0;        // 0 = all, 1..14 = dropdown index
    bool position = false;
    bool viaMqtt = false;
    std::string name;
    uint32_t curTime = 0;
    uint32_t secsUntilOffline = 900; // default 15 minutes

    // Own-position context so name matching runs against the rendered
    // short-name text, including the id fallback and the distance line.
    bool hasOwnPosition = false;
    int32_t ownLatitude = 0;
    int32_t ownLongitude = 0;
    bool metricUnits = true;
};

class VisibleNodeIndex
{
  public:
    void rebuild(const NodeStore &store, const NodeListFilter &filter, NodeId ownNode);

    const std::vector<NodeId> &ids() const { return visibleIds; }
    size_t size() const { return visibleIds.size(); }
    bool empty() const { return visibleIds.empty(); }
    uint32_t generation() const { return rebuildGeneration; }
    uint32_t membershipGeneration() const { return visibleMembershipGeneration; }

    std::optional<size_t> indexOf(NodeId id) const;
    bool contains(NodeId id) const;

#ifdef UNIT_TEST
    void resetContainsCallCountForTesting() const { containsCallCount = 0; }
    uint32_t containsCallCountForTesting() const { return containsCallCount; }
    uint32_t membershipProbeCountForTesting() const { return membershipProbeCount; }
    size_t rebuildScratchCapacityForTesting() const { return rebuildScratch.capacity(); }
    const NodeId *membershipStorageForTesting() const { return visibleMembership.data(); }
    size_t membershipStorageCapacityForTesting() const { return visibleMembership.size(); }
    size_t membershipStorageSizeForTesting() const { return visibleMembershipSize; }
#endif

    static bool isVisible(const NodeRecord &node, const NodeListFilter &filter, NodeId ownNode);

  private:
    std::vector<NodeId> visibleIds;
    std::vector<NodeId> rebuildScratch;
    std::array<NodeId, MAX_NUM_NODES_VIEW> visibleMembership{};
    size_t visibleMembershipSize = 0;
    uint32_t rebuildGeneration = 0;
    uint32_t visibleMembershipGeneration = 0;
#ifdef UNIT_TEST
    mutable uint32_t containsCallCount = 0;
    uint32_t membershipProbeCount = 0;
#endif
};
