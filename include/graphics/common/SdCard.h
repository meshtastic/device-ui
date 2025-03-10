#pragma once

#if defined(ARCH_PORTDUINO)
#include "PortduinoFS.h"
extern fs::FS &SDFs;

#elif defined(HAS_SD_MMC)
#include "SD_MMC.h" // TODO: replace by SdFat SDIO
extern fs::SDMMCFS &SD;

#elif defined(HAS_SDCARD)
#include "SdFat.h"
extern SdFs SDFs;
#endif

#include <string>
#include <set>

/**
 * SdCard adapter to define a common interface for SD and SdFs
 */
class ISdCard
{
  public:
    enum CardType {
      eNone,
      eMMC,
      eSD,
      eSDHC,
      eSDXC,
      eSDUC,
      eUnknown,
    };

    enum FatType {
      eNA,
      eFat16,
      eFat32,
      eExFat,
    };

    virtual bool init(void) = 0;
    virtual CardType cardType(void) = 0;
    virtual FatType fatType(void) = 0;
    virtual uint64_t usedBytes(void) = 0;
    virtual uint64_t freeBytes(void) = 0;
    virtual uint64_t cardSize(void) = 0;

    bool isUpdated(void) { return updated; }
    virtual std::set<std::string> loadMapStyles(const char* folder) = 0;
    virtual ~ISdCard(void) {}

    protected:
    bool updated = false;
};

#if defined(ARCH_PORTDUINO)
class SDCard : public ISdCard
{
  public:
    bool init(void) override;
    CardType cardType(void) override;
    FatType fatType(void) override;
    uint64_t usedBytes(void) override;
    uint64_t freeBytes(void) override;
    uint64_t cardSize(void) override;
    
    std::set<std::string> loadMapStyles(const char* folder) override;
    virtual ~SDCard(void) {}
};

#elif defined(HAS_SDCARD) || defined(HAS_SD_MMC)
class SdFsCard : public ISdCard
{
  public:
    bool init(void) override;
    CardType cardType(void) override;
    FatType fatType(void) override;
    uint64_t usedBytes(void) override;
    uint64_t freeBytes(void) override;
    uint64_t cardSize(void) override;
    
    std::set<std::string> loadMapStyles(const char* folder) override;
    virtual ~SdFsCard(void) {}
};

#endif

/**
 * Dummy to emulate sdCard interface without card reader or without card inserted
 */
class NoSdCard : public ISdCard
{
  public:
    bool init(void) override { return true; }
    CardType cardType(void) override { return CardType::eNone; }
    FatType fatType(void) override { return FatType::eNA; }
    uint64_t usedBytes(void) override { return 0; }
    uint64_t freeBytes(void) override { return 0; }
    uint64_t cardSize(void) override { return 1; }

    std::set<std::string> loadMapStyles(const char* folder) override { 
      updated = true; return std::set<std::string>{}; 
    }
    virtual ~NoSdCard(void) {}
  };


extern ISdCard *sdCard;