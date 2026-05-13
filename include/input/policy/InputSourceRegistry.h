#pragma once

#include "input/policy/IInputSource.h"
#include <memory>
#include <vector>

namespace input_policy
{

class InputSourceRegistry
{
  public:
    using SourcePtr = std::shared_ptr<IInputSource>;

    void clear();
    void registerSource(SourcePtr source);
    const std::vector<SourcePtr> &getSources() const;
    InputCapabilities mergedCapabilities() const;

  private:
    std::vector<SourcePtr> sources;
};

} // namespace input_policy
