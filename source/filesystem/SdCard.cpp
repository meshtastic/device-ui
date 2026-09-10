#include "filesystem/SdCard.h"
#include "graphics/map/MapTileSettings.h"
#include "util/ILog.h"
#include "util/ISpiLock.h"

#if defined(HAS_SD_MMC) && defined(CONFIG_IDF_TARGET_ESP32P4)
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
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
extern SPIClass SPI_HSPI;
static SPIClass &SDHandler = SPI_HSPI;
#elif defined(SDCARD_USE_SOFT_SPI)
static SoftSpiDriver<SPI_MISO, SPI_MOSI, SPI_SCK> SDHandler;
#else
static SPIClass &SDHandler = SPI;
#endif
SdFs SDFs;
using File = FsFile;
#endif

ISdCard *sdCard = nullptr;

static std::string mapArchivePath(const char *folder, const char *style)
{
    char dir[MapTileSettings::TILE_STYLE_SIZE];
    MapTileSettings::styleToDir(style, dir, sizeof(dir));
    if (!folder || !dir[0])
        return {};
    return std::string(folder) + "/" + dir + "/" + dir + MapTileSettings::PMTILES_EXTENSION;
}

#if defined(HAS_SD_MMC) && defined(CONFIG_IDF_TARGET_ESP32P4) && !defined(SENSECAP_INDICATOR)
static bool ensureDirectory(const std::string &path)
{
    struct stat st = {};
    if (stat(path.c_str(), &st) == 0)
        return S_ISDIR(st.st_mode);
    return mkdir(path.c_str(), 0775) == 0;
}
#endif

// SENSECAP_INDICATOR takes precedence over the generic SD classes, matching
// the declarations in SdCard.h: the card sits behind the co-processor
#if defined(ARCH_PORTDUINO) && !defined(SENSECAP_INDICATOR)

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

#elif defined(HAS_SD_MMC) && !defined(SENSECAP_INDICATOR)

#include "ff.h"

// the card is FatFs drive 0; SDMMCFS itself assumes the same in totalBytes()
#define SDCARD_FATFS_DRIVE "0:"

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
    if (!(s_sdmmc_card->ocr & SD_OCR_SDHC_CAP))
        return CardType::eSD;
    return cardSize() > 32ULL * 1024ULL * 1024ULL * 1024ULL ? CardType::eSDXC : CardType::eSDHC;
}

ISdCard::FatType SDCard::fatType(void)
{
    if (!s_sdmmc_card)
        return FatType::eNA;

    FATFS *fs = nullptr;
    DWORD freeClusters = 0;
    if (f_getfree(SDCARD_FATFS_DRIVE, &freeClusters, &fs) == FR_OK && fs) {
        switch (fs->fs_type) {
        case FS_FAT12:
        case FS_FAT16:
            return FatType::eFat16;
        case FS_FAT32:
            return FatType::eFat32;
        case FS_EXFAT:
            return FatType::eExFat;
        default:
            break;
        }
    }

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
    ISpiLock::Guard bus;
    // #ifndef BOARD_HAS_1BIT_SDMMC
    //     SDFs.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_D0, SDMMC_D1, SDMMC_D2, SDMMC_D3);
    //     return SDFs.begin("/sdcard", false);
    // #else
    SDFs.setPins(SD_SCLK_PIN, SD_MOSI_PIN, SD_MISO_PIN);
    return SDFs.begin("/sdcard", true);
}

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
        // SDMMCFS reports SDHC for anything with the SDHC capability bit, which
        // SDXC sets as well; only the capacity separates them
        return SDFs.cardSize() > 32Ull * 1024Ull * 1024Ull * 1024Ull ? CardType::eSDXC : CardType::eSDHC;
    case CARD_UNKNOWN:
    default:
        return CardType::eUnknown;
    }
    return CardType::eUnknown;
}

ISdCard::FatType SDCard::fatType(void)
{
    ISpiLock::Guard bus;
    FATFS *fs = nullptr;
    DWORD freeClusters = 0;
    if (f_getfree(SDCARD_FATFS_DRIVE, &freeClusters, &fs) != FR_OK || !fs)
        return FatType::eNA;
    switch (fs->fs_type) {
    case FS_FAT12:
    case FS_FAT16:
        return FatType::eFat16;
    case FS_FAT32:
        return FatType::eFat32;
    case FS_EXFAT:
        return FatType::eExFat;
    default:
        return FatType::eNA;
    }
}

