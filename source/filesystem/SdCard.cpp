#include "filesystem/SdCard.h"
#include "util/ILog.h"

#if defined(HAS_SD_MMC) && defined(CONFIG_IDF_TARGET_ESP32P4)
#include <cstring>
#include <dirent.h>
#endif

#ifndef SD_SPI_FREQUENCY
#define SD_SPI_FREQUENCY 50000000
#endif

#if defined(HAS_SD_MMC)
fs::SDMMCFS &SDFs = SD_MMC;
#elif defined(ARCH_PORTDUINO)
fs::FS &SDFs = PortduinoFS;
#elif defined(HAS_SDCARD)
#ifdef SDCARD_USE_SPI1
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

#elif defined(HAS_SD_MMC)

#if defined(CONFIG_IDF_TARGET_ESP32P4)
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "sd_protocol_defs.h"
#include "sdmmc_cmd.h"

#ifndef BOARD_MAX_SDMMC_FREQ
#define BOARD_MAX_SDMMC_FREQ 40000
#endif

static sdmmc_card_t *s_sdmmc_card = nullptr;

bool SDCard::init(void)
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.slot = SD_MMC_HOST_SLOT;
    host.flags = SDMMC_HOST_FLAG_1BIT;
    host.max_freq_khz = BOARD_MAX_SDMMC_FREQ;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.clk = (gpio_num_t)SD_SCLK_PIN;
    slot_config.cmd = (gpio_num_t)SD_MOSI_PIN;
    slot_config.d0 = (gpio_num_t)SD_MISO_PIN;
    slot_config.width = 1;
    // In 1-bit mode, force unused data lines to NC so slot defaults don't claim unrelated GPIOs.
    slot_config.d1 = GPIO_NUM_NC;
    slot_config.d2 = GPIO_NUM_NC;
    slot_config.d3 = GPIO_NUM_NC;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ILOG_DEBUG("SDCard (P4): slot=%d clk=%d cmd=%d d0=%d freq_khz=%d", host.slot, (int)slot_config.clk, (int)slot_config.cmd,
               (int)slot_config.d0, (int)host.max_freq_khz);

    s_sdmmc_card = nullptr;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &s_sdmmc_card);
    if (ret != ESP_OK) {
        ILOG_ERROR("SDCard (P4): mount failed: 0x%x %s", (int)ret, esp_err_to_name(ret));
        s_sdmmc_card = nullptr;
        return false;
    }
    return true;
}

ISdCard::CardType SDCard::cardType(void)
{
    if (!s_sdmmc_card)
        return CardType::eNone;
    if (s_sdmmc_card->is_mmc)
        return CardType::eMMC;
    return (s_sdmmc_card->ocr & SD_OCR_SDHC_CAP) ? CardType::eSDHC : CardType::eSD;
}

ISdCard::FatType SDCard::fatType(void)
{
    if (!s_sdmmc_card)
        return FatType::eNA;
    uint64_t sz = cardSize();
    return sz > 32ULL * 1024 * 1024 * 1024 ? FatType::eExFat : sz > 4ULL * 1024 * 1024 * 1024 ? FatType::eFat32 : FatType::eFat16;
}

ISdCard::ErrorType SDCard::errorType(void)
{
    return s_sdmmc_card ? ErrorType::eNoError : ErrorType::eSlotEmpty;
}

uint64_t SDCard::usedBytes(void)
{
    return SDFs.usedBytes(); // uses f_getfree() internally, works without _card
}

uint64_t SDCard::freeBytes(void)
{
    return SDFs.totalBytes() - SDFs.usedBytes();
}

uint64_t SDCard::cardSize(void)
{
    if (!s_sdmmc_card)
        return 0;
    return (uint64_t)s_sdmmc_card->csd.capacity * s_sdmmc_card->csd.sector_size;
}

SDCard::~SDCard(void)
{
    if (s_sdmmc_card) {
        esp_vfs_fat_sdcard_unmount("/sdcard", s_sdmmc_card);
        s_sdmmc_card = nullptr;
    }
}

#else // non-P4 HAS_SD_MMC

bool SDCard::init(void)
{
    SDFs.setPins(SD_SCLK_PIN, SD_MOSI_PIN, SD_MISO_PIN);
    return SDFs.begin("/sdcard", true);
}

ISdCard::CardType SDCard::cardType(void)
{
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
    return SDFs.cardSize() > 4ULL * 1024ULL * 1024ULL * 1024ULL ? FatType::eFat32 : FatType::eFat16;
}

