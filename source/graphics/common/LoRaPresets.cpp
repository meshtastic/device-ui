#include "graphics/common/LoRaPresets.h"
#include "lv_i18n.h"
#include "util/ILog.h"
#include <iterator>

/**
 * LoRa Presets
 * Note: keep order the same as in meshtastic_Config_LoRaConfig_RegionCode and as in settings_region_dropdown
 * Fields: region, freqStart, freqEnd, defaultPreset, spacing, padding, unitChannelMHz, overrideSlot, wideLora, bandSplitMHz
 */
LoRaPresets::RegionInfo LoRaPresets::regionInfo[] = {
    {"UNSET", 902.0f, 928.0f},
    {"US", 902.0f, 928.0f, meshtastic_Config_LoRaConfig_ModemPreset_LONG_TURBO},
    {"EU_433", 433.0f, 434.0f},
    {"EU_868", 869.4f, 869.65f},
    {"CN", 470.0f, 510.0f},
    {"JP", 920.5f, 923.5f, meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST, 0.0f, 0.0f, 0.2f, 0, false, 922.3f},
    {"ANZ", 915.0f, 928.0f},
    {"KR", 920.0f, 923.0f},
    {"TW", 920.0f, 925.0f},
    {"RU", 868.7f, 869.2f},
    {"IN", 865.0f, 867.0f},
    {"NZ_865", 864.0f, 868.0f},
    {"TH", 920.0f, 925.0f},
    {"LORA_24", 2400.0f, 2483.5f, meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST, 0.0f, 0.0f, 0.0f, 0, true},
    {"UA_433", 433.0f, 434.7f},
    {"UA_868", 868.0f, 868.6f}, // deprecated
    {"MY_433", 433.0f, 435.0f},
    {"MY_919", 919.0f, 924.0f},
    {"SG_923", 917.0f, 925.0f},
    {"PH_433", 433.0f, 434.7f},
    {"PH_868", 868.0f, 869.4f},
    {"PH_915", 915.0f, 918.0f},
    {"ANZ_433", 433.05f, 434.79f},
    {"KZ_433", 433.075f, 434.775f},
    {"KZ_863", 863.0f, 868.0f},
    {"NP_865", 865.0f, 868.0f},
    {"BR_902", 902.0f, 907.5f},
    {"ITU1_2M", 144.0f, 146.0f, meshtastic_Config_LoRaConfig_ModemPreset_TINY_FAST, 0.0f, 0.0022f, 0.0f, 26},
    {"ITU2_2M", 144.0f, 148.0f, meshtastic_Config_LoRaConfig_ModemPreset_TINY_FAST, 0.0f, 0.0022f, 0.0f, 51},
    {"EU_866", 865.6f, 867.6f, meshtastic_Config_LoRaConfig_ModemPreset_LITE_FAST, 0.4f, 0.0375f},
    {"EU_874", 874.0f, 874.4f, meshtastic_Config_LoRaConfig_ModemPreset_LITE_FAST},
    {"EU_917", 917.3f, 919.4f, meshtastic_Config_LoRaConfig_ModemPreset_LITE_FAST},
    {"EU_868_NARROW", 869.4f, 869.65f, meshtastic_Config_LoRaConfig_ModemPreset_NARROW_SLOW, 0.0f, 0.0104f, 0.0f, 1},
    {"ITU3_2M", 144.0f, 148.0f, meshtastic_Config_LoRaConfig_ModemPreset_TINY_FAST, 0.0f, 0.0022f, 0.0f, 33},
    {"ITU1_70CM", 430.0f, 440.0f, meshtastic_Config_LoRaConfig_ModemPreset_NARROW_SLOW, 0.0f, 0.01875f, 0.0f, 37},
    {"ITU2_70CM", 420.0f, 450.0f, meshtastic_Config_LoRaConfig_ModemPreset_NARROW_SLOW, 0.0f, 0.01875f, 0.0f, 137},
    {"ITU3_70CM", 430.0f, 450.0f, meshtastic_Config_LoRaConfig_ModemPreset_NARROW_SLOW, 0.0f, 0.01875f, 0.0f, 37},
    {"ITU2_125CM", 220.0f, 225.0f, meshtastic_Config_LoRaConfig_ModemPreset_NARROW_SLOW, 0.0f, 0.01875f, 0.0f, 37}};

LoRaPresets::ModemPreset LoRaPresets::modemPreset[] = {
    {"LongFast", "250", .250f},     {"LongSlow", "125", .125f},     {"VLongSlow", "62.5", .0625f}, {"MediumSlow", "250", .250f},
    {"MediumFast", "250", .250f},   {"ShortSlow", "250", .250f},    {"ShortFast", "250", .250f},   {"LongMod", "125", .125f},
    {"ShortTurbo", "500", .500f},   {"LongTurbo", "500", .500f},    {"LiteFast", "125", .125f},    {"LiteSlow", "125", .125f},
    {"NarrowFast", "62.5", .0625f}, {"NarrowSlow", "62.5", .0625f}, {"TinyFast", "15.6", .0156f},  {"TinySlow", "15.6", .0156f},
    {"MediumTurbo", "500", .500f}};