ISdCard::ErrorType SDCard::errorType(void)
{
    ISpiLock::Guard bus;
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

#endif // CONFIG_IDF_TARGET_ESP32P4
#endif

#if defined(HAS_SD_MMC) && defined(CONFIG_IDF_TARGET_ESP32P4) && !defined(SENSECAP_INDICATOR)
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
            if (entry->d_name[0] == '.')
                continue;

            std::string path = mountedFolder + "/" + entry->d_name;
            struct stat st = {};
            if (stat(path.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
                continue;

            std::string dirName = entry->d_name;
            if (dirName.size() < MapTileSettings::TILE_STYLE_SIZE) {
                ILOG_DEBUG("SD: found map style: %s", dirName.c_str());
                styles.insert(dirName);
            } else {
                ILOG_WARN("ignored: %s (name too long)", dirName.c_str());
            }
        }
        closedir(dir);
    } else {
        ILOG_DEBUG("SD: %s not found", mountedFolder.c_str());
    }

    if (styles.empty()) {
        struct stat st = {};
        if (stat("/sdcard/map", &st) == 0 && S_ISDIR(st.st_mode)) {
            ILOG_DEBUG("SD: found /map dir");
            styles.insert("/map");
        } else {
            ILOG_INFO("SD: no maps found");
        }
    }

    updated = true;
    return styles;
}

bool SDCard::hasMapArchive(const char *folder, const char *style)
{
    std::string filename = mapArchivePath(folder, style);
    if (filename.empty())
        return false;

    filename = "/sdcard" + filename;
    FILE *file = fopen(filename.c_str(), "rb");
    if (!file)
        return false;
    fclose(file);
    return true;
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
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), file) != nullptr) {
            char *nl = strpbrk(buffer, "\r\n");
            if (nl)
                *nl = '\0';
            fclose(file);
            return std::string{buffer};
        }
        fclose(file);
    }
    return {};
}

bool SDCard::setUrlProvider(const char *folder, const char *style, const char *urlTemplate)
{
    if (!folder || !style || !urlTemplate)
        return false;

    std::string mountedFolder = "/sdcard";
    mountedFolder += folder;
    std::string mountedDir = mountedFolder + "/" + style;
    if (!ensureDirectory(mountedFolder) || !ensureDirectory(mountedDir))
        return false;

    std::string cleanUrl = urlTemplate;
    size_t start = cleanUrl.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        cleanUrl.clear();
    } else {
        size_t end = cleanUrl.find_last_not_of(" \t\r\n");
        cleanUrl = cleanUrl.substr(start, end - start + 1);
    }

    std::string filename = mountedDir + "/.url";
    FILE *file = fopen(filename.c_str(), "w");
    if (!file)
        return false;

    std::string content = cleanUrl + "\n";
    bool written = fwrite(content.c_str(), 1, content.size(), file) == content.size();
    fclose(file);
    return written;
}

// SENSECAP_INDICATOR takes precedence: the SD card sits behind the
// co-processor even when a generic SD define is also set
#elif (defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC)) && !defined(SENSECAP_INDICATOR)
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
            if (style.isDirectory() && dir.c_str()[0] != '.') {
                if (dir.size() < MapTileSettings::TILE_STYLE_SIZE - 1) {
                    ILOG_DEBUG("SD: found map style: %s", dir.c_str());
                    styles.insert(dir);
                } else {
                    ILOG_WARN("ignored: %s (name too long)", dir.c_str());
                }
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

bool SDCard::hasMapArchive(const char *folder, const char *style)
{
    ISpiLock::Guard bus;
    std::string filename = mapArchivePath(folder, style);
    if (filename.empty())
        return false;
    File file = SDFs.open(filename.c_str(), FILE_READ);
    if (!file)
        return false;
    file.close();
    return true;
}

std::string SDCard::getUrlProvider(const char *folder, const char *style)
{
    ISpiLock::Guard bus;
    String filename = String(folder) + "/" + String(style) + "/.url";
    File file = SDFs.open(filename.c_str(), FILE_READ);
    if (file) {
        String url = file.readStringUntil('\n');
        url.trim();
        return std::string{url.c_str()};
    }
    return {};
}

bool SDCard::setUrlProvider(const char *folder, const char *style, const char *urlTemplate)
{
    ISpiLock::Guard bus;
    if (!folder || !style || !urlTemplate)
        return false;
    String dir = String(folder) + "/" + String(style);
    if (!SDFs.exists(folder)) {
        SDFs.mkdir(folder);
    }
    if (!SDFs.exists(dir.c_str())) {
        if (!SDFs.mkdir(dir.c_str()))
            return false;
    }
    String filename = dir + "/.url";
    if (SDFs.exists(filename.c_str())) {
        SDFs.remove(filename.c_str());
    }
    File file = SDFs.open(filename.c_str(), FILE_WRITE);
    if (file) {
        String cleanUrl = String(urlTemplate);
        cleanUrl.trim();
        bool written = file.print(cleanUrl) == cleanUrl.length() && file.print("\n") == 1;
        file.close();
        return written;
    }
    return false;
}

#elif defined(HAS_SDCARD) && !defined(SENSECAP_INDICATOR)
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
    ILOG_DEBUG("SDHandler.begin(%d, %d, %d, %d)", SPI_SCK, SPI_MISO, SPI_MOSI, SDCARD_CS);
    SDHandler.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SDCARD_CS);
    return SDFs.begin(SdSpiConfig(SDCARD_CS, SHARED_SPI | USER_SPI_BEGIN, SD_SPI_FREQUENCY, &SDHandler));
