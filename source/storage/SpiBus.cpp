#if defined(ARDUINO_ARCH_ESP32) && defined(HAS_SDCARD) && !defined(HAS_SD_MMC) && !defined(SDCARD_USE_SOFT_SPI)

#include "SD.h"
#include "SPI.h"
#include "diskio.h"
#include "ff.h"
#include "sd_diskio.h"
#include "storage/FatFsStorage.h"
#include "util/ILog.h"

#ifndef SD_SPI_FREQUENCY
#define SD_SPI_FREQUENCY 50000000
#endif

#ifdef SDCARD_USE_SPI1
extern SPIClass SPI_HSPI;
static SPIClass &SDHandler = SPI_HSPI;
#else
static SPIClass &SDHandler = SPI;
#endif

namespace
{
// the firmware mounts the card with SD.h at the same place, so both share one mount
constexpr const char *VFS_PATH = "/sd";
constexpr uint8_t MAX_FILES = 5;

// SDFS keeps its FatFs drive number protected
struct SdDrive : fs::SDFS {
    using fs::SDFS::_pdrv;
};

/**
 * Card on a hardware SPI bus, driven by the Arduino SD library (FatFs underneath),
 * whose SPI transactions coexist with the radio on a shared bus.
 */
class SpiBus : public ISdBus
{
  public:
    IStorage::Status mount(bool formatIfNeeded) override
    {
#ifdef SDCARD_INIT_SPI
        SDHandler.end();
#endif
#if defined(SPI_SCK) && defined(SPI_MISO) && defined(SPI_MOSI)
        SDHandler.begin(SPI_SCK, SPI_MISO, SPI_MOSI); // no-op when the bus is already running
#endif
        SD.end();
        if (SD.begin(SDCARD_CS, SDHandler, SD_SPI_FREQUENCY, VFS_PATH, MAX_FILES, formatIfNeeded))
            return IStorage::Status::Ok;

        // SD.begin() fails alike for an empty slot and a card without filesystem
        uint8_t pdrv = sdcard_init(SDCARD_CS, &SDHandler, SD_SPI_FREQUENCY);
        if (pdrv == 0xFF)
            return IStorage::Status::CardError;
        bool present = !(disk_initialize(pdrv) & STA_NOINIT);
        sdcard_uninit(pdrv);
        return present ? IStorage::Status::NoFilesystem : IStorage::Status::SlotEmpty;
    }

    void unmount(void) override { SD.end(); }

    uint8_t drive(void) const override { return SD.*(&SdDrive::_pdrv); }

    StorageInfo::CardType cardType(void) override
    {
        switch (SD.cardType()) {
        case CARD_NONE:
            return StorageInfo::eNone;
        case CARD_MMC:
            return StorageInfo::eMMC;
        case CARD_SD:
            return StorageInfo::eSD;
        case CARD_SDHC:
            // SDXC carries the SDHC capability bit as well; only the size separates them
            return SD.cardSize() > 32ULL * 1024 * 1024 * 1024 ? StorageInfo::eSDXC : StorageInfo::eSDHC;
        default:
            return StorageInfo::eUnknown;
        }
    }

    fs::FS *arduinoFS(void) override { return &SD; }
};
} // namespace

std::unique_ptr<ISdBus> createSpiBus(void)
{
    return std::unique_ptr<ISdBus>(new SpiBus);
}

#endif
