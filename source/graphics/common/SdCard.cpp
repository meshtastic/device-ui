#include "graphics/common/SdCard.h"
#include "util/ILog.h"
#include "util/ISpiLock.h"

#ifndef SD_SPI_FREQUENCY
#define SD_SPI_FREQUENCY 50000000
#endif

#if defined(HAS_SD_MMC)
fs::SDMMCFS &SDFs = SD_MMC;
#elif defined(ARCH_PORTDUINO)
fs::FS &SDFs = PortduinoFS;
#elif defined(SDCARD_SHARE_SPI)
#if defined(SDCARD_USE_SPI1)
extern SPIClass SPI_HSPI;
static SPIClass &SDHandler = SPI_HSPI; // re-use existing SPI1 instance
#else
static SPIClass &SDHandler = SPI; // re-use existing SPI instance
#endif
fs::SDFS &SDFs = SD;
#elif defined(HAS_SDCARD)
#if defined(SDCARD_USE_SPI1)
static SPIClass SPI1(HSPI);
static SPIClass &SDHandler = SPI1;
#elif defined(SDCARD_USE_SOFT_SPI)
static SoftSpiDriver<SPI_MISO, SPI_MOSI, SPI_SCK> SDHandler;
#else
static SPIClass &SDHandler = SPI;
#endif
SdFs SDFs;
using File = FsFile;
#endif

ISdCard *sdCard = nullptr;

#if defined(ARCH_PORTDUINO)

bool SDCard::init(void)
{
    return true;
}

ISdCard::CardType SDCard::cardType(void)
{
    return CardType::eUnknown;
}

ISdCard::FatType SDCard::fatType(void)
{
    return FatType::eFat32;
}

ISdCard::ErrorType SDCard::errorType(void)
{
    return ErrorType::eNoError;
}

uint64_t SDCard::usedBytes(void)
{
    return 0;
}

uint64_t SDCard::freeBytes(void)
{
    return 0;
}

uint64_t SDCard::cardSize(void)
{
    return 1;
}

SDCard::~SDCard(void) {}

#elif defined(HAS_SD_MMC) || defined(SDCARD_SHARE_SPI) // use SD/SD_MMC interface

#if defined(SDCARD_SHARE_SPI)
bool SDCard::init(void)
{
#ifdef SDCARD_INIT_SPI
    SDHandler.end();
    SDHandler.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    SD.end();
    if (!SD.begin(SDCARD_CS, SDHandler, SD_SPI_FREQUENCY)) {
        ILOG_DEBUG("No SD_MMC card detected");
        return false;
    }
#endif
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        ILOG_DEBUG("No SD_MMC card attached");
        return false;
    }
    ILOG_DEBUG("SD_MMC Card Type: %s", cardType == CARD_MMC    ? "MMC"
                                       : cardType == CARD_SD   ? "SDSC"
                                       : cardType == CARD_SDHC ? "SDHC"
                                                               : "UNKNOWN");

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    ILOG_DEBUG("SD Card Size: %lu MB", (uint32_t)cardSize);
    ILOG_DEBUG("Total space: %lu MB", (uint32_t)(SD.totalBytes() / (1024 * 1024)));
    ILOG_DEBUG("Used space: %lu MB", (uint32_t)(SD.usedBytes() / (1024 * 1024)));

    return true;
}

#else

bool SDCard::init(void)
{
    ISpiLock::Guard bus;
    // #ifndef BOARD_HAS_1BIT_SDMMC
    //     SDFs.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_D0, SDMMC_D1, SDMMC_D2, SDMMC_D3);
    //     return SDFs.begin("/sdcard", false);
    // #else
    SDFs.setPins(SD_SCLK_PIN, SD_MOSI_PIN, SD_MISO_PIN);
    return SDFs.begin("/sdcard", true);
    // #endif
}
#endif

ISdCard::CardType SDCard::cardType(void)
{
    ISpiLock::Guard bus;
    switch (SDFs.cardType()) {
    case CARD_NONE:
        return CardType::eNone;
    case CARD_MMC:
        return CardType::eMMC;
    case CARD_SD:
        return CardType::eSD;
    case CARD_SDHC:
        return CardType::eSDHC;
    case CARD_UNKNOWN:
    default:
        return CardType::eUnknown;
    }
    return CardType::eUnknown;
}

