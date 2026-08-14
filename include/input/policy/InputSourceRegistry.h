#pragma once

#include "input/policy/IInputSource.h"
#include <memory>
#include <vector>

namespace input_policy
{

/**
 * Maintains registered input sources for the current device build.
 * It also computes merged capabilities used by policy composition.
 */
class InputSourceRegistry
{
  public:
    using SourcePtr = std::shared_ptr<IInputSource>;

    // Clears all registered sources.
    void clear();
    // Registers an input source in the registry.
    void registerSource(SourcePtr source);
    // Returns the currently registered source list.
    const std::vector<SourcePtr> &getSources() const;
    // Returns capability flags merged across all registered sources.
    InputCapabilities mergedCapabilities() const;

  private:
    std::vector<SourcePtr> sources;
};

} // namespace input_policy
