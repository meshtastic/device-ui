#pragma once

#include "mesh-pb-constants.h"
#include <stdint.h>

class LoRaPresets
{
  public:
    struct RegionInfo {
        const char *region;
        float freqStart;
        float freqEnd;
        uint16_t defaultSlot;
    };

    static const char *loRaRegionToString(meshtastic_Config_LoRaConfig_RegionCode region);
    static float getFrequencyStart(meshtastic_Config_LoRaConfig_RegionCode region);
    static float getFrequencyEnd(meshtastic_Config_LoRaConfig_RegionCode region);
    static uint16_t getDefaultSlot(meshtastic_Config_LoRaConfig_RegionCode region,
                                   meshtastic_Config_LoRaConfig_ModemPreset preset);

    struct ModemPreset {
        const char *preset;
        const char *bandwidth_kHz;
        float bandwidth_MHz;
    };

    static float getBandwidth(meshtastic_Config_LoRaConfig_ModemPreset preset);
    static const char *getBandwidthString(meshtastic_Config_LoRaConfig_ModemPreset preset);
    static const char *modemPresetToString(meshtastic_Config_LoRaConfig_ModemPreset preset);

    static uint32_t getNumChannels(meshtastic_Config_LoRaConfig_RegionCode region,
                                   meshtastic_Config_LoRaConfig_ModemPreset preset);
    static float getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode region, meshtastic_Config_LoRaConfig_ModemPreset preset,
                              uint32_t channel);

  private:
    static struct RegionInfo regionInfo[];
    static struct ModemPreset modemPreset[];
};
