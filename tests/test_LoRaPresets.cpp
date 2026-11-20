#include "graphics/common/LoRaPresets.h"
#include <cmath>
#include <doctest/doctest.h>
#include <string>

namespace
{
// Absolute tolerance in MHz (1 kHz) for center frequencies and slot widths; doctest::Approx::epsilon()
// is relative and would allow ~0.92 MHz of error at 920 MHz.
constexpr float kFreqTolMHz = 0.001f;
constexpr float kBwTolMHz = 0.0001f;
} // namespace

TEST_CASE("LoRaPresets aligns JP to the ARIB STD-T108 200 kHz unit-channel grid")
{
    constexpr auto jp = meshtastic_Config_LoRaConfig_RegionCode_JP;

    SUBCASE("250 kHz presets bond 2 unit channels without crossing 922.3 MHz (4 lower + 3 upper = 7 slots)")
    {
        constexpr auto preset = meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST;
        const auto plan = LoRaPresets::getSlotPlan(jp, preset);

        CHECK(std::fabs(plan.width - 0.400f) <= kFreqTolMHz);
        CHECK(plan.count == 7);
        CHECK(plan.lowerCount == 4);
        CHECK(std::fabs(plan.firstCenter - 920.700f) <= kFreqTolMHz);
        CHECK(LoRaPresets::getNumChannels(jp, preset) == 7);

        const float expected[7] = {920.700f, 921.100f, 921.500f, 921.900f, 922.500f, 922.900f, 923.300f};
        for (uint32_t slot = 1; slot <= 7; slot++) {
            const float freq = LoRaPresets::getRadioFreq(jp, preset, slot);
            CAPTURE(slot);
            CAPTURE(freq);
            CHECK(std::fabs(freq - expected[slot - 1]) <= kFreqTolMHz);
        }
        CHECK(LoRaPresets::getDefaultSlot(jp, preset) == 6);
    }

    SUBCASE("500 kHz turbo presets bond 3 unit channels (3 lower + 2 upper = 5 slots)")
    {
        constexpr auto preset = meshtastic_Config_LoRaConfig_ModemPreset_SHORT_TURBO;
        const auto plan = LoRaPresets::getSlotPlan(jp, preset);

        CHECK(std::fabs(plan.width - 0.600f) <= kFreqTolMHz);
        CHECK(plan.count == 5);
        CHECK(plan.lowerCount == 3);
        CHECK(std::fabs(plan.firstCenter - 920.800f) <= kFreqTolMHz);
        const float expected[5] = {920.800f, 921.400f, 922.000f, 922.600f, 923.200f};
        for (uint32_t slot = 1; slot <= 5; slot++) {
            const float freq = LoRaPresets::getRadioFreq(jp, preset, slot);
            CAPTURE(slot);
            CAPTURE(freq);
            CHECK(std::fabs(freq - expected[slot - 1]) <= kFreqTolMHz);
        }
        CHECK(LoRaPresets::getDefaultSlot(jp, preset) == 1);
    }

    SUBCASE("125 kHz and 62.5 kHz presets occupy 1 unit channel (9 lower + 6 upper = 15 slots)")
    {
        for (auto preset :
             {meshtastic_Config_LoRaConfig_ModemPreset_LITE_FAST, meshtastic_Config_LoRaConfig_ModemPreset_NARROW_SLOW}) {
            const auto plan = LoRaPresets::getSlotPlan(jp, preset);
            CHECK(std::fabs(plan.width - 0.200f) <= kFreqTolMHz);
            CHECK(plan.count == 15);
            CHECK(plan.lowerCount == 9);
            CHECK(std::fabs(plan.firstCenter - 920.600f) <= kFreqTolMHz);
            CHECK(std::fabs(LoRaPresets::getRadioFreq(jp, preset, 1) - 920.600f) <= kFreqTolMHz);
            CHECK(std::fabs(LoRaPresets::getRadioFreq(jp, preset, 9) - 922.200f) <= kFreqTolMHz);
            CHECK(std::fabs(LoRaPresets::getRadioFreq(jp, preset, 10) - 922.400f) <= kFreqTolMHz);
            CHECK(std::fabs(LoRaPresets::getRadioFreq(jp, preset, 15) - 923.400f) <= kFreqTolMHz);
        }
    }
}

