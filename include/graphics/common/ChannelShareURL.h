#pragma once

#include "mesh-pb-constants.h"
#include <cstddef>
#include <string>

enum class ChannelShareMode {
    Replace,
    Add,
};

namespace ChannelShareURL {

std::string make(const meshtastic_ChannelSettings *settings, size_t settingCount,
                 const meshtastic_Config_LoRaConfig &lora, ChannelShareMode mode);

} // namespace ChannelShareURL
