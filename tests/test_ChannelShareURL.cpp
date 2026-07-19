#include "graphics/common/ChannelShareURL.h"
#include "mesh/generated/meshtastic/apponly.pb.h"
#include "util/macaron_Base64.h"
#include <doctest/doctest.h>
#include <pb_decode.h>
#include <cstdio>
#include <cstring>

namespace {

meshtastic_ChannelSettings channelSettings(const char *name, uint8_t seed)
{
    meshtastic_ChannelSettings settings = meshtastic_ChannelSettings_init_zero;
    snprintf(settings.name, sizeof(settings.name), "%s", name);
    settings.psk.size = 16;
    for (uint8_t i = 0; i < settings.psk.size; i++) {
        settings.psk.bytes[i] = seed + i;
    }
    return settings;
}

bool decodeChannelSet(const std::string &url, meshtastic_ChannelSet &result)
{
    size_t payloadStart = url.find('#');
    if (payloadStart == std::string::npos) {
        return false;
    }

    std::string payload = url.substr(payloadStart + 1);
    for (char &character : payload) {
        if (character == '-')
            character = '+';
        else if (character == '_')
            character = '/';
    }
    payload.append((4 - payload.size() % 4) % 4, '=');

    std::string encoded;
    if (!macaron::Base64::Decode(payload, encoded).empty()) {
        return false;
    }

    result = meshtastic_ChannelSet_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(reinterpret_cast<const pb_byte_t *>(encoded.data()), encoded.size());
    return pb_decode(&stream, &meshtastic_ChannelSet_msg, &result);
}

} // namespace

TEST_CASE("Channel share Replace URL carries channels and LoRa")
{
    meshtastic_ChannelSettings settings = channelSettings("Replace", 1);
    meshtastic_Config_LoRaConfig lora = meshtastic_Config_LoRaConfig_init_zero;
    lora.region = meshtastic_Config_LoRaConfig_RegionCode_US;
    lora.modem_preset = meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST;
    lora.channel_num = 7;

    const std::string url = ChannelShareURL::make(&settings, 1, lora, ChannelShareMode::Replace);

    CHECK(url.rfind("https://meshtastic.org/e/#", 0) == 0);
    CHECK(url.find_first_of("+/=", url.find('#') + 1) == std::string::npos);

    meshtastic_ChannelSet decoded = meshtastic_ChannelSet_init_zero;
    REQUIRE(decodeChannelSet(url, decoded));
    REQUIRE(decoded.settings_count == 1);
    CHECK(strcmp(decoded.settings[0].name, "Replace") == 0);
    CHECK(decoded.settings[0].psk.size == settings.psk.size);
    CHECK(memcmp(decoded.settings[0].psk.bytes, settings.psk.bytes, settings.psk.size) == 0);
    REQUIRE(decoded.has_lora_config);
    CHECK(decoded.lora_config.region == lora.region);
    CHECK(decoded.lora_config.modem_preset == lora.modem_preset);
    CHECK(decoded.lora_config.channel_num == lora.channel_num);
}

TEST_CASE("Channel share Add URL omits LoRa for up to eight channels")
{
    meshtastic_ChannelSettings settings[8] = {};
    for (uint8_t i = 0; i < 8; i++) {
        settings[i] = channelSettings("Channel", i);
        snprintf(settings[i].name, sizeof(settings[i].name), "Channel%u", i);
    }
    meshtastic_Config_LoRaConfig lora = meshtastic_Config_LoRaConfig_init_zero;
    lora.region = meshtastic_Config_LoRaConfig_RegionCode_EU_868;

    const std::string url = ChannelShareURL::make(settings, 8, lora, ChannelShareMode::Add);

    CHECK(url.rfind("https://meshtastic.org/e/?add=true#", 0) == 0);

    meshtastic_ChannelSet decoded = meshtastic_ChannelSet_init_zero;
    REQUIRE(decodeChannelSet(url, decoded));
    REQUIRE(decoded.settings_count == 8);
    CHECK(strcmp(decoded.settings[7].name, "Channel7") == 0);
    CHECK_FALSE(decoded.has_lora_config);
}
