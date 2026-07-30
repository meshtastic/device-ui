#include "graphics/common/ChannelShareURL.h"
#include "mesh/generated/meshtastic/apponly.pb.h"
#include "util/macaron_Base64.h"
#include <algorithm>
#include <pb_encode.h>

namespace {

std::string base64UrlEncode(const uint8_t *bytes, size_t size)
{
    std::string encoded = macaron::Base64::Encode(bytes, size);
    for (char &character : encoded) {
        if (character == '+')
            character = '-';
        else if (character == '/')
            character = '_';
    }
    while (!encoded.empty() && encoded.back() == '=') {
        encoded.pop_back();
    }
    return encoded;
}

} // namespace

std::string ChannelShareURL::make(const meshtastic_ChannelSettings *settings, size_t settingCount,
                                  const meshtastic_Config_LoRaConfig &lora, ChannelShareMode mode)
{
    if (!settings || settingCount == 0 || settingCount > 8) {
        return {};
    }

    meshtastic_ChannelSet channelSet = meshtastic_ChannelSet_init_zero;
    channelSet.settings_count = static_cast<pb_size_t>(settingCount);
    std::copy_n(settings, settingCount, channelSet.settings);
    if (mode == ChannelShareMode::Replace) {
        channelSet.has_lora_config = true;
        channelSet.lora_config = lora;
    }

    uint8_t payload[meshtastic_ChannelSet_size];
    pb_ostream_t stream = pb_ostream_from_buffer(payload, sizeof(payload));
    if (!pb_encode(&stream, &meshtastic_ChannelSet_msg, &channelSet)) {
        return {};
    }

    const char *prefix = mode == ChannelShareMode::Add ? "https://meshtastic.org/e/?add=true#" : "https://meshtastic.org/e/#";
    return std::string(prefix) + base64UrlEncode(payload, stream.bytes_written);
}
