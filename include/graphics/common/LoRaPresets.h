#pragma once

#include "mesh-pb-constants.h"
#include <stdint.h>

class LoRaPresets
{
  public:
    using DefaultPreset = meshtastic_Config_LoRaConfig_ModemPreset;

    struct RegionInfo {
        const char *region;
        float freqStart;
        float freqEnd;
        DefaultPreset preset = meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST;
        float spacing = 0.0f;
        float padding = 0.0f;
        float unitChannelMHz = 0.0f;
        int16_t overrideSlot = 0; // 0 = channel name hash, >0 = explicit slot
        bool wideLora = false;
    };

    struct SlotPlan {
        float width;       // MHz between consecutive slot centers
        uint32_t count;    // number of full slots fitting within [freqStart, freqEnd]
        float firstCenter; // MHz center frequency of slot 1 (1-based)
    };

    static const char *loRaRegionToString(meshtastic_Config_LoRaConfig_RegionCode region);
    static float getFrequencyStart(meshtastic_Config_LoRaConfig_RegionCode region);
    static float getFrequencyEnd(meshtastic_Config_LoRaConfig_RegionCode region);
    static meshtastic_Config_LoRaConfig_ModemPreset getDefaultPreset(meshtastic_Config_LoRaConfig_RegionCode region);
    static uint16_t getDefaultSlot(meshtastic_Config_LoRaConfig_RegionCode region,
                                   meshtastic_Config_LoRaConfig_ModemPreset preset, const char *channelName = nullptr);

    struct ModemPreset {
        const char *preset;
        const char *bandwidth_kHz;
        float bandwidth_MHz;
    };

    static float getBandwidth(meshtastic_Config_LoRaConfig_ModemPreset preset);
    static float getBandwidth(meshtastic_Config_LoRaConfig_RegionCode region, meshtastic_Config_LoRaConfig_ModemPreset preset);
    static const char *getBandwidthString(meshtastic_Config_LoRaConfig_ModemPreset preset);
    static const char *getBandwidthString(meshtastic_Config_LoRaConfig_RegionCode region,
                                          meshtastic_Config_LoRaConfig_ModemPreset preset);
    static const char *modemPresetToString(meshtastic_Config_LoRaConfig_ModemPreset preset);

    static SlotPlan getSlotPlan(meshtastic_Config_LoRaConfig_RegionCode region, meshtastic_Config_LoRaConfig_ModemPreset preset);
    static uint32_t getNumChannels(meshtastic_Config_LoRaConfig_RegionCode region,
                                   meshtastic_Config_LoRaConfig_ModemPreset preset);
    static float getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode region, meshtastic_Config_LoRaConfig_ModemPreset preset,
                              uint32_t channel);

  private:
    static struct RegionInfo regionInfo[];
    static struct ModemPreset modemPreset[];
};