TEST_CASE("LoRaPresets matches firmware spacing, padding, override slots, and wideLoRa")
{
    SUBCASE("US and EU_868 standard slots")
    {
        CHECK(LoRaPresets::getNumChannels(meshtastic_Config_LoRaConfig_RegionCode_US,
                                          meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST) == 104);
        CHECK(LoRaPresets::getDefaultSlot(meshtastic_Config_LoRaConfig_RegionCode_US,
                                          meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST) == 20);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_US,
                                                  meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST, 20) -
                        906.875f) <= kFreqTolMHz);

        CHECK(LoRaPresets::getNumChannels(meshtastic_Config_LoRaConfig_RegionCode_EU_868,
                                          meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST) == 1);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_EU_868,
                                                  meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST, 1) -
                        869.525f) <= kFreqTolMHz);
    }

    SUBCASE("EU_866 spaced + padded LITE channels")
    {
        constexpr auto r = meshtastic_Config_LoRaConfig_RegionCode_EU_866;
        constexpr auto p = meshtastic_Config_LoRaConfig_ModemPreset_LITE_FAST;
        CHECK(LoRaPresets::getNumChannels(r, p) == 4);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(r, p, 1) - 865.700f) <= kFreqTolMHz);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(r, p, 2) - 866.300f) <= kFreqTolMHz);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(r, p, 3) - 866.900f) <= kFreqTolMHz);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(r, p, 4) - 867.500f) <= kFreqTolMHz);
    }

    SUBCASE("EU_N_868 narrow channels and override slot")
    {
        constexpr auto r = meshtastic_Config_LoRaConfig_RegionCode_EU_N_868;
        constexpr auto p = meshtastic_Config_LoRaConfig_ModemPreset_NARROW_SLOW;
        CHECK(LoRaPresets::getDefaultPreset(r) == p);
        CHECK(LoRaPresets::getNumChannels(r, p) == 3);
        CHECK(LoRaPresets::getDefaultSlot(r, p) == 1);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(r, p, 1) - 869.442f) <= kFreqTolMHz);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(r, p, 2) - 869.525f) <= kFreqTolMHz);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(r, p, 3) - 869.608f) <= kFreqTolMHz);
    }

    SUBCASE("ITU amateur 2m, 70cm, and 125cm bands")
    {
        constexpr auto tiny = meshtastic_Config_LoRaConfig_ModemPreset_TINY_FAST;
        CHECK(LoRaPresets::getNumChannels(meshtastic_Config_LoRaConfig_RegionCode_ITU1_2M, tiny) == 100);
        CHECK(LoRaPresets::getDefaultSlot(meshtastic_Config_LoRaConfig_RegionCode_ITU1_2M, tiny) == 26);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_ITU1_2M, tiny, 26) - 144.510f) <=
              kFreqTolMHz);

        CHECK(LoRaPresets::getNumChannels(meshtastic_Config_LoRaConfig_RegionCode_ITU2_2M, tiny) == 200);
        CHECK(LoRaPresets::getDefaultSlot(meshtastic_Config_LoRaConfig_RegionCode_ITU2_2M, tiny) == 51);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_ITU2_2M, tiny, 51) - 145.010f) <=
              kFreqTolMHz);

        CHECK(LoRaPresets::getNumChannels(meshtastic_Config_LoRaConfig_RegionCode_ITU3_2M, tiny) == 200);
        CHECK(LoRaPresets::getDefaultSlot(meshtastic_Config_LoRaConfig_RegionCode_ITU3_2M, tiny) == 33);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_ITU3_2M, tiny, 33) - 144.650f) <=
              kFreqTolMHz);

        constexpr auto narrow = meshtastic_Config_LoRaConfig_ModemPreset_NARROW_SLOW;
        CHECK(LoRaPresets::getNumChannels(meshtastic_Config_LoRaConfig_RegionCode_ITU2_125CM, narrow) == 50);
        CHECK(LoRaPresets::getDefaultSlot(meshtastic_Config_LoRaConfig_RegionCode_ITU2_125CM, narrow) == 37);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_ITU2_125CM, narrow, 37) - 223.650f) <=
              kFreqTolMHz);

        CHECK(LoRaPresets::getNumChannels(meshtastic_Config_LoRaConfig_RegionCode_ITU2_70CM, narrow) == 300);
        CHECK(LoRaPresets::getDefaultSlot(meshtastic_Config_LoRaConfig_RegionCode_ITU2_70CM, narrow) == 137);
        CHECK(std::fabs(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_ITU2_70CM, narrow, 137) - 433.650f) <=
              kFreqTolMHz);
    }

    SUBCASE("LORA_24 uses wideLoRa bandwidths")
    {
        constexpr auto r = meshtastic_Config_LoRaConfig_RegionCode_LORA_24;
        CHECK(std::fabs(LoRaPresets::getBandwidth(r, meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST) - 0.8125f) <= kBwTolMHz);
        CHECK(std::string(LoRaPresets::getBandwidthString(r, meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST)) == "812.5");
        CHECK(LoRaPresets::getNumChannels(r, meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST) == 102);

        CHECK(std::fabs(LoRaPresets::getBandwidth(r, meshtastic_Config_LoRaConfig_ModemPreset_LONG_SLOW) - 0.40625f) <=
              kBwTolMHz);
        CHECK(LoRaPresets::getNumChannels(r, meshtastic_Config_LoRaConfig_ModemPreset_LONG_SLOW) == 205);

        CHECK(std::fabs(LoRaPresets::getBandwidth(r, meshtastic_Config_LoRaConfig_ModemPreset_SHORT_TURBO) - 1.625f) <=
              kBwTolMHz);
        CHECK(std::string(LoRaPresets::getBandwidthString(r, meshtastic_Config_LoRaConfig_ModemPreset_SHORT_TURBO)) == "1625");
        CHECK(LoRaPresets::getNumChannels(r, meshtastic_Config_LoRaConfig_ModemPreset_SHORT_TURBO) == 51);
    }
}