namespace
{
bool isValidRegionIndex(meshtastic_Config_LoRaConfig_RegionCode region, std::size_t count)
{
    return region >= 0 && static_cast<std::size_t>(region) < count;
}

bool isValidPresetIndex(meshtastic_Config_LoRaConfig_ModemPreset preset, std::size_t count)
{
    return preset >= 0 && static_cast<std::size_t>(preset) < count;
}
} // namespace

const char *LoRaPresets::loRaRegionToString(meshtastic_Config_LoRaConfig_RegionCode region)
{
    if (isValidRegionIndex(region, std::size(regionInfo))) {
        return regionInfo[region].region;
    }
    return _("<unsupported>");
}

float LoRaPresets::getFrequencyStart(meshtastic_Config_LoRaConfig_RegionCode region)
{
    if (isValidRegionIndex(region, std::size(regionInfo))) {
        return regionInfo[region].freqStart;
    }
    return 0.0f;
}

float LoRaPresets::getFrequencyEnd(meshtastic_Config_LoRaConfig_RegionCode region)
{
    if (isValidRegionIndex(region, std::size(regionInfo))) {
        return regionInfo[region].freqEnd;
    }
    return 0.0f;
}

meshtastic_Config_LoRaConfig_ModemPreset LoRaPresets::getDefaultPreset(meshtastic_Config_LoRaConfig_RegionCode region)
{
    if (isValidRegionIndex(region, std::size(regionInfo))) {
        return regionInfo[region].preset;
    }
    return meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST;
}

/**
 * Default slot number is generated using the same firmware hash algorithm
 */
uint16_t LoRaPresets::getDefaultSlot(meshtastic_Config_LoRaConfig_RegionCode region,
                                     meshtastic_Config_LoRaConfig_ModemPreset preset, const char *channelName)
{
    if (!isValidRegionIndex(region, std::size(regionInfo)) || !isValidPresetIndex(preset, std::size(modemPreset))) {
        return 0;
    }

    auto hash = [](const char *str) -> uint32_t {
        uint32_t hash = 5381;
        unsigned char c;
        while ((c = *str++) != '\0')
            hash += (hash << 5) + c;
        return hash;
    };

    const RegionInfo &info = regionInfo[region];
    uint32_t numChannels = getNumChannels(region, preset);
    if (numChannels == 0) {
        return region == meshtastic_Config_LoRaConfig_RegionCode_UNSET ? 0 : 1;
    }
    if (info.overrideSlot > 0 && static_cast<uint32_t>(info.overrideSlot) <= numChannels) {
        return static_cast<uint16_t>(info.overrideSlot);
    }
    return static_cast<uint16_t>(
        hash(channelName && channelName[0] != '\0' ? channelName : modemPreset[preset].preset) % numChannels + 1);
}

float LoRaPresets::getBandwidth(meshtastic_Config_LoRaConfig_ModemPreset preset)
{
    if (!isValidPresetIndex(preset, std::size(modemPreset))) {
        return 0.0f;
    }
    return modemPreset[preset].bandwidth_MHz;
}

float LoRaPresets::getBandwidth(meshtastic_Config_LoRaConfig_RegionCode region, meshtastic_Config_LoRaConfig_ModemPreset preset)
{
    if (!isValidPresetIndex(preset, std::size(modemPreset))) {
        return 0.0f;
    }
    if (isValidRegionIndex(region, std::size(regionInfo)) && regionInfo[region].wideLora) {
        switch (preset) {
        case meshtastic_Config_LoRaConfig_ModemPreset_SHORT_TURBO:
        case meshtastic_Config_LoRaConfig_ModemPreset_LONG_TURBO:
        case meshtastic_Config_LoRaConfig_ModemPreset_MEDIUM_TURBO:
            return 1.625f;
        case meshtastic_Config_LoRaConfig_ModemPreset_LONG_SLOW:
        case meshtastic_Config_LoRaConfig_ModemPreset_LONG_MODERATE:
            return 0.40625f;
        case meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST:
        case meshtastic_Config_LoRaConfig_ModemPreset_MEDIUM_SLOW:
        case meshtastic_Config_LoRaConfig_ModemPreset_MEDIUM_FAST:
        case meshtastic_Config_LoRaConfig_ModemPreset_SHORT_SLOW:
        case meshtastic_Config_LoRaConfig_ModemPreset_SHORT_FAST:
            return 0.8125f;
        default:
            break;
        }
    }
    return modemPreset[preset].bandwidth_MHz;
}

const char *LoRaPresets::getBandwidthString(meshtastic_Config_LoRaConfig_ModemPreset preset)
{
    if (!isValidPresetIndex(preset, std::size(modemPreset))) {
        return "";
    }
    return modemPreset[preset].bandwidth_kHz;
}

