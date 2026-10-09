#pragma once

#include "graphics/common/NodeStore.h"

#include <cstdint>

class NodeDiscoverySyncGate
{
  public:
    enum class MutationPolicy { Ignore, Defer, PassThrough };

    static constexpr uint32_t DelayMs = 100;

    MutationPolicy observe(const NodeMutation &mutation, uint32_t nowMs)
    {
        if (mutation.kind == NodeMutationKind::Unchanged) {
            return MutationPolicy::Ignore;
        }
        if (mutation.kind == NodeMutationKind::Inserted) {
            if (!syncPending) {
                startedAtMs = nowMs;
            }
            syncPending = true;
            return MutationPolicy::Defer;
        }
        if (syncPending) {
            syncForceRebind = true;
            return MutationPolicy::Defer;
        }
        return MutationPolicy::PassThrough;
    }

    bool pending() const { return syncPending; }
    bool forceRebind() const { return syncForceRebind; }
    bool due(uint32_t nowMs) const { return syncPending && static_cast<uint32_t>(nowMs - startedAtMs) >= DelayMs; }

    void consumeFullSync()
    {
        syncPending = false;
        syncForceRebind = false;
    }

  private:
    bool syncPending = false;
    bool syncForceRebind = false;
    uint32_t startedAtMs = 0;
};
