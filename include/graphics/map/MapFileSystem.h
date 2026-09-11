#pragma once

#include <stdint.h>
#if defined(SDCARD_SHARE_SPI) && !defined(ARCH_PORTDUINO) && !defined(HAS_SD_MMC)
#include "SD.h"
#endif

/**
 * Abstraction class helper to open, close and read files from different file systems
 */
class IMapFileSystem
{
  public:
    virtual bool open(const char *path) = 0;
    virtual void close(void) = 0;
    virtual bool readAt(uint64_t offset, uint8_t *buf, uint32_t len) = 0;
    virtual ~IMapFileSystem() {}
};

#if (defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC) || defined(SDCARD_SHARE_SPI)) && !defined(SENSECAP_INDICATOR)

#if (defined(SDCARD_SHARE_SPI) || defined(HAS_SD_MMC)) && !defined(ARCH_PORTDUINO)
#include "FS.h"
#include "graphics/common/SdCard.h"
#else
#include "graphics/common/SdCard.h"
#endif

// On SDIO the archive is read through FatFs instead of the VFS: fs::File::seek()
// takes a uint32_t and ends in fseek(long), so exFAT offsets past 2 GiB fail.
#if defined(HAS_SD_MMC) && !defined(ARCH_PORTDUINO)
#define MAPFILE_USE_FATFS 1
#endif

class SDMapFileSystem : public IMapFileSystem
{
  public:
    bool open(const char *path) override;
    void close(void) override;
    bool readAt(uint64_t offset, uint8_t *buf, uint32_t len) override;
#ifdef MAPFILE_USE_FATFS
    ~SDMapFileSystem(void) override { close(); }
#endif

  private:
#ifdef MAPFILE_USE_FATFS
    void *fil = nullptr; // FatFs FIL, kept opaque so ff.h stays out of this header
#else
    File file;
#endif
};

#elif defined(HAS_SDCARD) && !defined(SENSECAP_INDICATOR)

#include "graphics/common/SdCard.h"

class SdFatMapFileSystem : public IMapFileSystem
{
  public:
    bool open(const char *path) override;
    void close(void) override;
    bool readAt(uint64_t offset, uint8_t *buf, uint32_t len) override;

  private:
    FsFile file;
};

#elif defined(SENSECAP_INDICATOR)

class RemoteMapFileSystem : public IMapFileSystem
{
  public:
    RemoteMapFileSystem();
    bool open(const char *path) override;
    void close(void) override;
    bool readAt(uint64_t offset, uint8_t *buf, uint32_t len) override;

  private:
    char path[256];
};

#endif