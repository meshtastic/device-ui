#pragma once

#include "fonts.h"
#include "graphics/common/NodeStore.h"
#include "lvgl.h"
#include "meshtastic/mesh.pb.h"
#include "meshtastic/telemetry.pb.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <tuple>

namespace NodeListRowPresentation
{
inline std::tuple<uint32_t, uint32_t> nodeColors(uint32_t nodeNum)
{
    uint32_t red = (nodeNum & 0xff0000) >> 16;
    uint32_t green = (nodeNum & 0xff00) >> 8;
    uint32_t blue = nodeNum & 0xff;
    while (red + green + blue < 0xF0) {
        red += red / 3 + 10;
        green += green / 3 + 10;
        blue += blue / 3 + 10;
    }
    return std::make_tuple((red << 16) | (green << 8) | blue, (2 * red + 2 * green + blue) > 600 ? 0x000000 : 0xFFFFFF);
}

inline bool containsCaseInsensitive(const char *haystack, const char *needle, size_t needleLength)
{
    if (!haystack || !needle || needleLength == 0) {
        return false;
    }
    const char *haystackEnd = haystack + std::strlen(haystack);
    auto it = std::search(haystack, haystackEnd, needle, needle + needleLength, [](char left, char right) {
        return std::tolower(static_cast<unsigned char>(left)) == std::tolower(static_cast<unsigned char>(right));
    });
    return it != haystackEnd;
}

inline bool containsCaseInsensitive(const char *haystack, const char *needle)
{
    return containsCaseInsensitive(haystack, needle, needle ? std::strlen(needle) : 0);
}

inline void applyNodeImage(lv_obj_t *img, uint32_t nodeNum, const void *source, bool unmessagable, bool resetRecolor)
{
    uint32_t bgColor = 0;
    uint32_t fgColor = 0;
    std::tie(bgColor, fgColor) = nodeColors(nodeNum);
    lv_image_set_src(img, source);
    if (unmessagable) {
        lv_obj_set_style_border_color(img, lv_color_hex(bgColor), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(img, lv_color_hex(0x202020), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_image_recolor(img, lv_color_hex(0xff5555), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_image_recolor_opa(img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        return;
    }

    if (resetRecolor) {
        lv_obj_remove_local_style_prop(img, LV_STYLE_IMAGE_RECOLOR, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_obj_set_style_bg_color(img, lv_color_hex(bgColor), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(img, lv_color_hex(bgColor), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor_opa(img, fgColor ? 0 : 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

// Row short-name fallback: names that render narrower than ~4 px on
// ui_font_montserrat_14 are replaced by the low 16 bits of the node id.
inline bool shortNameFallsBackToId(const char *shortName)
{
    if (!shortName) {
        return true;
    }
    const size_t length = std::strlen(shortName);
    if (length == 0) {
        return true;
    }
    return lv_txt_get_width(shortName, static_cast<uint32_t>(length), &ui_font_montserrat_14, 0) <= 4;
}

inline void formatShortDisplayName(char *dest, size_t destSize, const char *shortName, NodeId nodeId)
{
    if (destSize == 0) {
        return;
    }
    std::memset(dest, 0, destSize);
    if (shortNameFallsBackToId(shortName)) {
        std::snprintf(dest, destSize, "%04x", static_cast<unsigned int>(nodeId & 0xffff));
    } else {
        std::snprintf(dest, destSize, "%s", shortName);
    }
}

// Short-name label composition: the (already fallback resolved) short name is
// padded to four characters and, when both positions are known, followed by a
// second line carrying the flat-earth distance.
inline void formatShortNameWithDistance(char *buffer, size_t bufferSize, const char *shortName, NodeId nodeId,
                                        bool hasOwnPosition, int32_t ownLatitude, int32_t ownLongitude, int32_t latitude,
                                        int32_t longitude, bool metricUnits)
{
    formatShortDisplayName(buffer, bufferSize, shortName, nodeId);

    if (!hasOwnPosition || bufferSize < 6) {
        return;
    }

    size_t shortLength = 0;
    while (shortLength < bufferSize && buffer[shortLength] != '\0') {
        ++shortLength;
    }
    for (size_t i = shortLength; i < 4 && i + 1 < bufferSize; ++i) {
        buffer[i] = ' ';
    }

    const float dx = 71.5f * 1e-7f * (static_cast<float>(ownLongitude) - static_cast<float>(longitude));
    const float dy = 111.3f * 1e-7f * (static_cast<float>(ownLatitude) - static_cast<float>(latitude));
    const float dist = std::sqrt(dx * dx + dy * dy);

    buffer[4] = '\n';
    if (metricUnits) {
        if (dist > 1.0f) {
            std::snprintf(&buffer[5], bufferSize - 5, "%.1f km ", dist);
        } else {
            std::snprintf(&buffer[5], bufferSize - 5, "%u m ", static_cast<unsigned int>(std::round(dist * 1000.0f)));
        }
    } else {
        if (dist > 0.1f) {
            std::snprintf(&buffer[5], bufferSize - 5, "%.1f mi ", dist * 0.621371f);
        } else {
            std::snprintf(&buffer[5], bufferSize - 5, "%u ft ", static_cast<unsigned int>(dist * 3280.84f));
        }
    }
}

// Position detail lines: "%.5f %.5f" coordinates and "<alt><unit> MSL";
// altitude magnitudes of 10000 or more clamp the displayed value to zero.
inline void formatPositionLines(int32_t latitude, int32_t longitude, int32_t altitude, bool metricUnits, char *positionText,
                                size_t positionTextSize, char *altitudeText, size_t altitudeTextSize)
{
    int32_t shownAltitude = altitude > -10000 && altitude < 10000 ? altitude : 0;
    const char *altitudeUnits = "m";
    if (!metricUnits) {
        shownAltitude = static_cast<int32_t>(static_cast<float>(shownAltitude) * 3.28084f);
        altitudeUnits = "ft";
    }
    std::snprintf(positionText, positionTextSize, "%.5f %.5f", latitude * 1e-7, longitude * 1e-7);
    std::snprintf(altitudeText, altitudeTextSize, "%d%s MSL", static_cast<int>(shownAltitude), altitudeUnits);
}

// Environment telemetry lines, including the IAQ chip line.
template <typename Metrics>
inline void formatTelemetryLines(const Metrics &metrics, bool metricUnits, char *telemetry1Text, size_t telemetry1Size,
                                 char *telemetry2Text, size_t telemetry2Size)
{
    telemetry2Text[0] = '\0';
    if (metricUnits) {
        if (static_cast<int>(metrics.relative_humidity) > 0) {
            std::snprintf(telemetry1Text, telemetry1Size, "%2.1f°C %d%% %3.1fhPa", metrics.temperature,
                          static_cast<int>(metrics.relative_humidity), metrics.barometric_pressure);
        } else {
            std::snprintf(telemetry1Text, telemetry1Size, "%2.1f°C %3.1fhPa", metrics.temperature, metrics.barometric_pressure);
        }
    } else {
        if (static_cast<int>(metrics.relative_humidity) > 0) {
            std::snprintf(telemetry1Text, telemetry1Size, "%2.1f°F %d%% %3.1finHg", metrics.temperature * 9 / 5 + 32,
                          static_cast<int>(metrics.relative_humidity), metrics.barometric_pressure / 33.86f);
        } else {
            std::snprintf(telemetry1Text, telemetry1Size, "%2.1f°F %3.1finHg", metrics.temperature * 9 / 5 + 32,
                          metrics.barometric_pressure / 33.86f);
        }
    }

    if (metrics.iaq > 0 && metrics.iaq < 1000) {
        std::snprintf(telemetry2Text, telemetry2Size, "IAQ: %d %.1fV %.1fmA", static_cast<int>(metrics.iaq), metrics.voltage,
                      metrics.current);
    }
}

// IAQ chip palette; returns {textColor, bgColor}.
inline std::tuple<lv_color_t, lv_color_t> iaqColors(uint32_t iaq)
{
    if (iaq <= 50) {
        return {lv_color_hex(0x00000000), lv_color_hex(0x000ce810)};
    } else if (iaq <= 100) {
        return {lv_color_hex(0x00000000), lv_color_hex(0x00faf646)};
    } else if (iaq <= 150) {
        return {lv_color_hex(0x00000000), lv_color_hex(0x00f98204)};
    } else if (iaq <= 200) {
        return {lv_color_hex(0x00000000), lv_color_hex(0x00e42104)};
    } else if (iaq <= 300) {
        return {lv_color_hex(0xffffffff), lv_color_hex(0x009b2970)};
    }
    return {lv_color_hex(0xffffffff), lv_color_hex(0x001d1414)};
}

// Row battery label; levels above 100% are clamped to 100%.
inline void formatBatteryLabel(uint32_t batteryLevel, float voltage, char *buffer, size_t bufferSize)
{
    std::snprintf(buffer, bufferSize, "%u%% %0.2fV", static_cast<unsigned int>(std::min<uint32_t>(batteryLevel, 100)), voltage);
}

// Last-heard buckets (minutes/hours/days, empty after 60 days).
// nowLabel carries the localized "now" string so this header stays i18n-macro free.
inline void formatLastHeardLabel(uint32_t lastHeard, uint32_t currentTime, const char *nowLabel, char *buffer, size_t bufferSize)
{
    if (lastHeard == 0) {
        buffer[0] = '\0';
        return;
    }

    const uint32_t age = currentTime > lastHeard ? currentTime - lastHeard : 0;
    if (age < 60) {
        std::snprintf(buffer, bufferSize, "%s", nowLabel ? nowLabel : "now");
    } else if (age < 3600) {
        std::snprintf(buffer, bufferSize, "%u min", age / 60);
    } else if (age < 86400) {
        std::snprintf(buffer, bufferSize, "%u h", age / 3600);
    } else if (age < 86400U * 60U) {
        std::snprintf(buffer, bufferSize, "%u d", age / 86400);
    } else {
        buffer[0] = '\0';
    }
}
} // namespace NodeListRowPresentation
