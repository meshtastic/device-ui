#include "graphics/common/VisibleNodeIndex.h"

#include "graphics/common/NodeListRowPresentation.h"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace
{

bool rankedBefore(const NodeStore &store, uint32_t now, NodeId ownNode, NodeId a, NodeId b)
{
    if (a != b) {
        if (a == ownNode && ownNode != 0) {
            return true;
        }
        if (b == ownNode && ownNode != 0) {
            return false;
        }
    }
    const auto *recA = store.find(a);
    const auto *recB = store.find(b);
    uint32_t lhA = recA ? recA->lastHeard : 0;
    uint32_t lhB = recB ? recB->lastHeard : 0;
    if (now != 0) {
        lhA = lhA > now ? now : lhA;
        lhB = lhB > now ? now : lhB;
    }
    if (lhA != lhB) {
        return lhA > lhB;
    }
    uint64_t recencyA = recA ? recA->recencyOrder : 0;
    uint64_t recencyB = recB ? recB->recencyOrder : 0;
    const bool promotedA = recA && recA->recencyPromoted;
    const bool promotedB = recB && recB->recencyPromoted;
    if (promotedA != promotedB) {
        return promotedA;
    }
    if (recencyA != recencyB) {
        return promotedA ? recencyA > recencyB : recencyA < recencyB;
    }
    return a < b;
}

} // namespace

bool VisibleNodeIndex::isVisible(const NodeRecord &node, const NodeListFilter &filter, NodeId ownNode)
{
    // Own node is never hidden by filters in MUI
    if (node.id == ownNode && ownNode != 0) {
        return true;
    }

    // Filter Unknown: hide if no user data or unknown
    if (filter.unknown) {
        if (!node.hasUser) {
            return false;
        }
    }

    // Filter Offline: hide if lastHeard is 0 or older than secsUntilOffline
    if (filter.offline) {
        if (node.lastHeard == 0) {
            return false;
        }
        if (filter.curTime > 0 && filter.curTime >= node.lastHeard) {
            if ((filter.curTime - node.lastHeard) > filter.secsUntilOffline) {
                return false;
            }
        }
    }

    // Filter Public Key: hide when there is no usable key; a marked bad key
    // no longer counts, matching the row presentation contract.
    if (filter.publicKey) {
        if (!node.hasKey || node.hasBadKey) {
            return false;
        }
    }

    // Filter Channel: dropdown index 0 = all, >0 = channel index (selected - 1)
    if (filter.channel != 0) {
        uint8_t targetChannel = filter.channel - 1;
        if (node.channel != targetChannel) {
            return false;
        }
    }

    // Filter Hops: dropdown index 0 = all
    if (filter.hops != 0) {
        int selected = filter.hops - 7;
        if (node.hopsAway < 0) {
            return false;
        }
        if (selected <= 0) {
            if (node.hopsAway > -selected) {
                return false;
            }
        } else {
            if (node.hopsAway < selected) {
                return false;
            }
        }
    }

    // The MQTT filter stays disabled: viaMqtt never affects visibility.
    (void)filter.viaMqtt;

    // Filter Position: require known coordinates
    if (filter.position) {
        if (!node.position.hasCoordinates()) {
            return false;
        }
    }

    // Filter Name: match against the rendered short-name text, including the
    // id fallback and the distance line.
    if (!filter.name.empty()) {
        auto matchesAny = [&](const std::string &query) {
            if (NodeListRowPresentation::containsCaseInsensitive(node.user.long_name, query.c_str(), query.size())) {
                return true;
            }
            char renderedShort[48];
            if (filter.hasOwnPosition && node.position.hasCoordinates() && node.id != ownNode) {
                NodeListRowPresentation::formatShortNameWithDistance(
                    renderedShort, sizeof(renderedShort), node.user.short_name, node.id, filter.hasOwnPosition,
                    filter.ownLatitude, filter.ownLongitude, node.position.latitude, node.position.longitude, filter.metricUnits);
            } else {
                NodeListRowPresentation::formatShortDisplayName(renderedShort, sizeof(renderedShort), node.user.short_name,
                                                                node.id);
            }
            return NodeListRowPresentation::containsCaseInsensitive(renderedShort, query.c_str(), query.size());
        };

        if (filter.name[0] != '!') {
            if (!matchesAny(filter.name)) {
                return false;
            }
        } else {
            // An empty negation matches every name and therefore hides the row.
            const char *negated = filter.name.c_str() + 1;
            const size_t negatedLength = filter.name.size() - 1;
            if (negatedLength == 0 ||
                NodeListRowPresentation::containsCaseInsensitive(node.user.long_name, negated, negatedLength)) {
                return false;
            }
            char renderedShort[48];
            if (filter.hasOwnPosition && node.position.hasCoordinates() && node.id != ownNode) {
                NodeListRowPresentation::formatShortNameWithDistance(
                    renderedShort, sizeof(renderedShort), node.user.short_name, node.id, filter.hasOwnPosition,
                    filter.ownLatitude, filter.ownLongitude, node.position.latitude, node.position.longitude, filter.metricUnits);
            } else {
                NodeListRowPresentation::formatShortDisplayName(renderedShort, sizeof(renderedShort), node.user.short_name,
                                                                node.id);
            }
            if (NodeListRowPresentation::containsCaseInsensitive(renderedShort, negated, negatedLength)) {
                return false;
            }
        }
    }

    return true;
}

