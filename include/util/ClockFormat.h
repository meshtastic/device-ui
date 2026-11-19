#pragma once

#include <cstdio>
#include <cstdlib>
#include <ctime>

/**
 * Home screen clock text, e.g. "15:38:12 UTC+1\nSun 15 Nov 2026" (24h)
 * or "03:38:12 PM\nSun 15 Nov 2026" (12h).
 */
inline size_t formatHomeClock(const std::tm &t, bool use12h, char *buf, size_t size)
{
    char hms[16], weekday[8], month[8], zone[16] = "";
    std::strftime(hms, sizeof(hms), use12h ? "%I:%M:%S %p" : "%T", &t);
    std::strftime(weekday, sizeof(weekday), "%a", &t);
    std::strftime(month, sizeof(month), "%b", &t);

    char offset[8] = "";
    if (!use12h && std::strftime(offset, sizeof(offset), "%z", &t) == 5) {
        const long hhmm = std::strtol(offset + 1, nullptr, 10);
        const int hours = hhmm / 100, minutes = hhmm % 100;
        if (hhmm == 0)
            std::snprintf(zone, sizeof(zone), " UTC");
        else if (minutes == 0)
            std::snprintf(zone, sizeof(zone), " UTC%c%d", offset[0], hours);
        else
            std::snprintf(zone, sizeof(zone), " UTC%c%d:%02d", offset[0], hours, minutes);
    }

    const int len = std::snprintf(buf, size, "%s%s\n%s %d %s %d", hms, zone, weekday, t.tm_mday, month, t.tm_year + 1900);
    return len < 0 ? 0 : static_cast<size_t>(len);
}
