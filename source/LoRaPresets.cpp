#include "LoRaPresets.h"

LoRaPresets::RegionInfo LoRaPresets::regionInfo[] = {
    {"UNSET", 902.0f, 928.0f, 20},   {"US", 902.0f, 928.0f, 20},       {"EU_433", 433.0f, 434.0f, 4},
    {"EU_868", 869.4f, 869.65f, 1},  {"CN", 470.0f, 510.0f, 36},       {"JP", 920.8f, 927.8f, 20},
    {"ANZ", 915.0f, 928.0f, 20},     {"KR", 920.0f, 923.0f, 12},       {"TW", 920.0f, 925.0f, 16},
    {"RU", 868.7f, 869.2f, 2},       {"IN", 865.0f, 867.0f, 4},        {"NZ_865", 864.0f, 868.0f, 4},
    {"TH", 920.0f, 925.0f, 16},      {"LORA_24", 2400.0f, 2483.5f, 6}, {"UA_433", 433.0f, 434.7f, 6},  
    {"UA_868", 868.0f, 868.6f, 2},   {"MY_433", 433.0f, 435.0f, 4},    {"MY_919", 919.0f, 924.0f, 16}, 
    {"SG_923", 917.0f, 925.0f, 4}
    };

LoRaPresets::ModemPreset LoRaPresets::modemPreset[] = {{"LONG FAST", "250", .250f},        {"LONG SLOW", "125", .125f},
                                                       {"VERY LONG SLOW", "62.5", .0625f}, {"MEDIUM SLOW", "250", .250f},
                                                       {"MEDIUM FAST", "250", .250f},      {"SHORT SLOW", "250", .250f},
                                                       {"SHORT FAST", "250", .250f},       {"LONG MODERATE", "125", .125f},
                                                       {"SHORT TURBO", "500", .500f}};

const char *LoRaPresets::loRaRegionToString(meshtastic_Config_LoRaConfig_RegionCode region)
{
    return regionInfo[region].region;
}

float LoRaPresets::getFrequencyStart(meshtastic_Config_LoRaConfig_RegionCode region)
{
    return regionInfo[region].freqStart;
}

float LoRaPresets::getFrequencyEnd(meshtastic_Config_LoRaConfig_RegionCode region)
{
    return regionInfo[region].freqEnd;
}

uint16_t LoRaPresets::getDefaultSlot(meshtastic_Config_LoRaConfig_RegionCode region)
{
    return regionInfo[region].defaultSlot;
}

float LoRaPresets::getBandwidth(meshtastic_Config_LoRaConfig_ModemPreset preset)
{
    // TODO: LORA_24 wide mode (3.25 vs. 31/.03125f, 62/.0625f, 200/.203125f, 400/.40625f, 800/.8125f, 1600/1.6250f
    return modemPreset[preset].bandwidth_MHz;
}

const char *LoRaPresets::getBandwidthString(meshtastic_Config_LoRaConfig_ModemPreset preset)
{
    return modemPreset[preset].bandwidth_kHz;
}

const char *LoRaPresets::modemPresetToString(meshtastic_Config_LoRaConfig_ModemPreset preset)
{
    return modemPreset[preset].preset;
}

uint32_t LoRaPresets::getNumChannels(meshtastic_Config_LoRaConfig_RegionCode region,
                                     meshtastic_Config_LoRaConfig_ModemPreset preset)
{
    return (region == meshtastic_Config_LoRaConfig_RegionCode_UNSET
                ? 0
                : uint32_t((regionInfo[region].freqEnd - regionInfo[region].freqStart) / modemPreset[preset].bandwidth_MHz));
}

float LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode region, meshtastic_Config_LoRaConfig_ModemPreset preset,
                                uint32_t channel)
{
    return (regionInfo[region].freqStart + modemPreset[preset].bandwidth_MHz / 2) +
           (channel - 1) * modemPreset[preset].bandwidth_MHz;
}