void VisibleNodeIndex::rebuild(const NodeStore &store, const NodeListFilter &filter, NodeId ownNode)
{
    std::vector<NodeId> &next = rebuildScratch;
    next.clear();
    next.reserve(visibleMembership.size());

    for (const auto &pair : store.records()) {
        const auto &record = pair.second;
        if (isVisible(record, filter, ownNode)) {
            if (next.size() < visibleMembership.size()) {
                next.push_back(record.id);
            } else {
                auto worst =
                    std::max_element(next.begin(), next.end(), [&store, now = filter.curTime, ownNode](NodeId a, NodeId b) {
                        return rankedBefore(store, now, ownNode, a, b);
                    });
                if (worst != next.end() && rankedBefore(store, filter.curTime, ownNode, record.id, *worst)) {
                    *worst = record.id;
                }
            }
        }
    }

#ifdef UNIT_TEST
    membershipProbeCount = 0;
#endif
    assert(next.size() <= visibleMembership.size());
    std::sort(next.begin(), next.end());

    bool membershipChanged = next.size() != visibleMembershipSize;
    if (!membershipChanged) {
        for (size_t i = 0; i < next.size(); ++i) {
#ifdef UNIT_TEST
            ++membershipProbeCount;
#endif
            if (next[i] != visibleMembership[i]) {
                membershipChanged = true;
                break;
            }
        }
    }
    if (membershipChanged) {
        std::copy(next.begin(), next.end(), visibleMembership.begin());
        visibleMembershipSize = next.size();
        ++visibleMembershipGeneration;
    }

    // Sort by effective lastHeard descending, then newest same-second mutation
    // first. The own node keeps the first position regardless of freshness.
    std::sort(next.begin(), next.end(),
              [&store, now = filter.curTime, ownNode](NodeId a, NodeId b) { return rankedBefore(store, now, ownNode, a, b); });

    // Only publish (and bump the generation for) genuinely changed orders so
    // redundant syncs can skip their refresh work downstream.
    if (next != visibleIds) {
        visibleIds = next;
        ++rebuildGeneration;
    }
}

std::optional<size_t> VisibleNodeIndex::indexOf(NodeId id) const
{
    for (size_t i = 0; i < visibleIds.size(); ++i) {
        if (visibleIds[i] == id) {
            return i;
        }
    }
    return std::nullopt;
}

bool VisibleNodeIndex::contains(NodeId id) const
{
#ifdef UNIT_TEST
    ++containsCallCount;
#endif
    return std::binary_search(visibleMembership.begin(), visibleMembership.begin() + visibleMembershipSize, id);
}
