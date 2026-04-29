#include "Arduino.h"
#include "filesystem/IFileSystem.h"
#include <memory>

#if defined(HAS_SD_MMC) && defined(CONFIG_IDF_TARGET_ESP32P4)
#include "filesystem/PosixFileSystem.h"
#elif defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC)
#include "filesystem/ArduinoFileSystem.h"
#elif defined(HAS_SDCARD)
#include "filesystem/SdFatFileSystem.h"
#endif

/**
 * @brief Factory function to create appropriate file system implementation
 * @return Unique pointer to platform-specific IFileSystem instance
 */
std::unique_ptr<IFileSystem> createFileSystem()
{
#if defined(HAS_SD_MMC) && defined(CONFIG_IDF_TARGET_ESP32P4)
    return std::make_unique<PosixFileSystem>();
#elif defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC)
    return std::unique_ptr<ArduinoFileSystem>(new ArduinoFileSystem());
#elif defined(HAS_SDCARD)
    return std::make_unique<SdFatFileSystem>();
#else
    return nullptr;
#endif
}
