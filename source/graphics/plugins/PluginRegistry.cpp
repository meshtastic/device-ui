#include "graphics/plugin/PluginRegistry.h"
#include "graphics/plugin/GfxPlugin.h"

std::unordered_map<std::string, GfxPlugin *> PluginRegistry::registeredPlugins;

bool PluginRegistry::registerPlugin(const std::string &name, GfxPlugin *plugin)
{
    auto it = registeredPlugins.find(name);
    if (it != registeredPlugins.end()) {
        return false;
    } else {
        registeredPlugins.insert(std::make_pair(name, plugin));
    }
    return true;
}

bool PluginRegistry::deregisterPlugin(GfxPlugin *plugin)
{
    for (auto &it : registeredPlugins) {
        if (it.second == plugin) {
            registeredPlugins.erase(it.first);
            return true;
        }
    }
    return false;
}

void PluginRegistry::task_handler(uint32_t millis)
{
    for (auto &it : registeredPlugins) {
        it.second->task_handler(millis);
    }
}