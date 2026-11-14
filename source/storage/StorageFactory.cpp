#include "storage/IStorage.h"

#if defined(SENSECAP_INDICATOR)
#include "storage/RemoteStorage.h"
#elif defined(ARCH_PORTDUINO)
#include "PortduinoFS.h"
#include "storage/PosixStorage.h"
#elif defined(ARCH_RP2040) && defined(HAS_SDCARD)
#include "storage/RP2040Storage.h"
#elif defined(ARDUINO_ARCH_ESP32) && (defined(HAS_SD_MMC) || defined(HAS_SDCARD))
#include "storage/FatFsStorage.h"
#endif

std::unique_ptr<IStorage> createStorage(void)
{
#if defined(SENSECAP_INDICATOR)
    // the card sits behind the co-processor even when a local SD define is set
    return std::unique_ptr<IStorage>(new RemoteStorage);
#elif defined(ARCH_PORTDUINO)
    return std::unique_ptr<IStorage>(new PosixStorage(portduinoVFS->mountpoint()));
#elif defined(ARCH_RP2040) && defined(HAS_SDCARD)
    return createRP2040Storage();
#elif defined(ARDUINO_ARCH_ESP32) && defined(HAS_SD_MMC)
    return std::unique_ptr<IStorage>(new FatFsStorage(createSdmmcBus()));
#elif defined(ARDUINO_ARCH_ESP32) && defined(HAS_SDCARD) && defined(SDCARD_USE_SOFT_SPI)
    return std::unique_ptr<IStorage>(new FatFsStorage(createSoftSpiBus()));
#elif defined(ARDUINO_ARCH_ESP32) && defined(HAS_SDCARD)
    return std::unique_ptr<IStorage>(new FatFsStorage(createSpiBus()));
#else
    return std::unique_ptr<IStorage>(new NullStorage);
#endif
}
