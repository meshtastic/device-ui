#include "graphics/common/NodeDiscoverySyncGate.h"
#include "graphics/common/NodeStore.h"

#include <doctest/doctest.h>

namespace
{
NodeMutation inserted(NodeId id)
{
    return {NodeMutationKind::Inserted, id, NodeFieldNone};
}

NodeMutation updated(NodeId id)
{
    return {NodeMutationKind::Updated, id, NodeFieldUser};
}
} // namespace

TEST_CASE("node discovery sync gate keeps the first insert deadline")
{
    NodeDiscoverySyncGate gate;

    CHECK(gate.observe(inserted(1), 1000) == NodeDiscoverySyncGate::MutationPolicy::Defer);
    CHECK(gate.pending());
    CHECK_FALSE(gate.due(1099));

    CHECK(gate.observe(inserted(2), 1050) == NodeDiscoverySyncGate::MutationPolicy::Defer);
    CHECK_FALSE(gate.due(1099));
    CHECK(gate.due(1100));
}

TEST_CASE("node discovery sync gate requests forced rebind for later changed mutations")
{
    NodeDiscoverySyncGate gate;

    CHECK(gate.observe(inserted(1), 1000) == NodeDiscoverySyncGate::MutationPolicy::Defer);
    CHECK(gate.observe(updated(1), 1010) == NodeDiscoverySyncGate::MutationPolicy::Defer);

    CHECK(gate.pending());
    CHECK(gate.forceRebind());
    CHECK(gate.due(1100));
}

TEST_CASE("node discovery sync gate consumes pending state after full sync")
{
    NodeDiscoverySyncGate gate;

    CHECK(gate.observe(inserted(1), 1000) == NodeDiscoverySyncGate::MutationPolicy::Defer);
    CHECK(gate.observe(updated(1), 1010) == NodeDiscoverySyncGate::MutationPolicy::Defer);

    gate.consumeFullSync();

    CHECK_FALSE(gate.pending());
    CHECK_FALSE(gate.forceRebind());
    CHECK_FALSE(gate.due(1100));
    CHECK(gate.observe(updated(1), 1110) == NodeDiscoverySyncGate::MutationPolicy::PassThrough);
}