ISdCard::ErrorType SDCard::errorType(void)
{
    switch (SDFs.cardType()) {
    case CARD_NONE:
        return ErrorType::eSlotEmpty;
    case CARD_UNKNOWN:
        return ErrorType::eCardError;
    default:
        return ErrorType::eUnknownError;
    }
}

uint64_t SDCard::usedBytes(void)
{
    return SDFs.usedBytes();
}

uint64_t SDCard::freeBytes(void)
{
    return SDFs.totalBytes() - SDFs.usedBytes();
}

uint64_t SDCard::cardSize(void)
{
    return SDFs.totalBytes();
}

SDCard::~SDCard(void)
{
    SDFs.end();
}

#endif // CONFIG_IDF_TARGET_ESP32P4
#endif

#if defined(HAS_SD_MMC) && defined(CONFIG_IDF_TARGET_ESP32P4)
// P4 implementation: use POSIX directory and file functions via VFS at /sdcard

std::set<std::string> SDCard::loadMapStyles(const char *folder)
{
    ILOG_DEBUG("SDCard::loadMapStyles %s", folder);
    std::set<std::string> styles;

    std::string mountedFolder = "/sdcard";
    mountedFolder += folder;

    DIR *dir = opendir(mountedFolder.c_str());
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_name[0] != '.') {
                ILOG_DEBUG("SD: found map style: %s", entry->d_name);
                styles.insert(entry->d_name);
            }
        }
        closedir(dir);
    } else {
        ILOG_DEBUG("SD: %s not found", mountedFolder.c_str());
    }

    if (styles.empty()) {
        dir = opendir("/sdcard/map");
        if (dir) {
            ILOG_DEBUG("SD: found /map dir");
            styles.insert("/map");
            closedir(dir);
        } else {
            ILOG_INFO("SD: no maps found");
        }
    }

    updated = true;
    return styles;
}

std::string SDCard::getUrlProvider(const char *folder, const char *style)
{
    std::string filename = "/sdcard";
    filename += folder;
    filename += "/";
    filename += style;
    filename += "/.url";

    FILE *file = fopen(filename.c_str(), "r");
    if (file) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), file) != nullptr) {
            // Remove trailing newline
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }
            fclose(file);
            return std::string{buffer};
        }
        fclose(file);
    }
    return {};
}

#elif defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC)
// Non-P4 implementation: use Arduino File API

std::set<std::string> SDCard::loadMapStyles(const char *folder)
{
    ILOG_DEBUG("SDCard::loadMapStyles %s", folder);
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
    } else {
        ILOG_DEBUG("SD: /maps not found");
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
    String filename = String(folder) + "/" + String(style) + "/.url";
    File file = SDFs.open(filename.c_str(), FILE_READ);
    if (file) {
        String url = file.readStringUntil('\n');
        return std::string{url.c_str()};
    }
    return {};
}

#elif defined(HAS_SDCARD) // not ARCH_PORTDUINO and not MMC
bool SdFsCard::init(void)
{
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
    SDHandler.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SDCARD_CS);
    return SDFs.begin(SdSpiConfig(SDCARD_CS, SHARED_SPI | USER_SPI_BEGIN, SD_SPI_FREQUENCY, &SDHandler));
#else
    return SDFs.begin(SdSpiConfig(SDCARD_CS, SHARED_SPI, SD_SPI_FREQUENCY, &SDHandler));
#endif
#endif
}

ISdCard::CardType SdFsCard::cardType(void)
{
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
    ILOG_ERROR("SD card error code: %d", SDFs.sdErrorCode());
    if (SDFs.sdErrorCode() == SD_CARD_ERROR_CMD0)
        return ErrorType::eSlotEmpty;
    else if (SDFs.sdErrorCode() > SD_CARD_ERROR_CMD0)
        return ErrorType::eCardError;
    else {
        // check mbr type
        MbrSector_t mbr;
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
    return cardSize() - freeBytes();
}

uint64_t SdFsCard::freeBytes(void)
{
    return uint64_t(SDFs.freeClusterCount()) * uint64_t(SDFs.bytesPerCluster());
}

uint64_t SdFsCard::cardSize(void)
{
    return uint64_t(SDFs.clusterCount()) * uint64_t(SDFs.bytesPerCluster());
}

bool SdFsCard::format(void)
{
    return SDFs.format();
}

std::set<std::string> SdFsCard::loadMapStyles(const char *folder)
{
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
    String filename = String(folder) + "/" + String(style) + "/.url";
    File file = SDFs.open(filename.c_str(), FILE_READ);
    if (file) {
        String url = file.readStringUntil('\n');
        return std::string{url.c_str()};
    }
    return {};
}
#endif // HAS_SDCARD