TEST_CASE("LoRaPresets handles UNSET, channel 0, and out-of-range inputs safely")
{
    CHECK(LoRaPresets::getNumChannels(meshtastic_Config_LoRaConfig_RegionCode_UNSET,
                                      meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST) == 0);
    CHECK(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_UNSET,
                                    meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST, 1) == 0.0f);
    CHECK(LoRaPresets::getDefaultSlot(meshtastic_Config_LoRaConfig_RegionCode_UNSET,
                                      meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST) == 0);

    // Channel 0 must not underflow (channel - 1) in unsigned arithmetic
    CHECK(std::fabs(LoRaPresets::getRadioFreq(meshtastic_Config_LoRaConfig_RegionCode_JP,
                                              meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST, 0) -
                    920.700f) <= kFreqTolMHz);

    // Out-of-range region and preset enums must fail safely
    const auto bogusRegion = static_cast<meshtastic_Config_LoRaConfig_RegionCode>(999);
    const auto bogusPreset = static_cast<meshtastic_Config_LoRaConfig_ModemPreset>(999);
    CHECK(LoRaPresets::getNumChannels(bogusRegion, meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST) == 0);
    CHECK(LoRaPresets::getNumChannels(meshtastic_Config_LoRaConfig_RegionCode_JP, bogusPreset) == 0);
    CHECK(LoRaPresets::getRadioFreq(bogusRegion, meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST, 1) == 0.0f);
    CHECK(LoRaPresets::getDefaultSlot(bogusRegion, meshtastic_Config_LoRaConfig_ModemPreset_LONG_FAST) == 0);
}

TEST_CASE("LoRaPresets keeps every region's default-preset slots inside [freqStart, freqEnd]")
{
    for (int code = meshtastic_Config_LoRaConfig_RegionCode_US; code <= meshtastic_Config_LoRaConfig_RegionCode_ITU2_125CM;
         code++) {
        const auto region = static_cast<meshtastic_Config_LoRaConfig_RegionCode>(code);
        const auto preset = LoRaPresets::getDefaultPreset(region);
        const float freqStart = LoRaPresets::getFrequencyStart(region);
        const float freqEnd = LoRaPresets::getFrequencyEnd(region);
        const float halfBw = LoRaPresets::getBandwidth(region, preset) * 0.5f;
        const auto plan = LoRaPresets::getSlotPlan(region, preset);

        CAPTURE(code);
        CAPTURE(LoRaPresets::loRaRegionToString(region));
        REQUIRE(plan.count >= 1);
        float prevUpper = 0.0f;
        for (uint32_t s = 0; s < plan.count; s++) {
            const float center = plan.centerFreq(s);
            const float lo = center - halfBw;
            const float hi = center + halfBw;
            CHECK(lo >= freqStart - 1e-4f);
            CHECK(hi <= freqEnd + 1e-4f);
            if (s > 0) {
                CHECK(lo >= prevUpper - 1e-4f);
            }
            if (region == meshtastic_Config_LoRaConfig_RegionCode_JP) {
                CHECK((hi <= 922.3f + 1e-4f || lo >= 922.3f - 1e-4f));
            }
            prevUpper = hi;
        }
    }
}
