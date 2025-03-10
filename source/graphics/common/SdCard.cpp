#include "graphics/common/SdCard.h"
#include "util/ILog.h"

#ifndef SD_SPI_FREQUENCY
#define SD_SPI_FREQUENCY 50000000
#endif

#if defined(HAS_SD_MMC)
SPIClass SDHandler; //TODO allow specification of spi bus
fs::SDMMCFS &SDFs = SD_MMC;
#elif defined(ARCH_PORTDUINO)
fs::FS &SDFs = PortduinoFS;
#elif defined(HAS_SDCARD)
SPIClass &SDHandler = SPI;
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


std::set<std::string> SDCard::loadMapStyles(const char* folder)
{
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

#elif defined(HAS_SDCARD) || defined(HAS_SD_MMC)

bool SdFsCard::init(void)
{
    // TODO: allow specification of SPI bus
    // TODO: use begin(SdioConfig(FIFO_SDIO)) for SDIO (T-HMI)
    // Note: this can also be done via #define BUILTIN_SDCARD SDCARD_CS using begin(SDCARD_CS)
    // see also HAS_SDIO_CLASS
    return SDFs.begin(SdSpiConfig(SDCARD_CS, SHARED_SPI, SD_SPI_FREQUENCY, &SDHandler));
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
    return type == FAT_TYPE_EXFAT ? FatType::eExFat 
         : type == FAT_TYPE_FAT32 ? FatType::eFat32 
         : type == FAT_TYPE_FAT16 ? FatType::eFat16
         : FatType::eNA;
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


std::set<std::string> SdFsCard::loadMapStyles(const char* folder)
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

#endif // HAS_SDCARD