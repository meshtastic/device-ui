#include "util/ClockFormat.h"
#include <cstdlib>
#include <ctime>
#include <doctest/doctest.h>
#include <string>

namespace
{
std::string clockAt(const char *tz, time_t utc, bool use12h)
{
    setenv("TZ", tz, 1);
    tzset();
    std::tm local{};
    localtime_r(&utc, &local);
    char buf[48];
    formatHomeClock(local, use12h, buf, sizeof(buf));
    unsetenv("TZ");
    tzset();
    return buf;
}

constexpr time_t kNov15_2026_1438Z = 1794753492; // 2026-11-15 14:38:12 UTC
constexpr time_t kJul05_2026_0807Z = 1783238820; // 2026-07-05 08:07:00 UTC
} // namespace

TEST_CASE("formatHomeClock shows the UTC offset and a full date")
{
    CHECK(clockAt("CET-1CEST,M3.5.0,M10.5.0/3", kNov15_2026_1438Z, false) == "15:38:12 UTC+1\nSun 15 Nov 2026");
    CHECK(clockAt("CET-1CEST,M3.5.0,M10.5.0/3", kJul05_2026_0807Z, false) == "10:07:00 UTC+2\nSun 5 Jul 2026");
    CHECK(clockAt("UTC0", kNov15_2026_1438Z, false) == "14:38:12 UTC\nSun 15 Nov 2026");
}

TEST_CASE("formatHomeClock keeps the offset readable for phone-set GMT zones and fractional offsets")
{
    CHECK(clockAt("GMT-10", kNov15_2026_1438Z, false) == "00:38:12 UTC+10\nMon 16 Nov 2026");
    CHECK(clockAt("IST-5:30", kNov15_2026_1438Z, false) == "20:08:12 UTC+5:30\nSun 15 Nov 2026");
    CHECK(clockAt("NST3:30", kNov15_2026_1438Z, false) == "11:08:12 UTC-3:30\nSun 15 Nov 2026");
}

TEST_CASE("formatHomeClock 12h clock")
{
    CHECK(clockAt("CET-1CEST,M3.5.0,M10.5.0/3", kNov15_2026_1438Z, true) == "03:38:12 PM\nSun 15 Nov 2026");
}