ISdCard::FatType SDCard::fatType(void)
{
    ISpiLock::Guard bus;
    return SDFs.cardSize() > 4Ull * 1024Ull * 1024Ull * 1024Ull ? FatType::eFat32 : FatType::eFat16;
}

ISdCard::ErrorType SDCard::errorType(void)
{
    ISpiLock::Guard bus;
    switch (SDFs.cardType()) {
    case CARD_NONE:
        return ErrorType::eSlotEmpty;
    case CARD_UNKNOWN:
        return ErrorType::eCardError;
    case CARD_MMC:
    case CARD_SD:
    case CARD_SDHC:
        return ErrorType::eNoError;
    default:
        return ErrorType::eUnknownError;
    }
}

uint64_t SDCard::usedBytes(void)
{
    ISpiLock::Guard bus;
    return SDFs.usedBytes();
}

uint64_t SDCard::freeBytes(void)
{
    ISpiLock::Guard bus;
    return SDFs.totalBytes() - SDFs.usedBytes();
}

uint64_t SDCard::cardSize(void)
{
    ISpiLock::Guard bus;
    return SDFs.totalBytes();
}

SDCard::~SDCard(void)
{
    ISpiLock::Guard bus;
    SDFs.end();
}
#endif

#if defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC) || defined(SDCARD_SHARE_SPI)
std::set<std::string> SDCard::loadMapStyles(const char *folder)
{
    ISpiLock::Guard bus;
    std::set<std::string> styles;
    File maps = SDFs.open(folder);
    if (maps) {
        do {
            File style = maps.openNextFile();
            if (!style)
                break;

            std::string path = style.name();
            std::string dir = path.substr(path.find_last_of("/") + 1);
            if (/* style.isDirectory() && */ dir.c_str()[0] != '.') {
                ILOG_DEBUG("SD: found map style: %s", dir.c_str());
                styles.insert(dir);
            }
            style.close();
        } while (true);
        maps.close();
    }
    if (styles.empty()) {
        File map = SDFs.open("/map");
        if (map) {
            ILOG_DEBUG("SD: found /map dir");
            styles.insert("/map");
            map.close();
        } else {
            ILOG_INFO("SD: no maps found");
        }
    }
    updated = true;
    return styles;
}

std::string SDCard::getUrlProvider(const char *folder, const char *style)
{
    ISpiLock::Guard bus;
    String filename = String(folder) + "/" + String(style) + "/.url";
    File file = SDFs.open(filename.c_str(), FILE_READ);
    if (file) {
        String url = file.readStringUntil('\n');
        return std::string{url.c_str()};
    }
    return {};
}

#elif defined(HAS_SDCARD)
bool SdFsCard::init(void)
{
    ISpiLock::Guard bus;
    // TODO: allow specification of SPI bus
    // TODO: use begin(SdioConfig(FIFO_SDIO)) for SDIO (T-HMI)
    // Note: this can also be done via #define BUILTIN_SDCARD SDCARD_CS using begin(SDCARD_CS)
    // see also HAS_SDIO_CLASS
#if HAS_SDIO_CLASS
    return SDFs.begin(SdioConfig(FIFO_SDIO));
#elif defined(SDCARD_USE_SOFT_SPI)
    return SDFs.begin(SdSpiConfig(SDCARD_CS, DEDICATED_SPI, SD_SCK_MHZ(0), &SDHandler));
#else
#if defined(SDCARD_USER_SPI_BEGIN)
    // fix : HSPI Does not have default pins on ESP32S3!
    SDHandler.end();
    SDHandler.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SDCARD_CS);
    return SDFs.begin(SdSpiConfig(SDCARD_CS, SHARED_SPI | USER_SPI_BEGIN, SD_SPI_FREQUENCY, &SDHandler));
#else
    return SDFs.begin(SdSpiConfig(SDCARD_CS, SHARED_SPI, SD_SPI_FREQUENCY, &SDHandler));
#endif
#endif
}

ISdCard::CardType SdFsCard::cardType(void)
{
    ISpiLock::Guard bus;
    uint8_t card = SDFs.card()->type(); // 0 - SD V1, 1 - SD V2, or 3 - SDHC/SDXC
    uint8_t fsType = SDFs.fatType();    // FAT_TYPE_EXFAT, FAT_TYPE_FAT32, FAT_TYPE_FAT16, or zero for error
    if (card == 3)
        return fsType == FAT_TYPE_EXFAT ? CardType::eSDXC : CardType::eSDHC;
    if (fsType != 0)
        return CardType::eSD;
    return CardType::eNone;
}

