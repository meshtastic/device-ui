#pragma once

#include <stdint.h>

// Local receiver state, independent of position packets and mesh broadcast settings.
struct LocalGPSStatus {
    bool connected = false;
    bool awake = false;
    bool hasTime = false;
    bool hasFix = false;          // Current receiver fix; false while sleeping or stale.
    bool satellitesValid = false; // A count has been received; check its age for freshness.
    uint32_t satellites = 0;      // GGA satellites used, not the number visible in the sky.
    uint32_t satellitesAgeMs = UINT32_MAX;
    bool hasPosition = false; // Last accepted position can remain available during GPS sleep.
    int32_t latitude_i = 0;
    int32_t longitude_i = 0;
    int32_t altitude = 0;
};
