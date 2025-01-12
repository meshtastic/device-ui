#include "util/LinuxHelper.h"
#include <fstream>
#include <limits>
#include <string>

uint32_t LinuxHelper::getAvailableMem(void)
{
    return getMem("MemAvailable:");
}

uint32_t LinuxHelper::getFreeMem(void)
{
    return getMem("MemFree:");
}

uint32_t LinuxHelper::getTotalMem(void)
{
    return getMem("MemTotal:");
}

uint32_t LinuxHelper::getMem(const char *entry)
{
    std::string token;
    std::ifstream file("/proc/meminfo");
    while (file >> token) {
        if (token == entry) {
            unsigned long mem;
            if (file >> mem) {
                return mem;
            } else {
                return 0;
            }
        }
        // Ignore the rest of the line
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return 0;
}