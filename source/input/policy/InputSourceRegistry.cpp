#include "input/policy/InputSourceRegistry.h"

namespace input_policy
{

void InputSourceRegistry::clear()
{
    sources.clear();
}

void InputSourceRegistry::registerSource(SourcePtr source)
{
    if (source) {
        sources.push_back(source);
    }
}

const std::vector<InputSourceRegistry::SourcePtr> &InputSourceRegistry::getSources() const
{
    return sources;
}

InputCapabilities InputSourceRegistry::mergedCapabilities() const
{
    InputCapabilities merged{};

    for (const auto &source : sources) {
        if (!source) {
            continue;
        }

        InputCapabilities caps = source->getCapabilities();
        merged.hasArrowKeys = merged.hasArrowKeys || caps.hasArrowKeys;
        merged.hasTabKey = merged.hasTabKey || caps.hasTabKey;
        merged.hasHomeKey = merged.hasHomeKey || caps.hasHomeKey;
        merged.hasEndKey = merged.hasEndKey || caps.hasEndKey;
        merged.hasPgUpPgDownKeys = merged.hasPgUpPgDownKeys || caps.hasPgUpPgDownKeys;
        merged.hasCancelKey = merged.hasCancelKey || caps.hasCancelKey;
        merged.hasEnterKey = merged.hasEnterKey || caps.hasEnterKey;
        merged.hasModifiers = merged.hasModifiers || caps.hasModifiers;
        merged.supportsLongPress = merged.supportsLongPress || caps.supportsLongPress;
        merged.supportsRepeat = merged.supportsRepeat || caps.supportsRepeat;
        merged.supportsTextEntry = merged.supportsTextEntry || caps.supportsTextEntry;
    }

    return merged;
}

} // namespace input_policy
