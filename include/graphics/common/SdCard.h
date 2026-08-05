#pragma once

#if defined(ARCH_PORTDUINO)
#include "PortduinoFS.h"
extern fs::FS &SDFs;

#elif defined(HAS_SD_MMC)
#include "SD_MMC.h" // TODO: replace by SdFat SDIO
extern fs::SDMMCFS &SDFs;

#elif defined(HAS_SDCARD)
#include "SdFat.h"
extern SdFs SDFs;
#endif

#include <set>
#include <string>

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

    enum ErrorType { eNoError, eSlotEmpty, eCardError, eFormatError, eNoMbrError, eUnknownError };

    virtual bool init(void) = 0;
    virtual CardType cardType(void) = 0;
    virtual FatType fatType(void) = 0;
    virtual ErrorType errorType(void) = 0;

    virtual uint64_t usedBytes(void) = 0;
    virtual uint64_t freeBytes(void) = 0;
    // false while used/free are not known yet (e.g. a background scan on a
    // co-processor has not finished); local cards always know them
    virtual bool statsValid(void) { return true; }

    enum StatsResult {
        eStatsValid,       // used/free are up to date
        eStatsPending,     // still being computed, ask again later
        eStatsUnavailable, // card or link gone, stop asking
    };
    // Re-read used/free without re-detecting the card
    virtual StatsResult refreshStats(void) { return eStatsValid; }
    // Release the card so it can be pulled safely. False when the card cannot
    // be ejected (locally mounted cards are simply not held open).
    virtual bool eject(void) { return false; }
    virtual uint64_t cardSize(void) = 0;
    virtual bool format(void) = 0;

    bool isUpdated(void) { return updated; }
    virtual std::set<std::string> loadMapStyles(const char *folder) = 0;
    virtual std::string getUrlProvider(const char *folder, const char *style) = 0;
    virtual ~ISdCard(void) {}

  protected:
    bool updated = false;
};

// SENSECAP_INDICATOR takes precedence: the SD card sits behind the
// co-processor even when a generic SD define is also set
#if (defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC)) && !defined(SENSECAP_INDICATOR)
class SDCard : public ISdCard
{
  public:
    bool init(void) override;
    CardType cardType(void) override;
    FatType fatType(void) override;
    ErrorType errorType(void) override;
    uint64_t usedBytes(void) override;
    uint64_t freeBytes(void) override;
    uint64_t cardSize(void) override;
    bool format(void) override { return false; };

    std::set<std::string> loadMapStyles(const char *folder) override;
    std::string getUrlProvider(const char *folder, const char *style) override;
    virtual ~SDCard(void);
};

#elif defined(HAS_SDCARD) && !defined(SENSECAP_INDICATOR)
class SdFsCard : public ISdCard
{
  public:
    bool init(void) override;
    CardType cardType(void) override;
    FatType fatType(void) override;
    ErrorType errorType(void) override;
    uint64_t usedBytes(void) override;
    uint64_t freeBytes(void) override;
    uint64_t cardSize(void) override;
    bool format(void) override;

    std::set<std::string> loadMapStyles(const char *folder) override;
    std::string getUrlProvider(const char *folder, const char *style) override;
    virtual ~SdFsCard(void) {}
};

#elif defined(SENSECAP_INDICATOR)
#include "graphics/map/RemoteSDService.h"

/**
 * SD card behind a co-processor, accessed through the RemoteSDService
 * IRemoteFS backend. Statistics are fetched once over the link on init().
 */
class RemoteSdCard : public ISdCard
{
  public:
    bool init(void) override;
    CardType cardType(void) override;
    FatType fatType(void) override;
    ErrorType errorType(void) override
    {
        if (info.present)
            return eNoError;
        // a card that is in there but has no filesystem can be formatted,
        // just like a local one
        return info.unformatted ? eFormatError : eSlotEmpty;
    }
    uint64_t usedBytes(void) override { return info.usedBytes; }
    uint64_t freeBytes(void) override { return info.freeBytes; }
    bool statsValid(void) override { return info.statsValid; }
    StatsResult refreshStats(void) override;
    bool eject(void) override;
    uint64_t cardSize(void) override { return info.cardSize; }
    bool format(void) override;

    std::set<std::string> loadMapStyles(const char *folder) override;
    std::string getUrlProvider(const char *folder, const char *style) override;
    virtual ~RemoteSdCard(void) {}

  private:
    RemoteSdInfo info;
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
    ErrorType errorType(void) override { return ErrorType::eNoError; }
    uint64_t usedBytes(void) override { return 0; }
    uint64_t freeBytes(void) override { return 0; }
    uint64_t cardSize(void) override { return 1; }
    bool format(void) override { return false; }

    std::set<std::string> loadMapStyles(const char *folder) override
    {
        updated = true;
        return std::set<std::string>{};
    }
    std::string getUrlProvider(const char *folder, const char *style) override { return {}; }
    virtual ~NoSdCard(void) {}
};

extern ISdCard *sdCard;