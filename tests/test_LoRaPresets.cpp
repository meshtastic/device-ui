#include "graphics/common/LoRaPresets.h"
#include <doctest/doctest.h>

TEST_CASE("LoRaPresets::getRadioFreq returns no frequency without a valid region and slot")
{
    CHECK(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_UNSET,
                                    meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST, 0) == doctest::Approx(0.0f));
    CHECK(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_US,
                                    meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST, 0) == doctest::Approx(0.0f));
    CHECK(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_US,
                                    meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST, 1) == doctest::Approx(902.125f));
}
