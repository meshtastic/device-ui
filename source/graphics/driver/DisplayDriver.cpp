#include "graphics/driver/DisplayDriver.h"
#include "util/ILog.h"

#if LV_USE_PROFILER
#if defined(ARCH_PORTDUINO)
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#endif
#include "src/misc/lv_profiler_builtin_private.h"
#endif

DisplayDriver::DisplayDriver(uint16_t width, uint16_t height)
    : lvgl(width, height), display(nullptr), touch(nullptr), view(nullptr), screenWidth(width), screenHeight(height)
{
}

void DisplayDriver::init(DeviceGUI *gui)
{
    ILOG_DEBUG("DisplayDriver init...");
    view = gui;
    lvgl.init();

#if LV_USE_PROFILER
    // initialize lvgl profiler
    lv_profiler_builtin_config_t config;
    lv_profiler_builtin_config_init(&config);
#ifdef ARCH_PORTDUINO
    config.tick_per_sec = 1000000000;
    config.tick_get_cb = []() -> uint64_t {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * 1000000000 + ts.tv_nsec;
    };
    config.tid_get_cb = []() -> int { return (int)syscall(SYS_gettid); };
    config.cpu_get_cb = []() -> int {
        int cpu_id = 0;
        syscall(SYS_getcpu, &cpu_id, NULL);
        return cpu_id;
    };
    config.flush_cb = [](const char *buf) { Serial.print(buf); };
#else // arduino
    config.tick_per_sec = 1000000;
    config.tick_get_cb = []() -> uint64_t { return micros(); };
    config.flush_cb = [](const char *buf) { Serial.println(buf); };
#endif
    lv_profiler_builtin_init(&config);
#endif
}
