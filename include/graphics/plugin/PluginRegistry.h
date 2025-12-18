#pragma once

#include <stdint.h>
#include <string>
#include <unordered_map>

class GfxPlugin;

class PluginRegistry
{
  public:
    // Register a widget by name
    bool registerPlugin(const std::string &name, GfxPlugin *plugin);
    bool deregisterPlugin(GfxPlugin *plugin);

    // Task handler for periodic updates
    virtual void task_handler(uint32_t millis);

    virtual ~PluginRegistry() = default;

  protected:
    // internal registration
    static std::unordered_map<std::string, GfxPlugin *> registeredPlugins;
};