const char *LoRaPresets::getBandwidthString(meshtastic_Config_LoRaConfig_RegionCode region,
                                            meshtastic_Config_LoRaConfig_ModemPreset preset)
{
    if (!isValidPresetIndex(preset, std::size(modemPreset))) {
        return "";
    }
    if (isValidRegionIndex(region, std::size(regionInfo)) && regionInfo[region].wideLora) {
        switch (preset) {
        case meshtastic_Config_LoRaConfig_ModemPreset_SHORT_TURBO:
        case meshtastic_Config_LoRaConfig_ModemPreset_LONG_TURBO:
        case meshtastic_Config_LoRaConfig_ModemPreset_MEDIUM_TURBO:
            return "1625";
        case meshtastic_Config_LoRaConfig_ModemPreset_LONG_SLOW:
        case meshtastic_Config_LoRaConfig_ModemPreset_LONG_MODERATE:
            return "406.25";
        case meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST:
        case meshtastic_Config_LoRaConfig_ModemPreset_MEDIUM_SLOW:
        case meshtastic_Config_LoRaConfig_ModemPreset_MEDIUM_FAST:
        case meshtastic_Config_LoRaConfig_ModemPreset_SHORT_SLOW:
        case meshtastic_Config_LoRaConfig_ModemPreset_SHORT_FAST:
            return "812.5";
        default:
            break;
        }
    }
    return modemPreset[preset].bandwidth_kHz;
}

const char *LoRaPresets::modemPresetToString(meshtastic_Config_LoRaConfig_ModemPreset preset)
{
    if (!isValidPresetIndex(preset, std::size(modemPreset))) {
        return "";
    }
    return modemPreset[preset].preset;
}

LoRaPresets::SlotPlan LoRaPresets::getSlotPlan(meshtastic_Config_LoRaConfig_RegionCode region,
                                               meshtastic_Config_LoRaConfig_ModemPreset preset)
{
    if (region == meshtastic_Config_LoRaConfig_RegionCode_UNSET || !isValidRegionIndex(region, std::size(regionInfo)) ||
        !isValidPresetIndex(preset, std::size(modemPreset))) {
        return {0.0f, 0, 0.0f, 0, 0.0f};
    }

    const RegionInfo &info = regionInfo[region];
    const float bwMHz = getBandwidth(region, preset);
    if (bwMHz <= 0.0f || info.freqEnd <= info.freqStart) {
        return {0.0f, 0, 0.0f, 0, 0.0f};
    }

    const float spacing = info.spacing;
    float padding = info.padding;
    if (info.unitChannelMHz > 0.0f) {
        // Mirror firmware RegionInfo::getSlotPlan(): bond ceil(bw / unit) unit channels symmetrically.
        const uint32_t numUnits = static_cast<uint32_t>((bwMHz / info.unitChannelMHz) * (1.0f - 1e-6f)) + 1;
        const float slotSpan = static_cast<float>(numUnits) * info.unitChannelMHz;
        padding = slotSpan > bwMHz ? (slotSpan - bwMHz) * 0.5f : 0.0f;
    }

    const float width = spacing + (padding * 2.0f) + bwMHz;
    const float centerOffset = (bwMHz * 0.5f) + padding;
    const float lowerFirst = info.freqStart + centerOffset;
    if (width <= 0.0f) {
        return {width, 0, lowerFirst, 0, lowerFirst};
    }

    auto countSubBand = [&](float subStart, float subEnd) -> uint32_t {
        const float span = subEnd - subStart;
        if ((span + spacing) <= 0.0f) {
            return 0;
        }
        // Absorb ~1 ULP of float32 endpoint cancellation (e.g. 922.3f - 920.5f) without inflating narrow slots.
        const float endpointEps = subEnd * 1.5e-7f;
        const float maxEps = width * 0.25f;
        const float tol = endpointEps < maxEps ? endpointEps : maxEps;
        return static_cast<uint32_t>((((span + spacing) + tol) / width) * (1.0f + 1e-6f));
    };

    if (info.bandSplitMHz > info.freqStart && info.bandSplitMHz < info.freqEnd) {
        const uint32_t lowerCount = countSubBand(info.freqStart, info.bandSplitMHz);
        const uint32_t upperCount = countSubBand(info.bandSplitMHz, info.freqEnd);
        const uint32_t total = lowerCount + upperCount;
        const float upperFirst = info.bandSplitMHz + centerOffset;
        const float firstCenter = (lowerCount > 0 || upperCount == 0) ? lowerFirst : upperFirst;
        return {width, total, firstCenter, lowerCount, upperFirst};
    }

    const uint32_t count = countSubBand(info.freqStart, info.freqEnd);
    return {width, count, lowerFirst, count, lowerFirst};
}

uint32_t LoRaPresets::getNumChannels(meshtastic_Config_LoRaConfig_RegionCode region,
                                     meshtastic_Config_LoRaConfig_ModemPreset preset)
{
    return getSlotPlan(region, preset).count;
}

float LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode region, meshtastic_Config_LoRaConfig_ModemPreset preset,
                                uint32_t channel)
{
    const SlotPlan plan = getSlotPlan(region, preset);
    if (plan.count == 0) {
        return 0.0f;
    }
    const uint32_t slot = channel > 0 ? channel : 1;
    return plan.centerFreq(slot - 1);
}
