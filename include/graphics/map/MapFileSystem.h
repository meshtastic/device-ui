#pragma once

#include <stdint.h>

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

#if defined(SDCARD_SHARE_SPI) && !defined(ARCH_PORTDUINO) && !defined(HAS_SD_MMC)
#include "SD.h"
#else
#include "graphics/common/SdCard.h"
#endif

class SDMapFileSystem : public IMapFileSystem
{
  public:
    bool open(const char *path) override;
    void close(void) override;
    bool readAt(uint64_t offset, uint8_t *buf, uint32_t len) override;

  private:
    File file;
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