#else
    ILOG_DEBUG("SDFs.begin(%d, %d, %d)", SDCARD_CS, SHARED_SPI, SD_SPI_FREQUENCY);
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

            char name[32];
            style.getName(name, sizeof(name));
            std::string path = name;
            std::string dir = path.substr(path.find_last_of("/") + 1);
            if (style.isDirectory() && dir.c_str()[0] != '.') {
                if (dir.size() < MapTileSettings::TILE_STYLE_SIZE - 1) {
                    ILOG_DEBUG("SdFs: found map style: %s", dir.c_str());
                    styles.insert(dir);
                } else {
                    ILOG_WARN("ignored: %s (name too long)", dir.c_str());
                }
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

bool SdFsCard::hasMapArchive(const char *folder, const char *style)
{
    ISpiLock::Guard bus;
    std::string filename = mapArchivePath(folder, style);
    if (filename.empty())
        return false;
    FsFile file = SDFs.open(filename.c_str(), O_RDONLY);
    if (!file)
        return false;
    file.close();
    return true;
}

std::string SdFsCard::getUrlProvider(const char *folder, const char *style)
{
    ISpiLock::Guard bus;
    String filename = String(folder) + "/" + String(style) + "/.url";
    File file = SDFs.open(filename.c_str(), FILE_READ);
    if (file) {
        String url = file.readStringUntil('\n');
        url.trim();
        return std::string{url.c_str()};
    }
    return {};
}

bool SdFsCard::setUrlProvider(const char *folder, const char *style, const char *urlTemplate)
{
    ISpiLock::Guard bus;
    if (!folder || !style || !urlTemplate)
        return false;
    String dir = String(folder) + "/" + String(style);
    if (!SDFs.exists(folder)) {
        SDFs.mkdir(folder);
    }
    if (!SDFs.exists(dir.c_str())) {
        if (!SDFs.mkdir(dir.c_str()))
            return false;
    }
    String filename = dir + "/.url";
    if (SDFs.exists(filename.c_str())) {
        SDFs.remove(filename.c_str());
    }
    File file = SDFs.open(filename.c_str(), FILE_WRITE);
    if (file) {
        String cleanUrl = String(urlTemplate);
        cleanUrl.trim();
        bool written = file.print(cleanUrl) == cleanUrl.length() && file.print("\n") == 1;
        file.close();
        return written;
    }
    return false;
}

#elif defined(SENSECAP_INDICATOR)

#include "graphics/map/RemoteSDService.h"
#include <cstring>

bool RemoteSdCard::init(void)
{
    IRemoteFS *fs = RemoteSDService::backend();
    if (!fs)
        return false;
    // a mount here is what makes the button revive a card that was ejected
    // (or replaced) since the last look; mounting a mounted card is a no-op
    fs->sdMount();
    if (!fs->sdInfo(info))
        return false;
    return info.present;
}

bool RemoteSdCard::eject(void)
{
    IRemoteFS *fs = RemoteSDService::backend();
    if (!fs || !fs->sdEject())
        return false;
    info = RemoteSdInfo(); // gone until it is mounted again
    return true;
}

bool RemoteSdCard::format(void)
{
    IRemoteFS *fs = RemoteSDService::backend();
    if (!fs)
        return false;
    return fs->sdFormat();
}

ISdCard::StatsResult RemoteSdCard::refreshStats(void)
{
    IRemoteFS *fs = RemoteSDService::backend();
    RemoteSdInfo fresh;
    // a card that is gone (or a link that is down) is not a pending scan:
    // the caller must stop polling and re-detect instead
    if (!fs || !fs->sdInfo(fresh) || !fresh.present)
        return eStatsUnavailable;
    // takes the identity along: the card may have been swapped
    info = fresh;
    return info.statsValid ? eStatsValid : eStatsPending;
}

ISdCard::CardType RemoteSdCard::cardType(void)
{
    // numeric values follow the meshtastic.SdCardInfo protobuf enum
    switch (info.cardType) {
    case 1:
        return eMMC;
    case 2:
        return eSD;
    case 3:
        return eSDHC;
    case 4:
        return eSDXC;
    case 5:
        return eUnknown;
    default:
        return eNone;
    }
}

ISdCard::FatType RemoteSdCard::fatType(void)
{
    switch (info.fatType) {
    case 1:
        return eFat16;
    case 2:
        return eFat32;
    case 3:
        return eExFat;
    default:
        return eNA;
    }
}

std::set<std::string> RemoteSdCard::loadMapStyles(const char *folder)
{
    std::set<std::string> styles;
    IRemoteFS *fs = RemoteSDService::backend();
    if (fs) {
        std::set<std::string> entries;
        if (fs->listDir(folder, entries)) {
            for (auto &entry : entries) {
                if (entry.empty() || entry[0] == '.')
                    continue;
                // only subdirectories (trailing slash) are map styles; plain
                // files in the maps folder must not show up in the selection
                if (entry.back() != '/')
                    continue;
                std::string dir = entry.substr(0, entry.size() - 1);
                if (dir.size() < MapTileSettings::TILE_STYLE_SIZE) {
                    ILOG_DEBUG("remote SD: found map style: %s", dir.c_str());
                    styles.insert(dir);
                } else {
                    ILOG_WARN("ignored: %s (name too long)", dir.c_str());
                }
            }
        }
        if (styles.empty()) {
            std::set<std::string> mapDir;
            if (fs->listDir("/map", mapDir)) {
                ILOG_DEBUG("remote SD: found /map dir");
                styles.insert("/map");
            } else {
                ILOG_INFO("remote SD: no maps found");
            }
        }
    }
    updated = true;
    return styles;
}

bool RemoteSdCard::hasMapArchive(const char *folder, const char *style)
{
    IRemoteFS *fs = RemoteSDService::backend();
    std::string filename = mapArchivePath(folder, style);
    if (!fs || filename.empty())
        return false;
    uint8_t byte = 0;
    uint32_t bytesRead = 0, fileSize = 0;
    return fs->readChunk(filename.c_str(), 0, &byte, 1, &bytesRead, &fileSize) && bytesRead == 1 && fileSize > 0;
}

std::string RemoteSdCard::getUrlProvider(const char *folder, const char *style)
{
    IRemoteFS *fs = RemoteSDService::backend();
    if (!fs)
        return {};
    std::string filename = std::string(folder) + "/" + style + "/.url";
    // read until the first line is complete instead of trusting a single
    // chunk, so a long URL template does not get silently truncated
    uint8_t buf[1024];
    uint32_t total = 0, fileSize = 0;
    while (total < sizeof(buf) - 1) {
        uint32_t bytesRead = 0;
        if (!fs->readChunk(filename.c_str(), total, buf + total, sizeof(buf) - 1 - total, &bytesRead, &fileSize) ||
            bytesRead == 0)
            break;
        if (memchr(buf + total, '\n', bytesRead)) {
            total += bytesRead;
            break;
        }
        total += bytesRead;
        if (fileSize > 0 && total >= fileSize)
            break;
    }
    if (total == 0)
        return {};
    buf[total] = '\0';
    // first line only
    char *nl = strpbrk((char *)buf, "\r\n");
    if (nl)
        *nl = '\0';
    return std::string{(char *)buf};
}

bool RemoteSdCard::setUrlProvider(const char *folder, const char *style, const char *urlTemplate)
{
    IRemoteFS *fs = RemoteSDService::backend();
    if (!fs || !folder || !style || !urlTemplate)
        return false;
    std::string filename = std::string(folder) + "/" + style + "/.url";
    std::string cleanUrl = urlTemplate;
    while (!cleanUrl.empty() &&
           (cleanUrl.back() == '\r' || cleanUrl.back() == '\n' || cleanUrl.back() == ' ' || cleanUrl.back() == '\t')) {
        cleanUrl.pop_back();
    }
    if (cleanUrl.size() > 1023)
        return false;

    std::string content = cleanUrl + "\n";
    return fs->writeChunk(filename.c_str(), 0, (const uint8_t *)content.c_str(), content.size(), true);
}

#endif // HAS_SDCARD