ISdCard::FatType SdFsCard::fatType(void)
{
    ISpiLock::Guard bus;
    uint8_t type = SDFs.fatType();
    return type == FAT_TYPE_EXFAT   ? FatType::eExFat
           : type == FAT_TYPE_FAT32 ? FatType::eFat32
           : type == FAT_TYPE_FAT16 ? FatType::eFat16
                                    : FatType::eNA;
}

/**
 * @brief Check the error type of the SD card
 * @return ErrorType
 * Note: call only in case of sd.begin() error
 */
ISdCard::ErrorType SdFsCard::errorType(void)
{
    ISpiLock::Guard bus;
    ILOG_ERROR("SD card error code: %d", SDFs.sdErrorCode());
    if (SDFs.sdErrorCode() == SD_CARD_ERROR_CMD0)
        return ErrorType::eSlotEmpty;
    else if (SDFs.sdErrorCode() > SD_CARD_ERROR_CMD0)
        return ErrorType::eCardError;
    else {
        // check mbr type
        MbrSector_t mbr;
        bool valid = true;
        if (SDFs.card()) {
            if (!SDFs.card()->readSector(0, (uint8_t *)&mbr)) {
                // read MBR failed
                return ErrorType::eFormatError;
            }
            MbrPart_t *pt = &mbr.part[0];
            ILOG_WARN("MBR: boot:%02X type:%02X part:%02X", pt->boot, pt->type, pt->beginCHS[0]);
            if (pt->boot != 0 || pt->type == 0 || pt->type == 0xEE) {
                // not mbr
                return ErrorType::eNoMbrError;
            } else if (pt->beginCHS[0] == 0xA || pt->beginCHS[0] == 0x82) {
                // partition looks good
                return ErrorType::eUnknownError;
            }
            return ErrorType::eFormatError;
        }
    }
    return ErrorType::eUnknownError;
}

uint64_t SdFsCard::usedBytes(void)
{
    ISpiLock::Guard bus;
    // Computed here rather than as cardSize() - freeBytes(): those take the guard
    // themselves, and nesting would lean on the host's lock being reentrant. Not
    // nesting is cheaper than depending on every host getting that right.
    uint64_t bytesPerCluster = uint64_t(SDFs.bytesPerCluster());
    return (uint64_t(SDFs.clusterCount()) - uint64_t(SDFs.freeClusterCount())) * bytesPerCluster;
}

uint64_t SdFsCard::freeBytes(void)
{
    ISpiLock::Guard bus;
    return uint64_t(SDFs.freeClusterCount()) * uint64_t(SDFs.bytesPerCluster());
}

uint64_t SdFsCard::cardSize(void)
{
    ISpiLock::Guard bus;
    return uint64_t(SDFs.clusterCount()) * uint64_t(SDFs.bytesPerCluster());
}

bool SdFsCard::format(void)
{
    ISpiLock::Guard bus;
    return SDFs.format();
}

std::set<std::string> SdFsCard::loadMapStyles(const char *folder)
{
    ISpiLock::Guard bus;
    std::set<std::string> styles;
    File maps = SDFs.open(folder);
    if (maps) {
        do {
            File style = maps.openNextFile();
            if (!style)
                break;

            char name[20];
            style.getName(name, sizeof(name));
            std::string path = name;
            std::string dir = path.substr(path.find_last_of("/") + 1);
            if (style.isDirectory() && dir.c_str()[0] != '.') {
                ILOG_DEBUG("SdFs: found map style: %s", dir.c_str());
                styles.insert(dir);
            }
            style.close();
        } while (true);
        maps.close();
    }
    if (styles.empty()) {
        File map = SDFs.open("/map");
        if (map) {
            ILOG_DEBUG("SdFs: found /map dir");
            styles.insert("/map");
            map.close();
        } else {
            ILOG_INFO("SdFs: no maps found");
        }
    }
    updated = true;
    return styles;
}

std::string SdFsCard::getUrlProvider(const char *folder, const char *style)
{
    ISpiLock::Guard bus;
    String filename = String(folder) + "/" + String(style) + "/.url";
    File file = SDFs.open(filename.c_str(), FILE_READ);
    if (file) {
        String url = file.readStringUntil('\n');
        return std::string{url.c_str()};
    }
    return {};
}
#endif // HAS_SDCARD
