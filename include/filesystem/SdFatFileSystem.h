#pragma once

#include "IFileSystem.h"

#if defined(HAS_SDCARD) && !defined(HAS_SD_MMC)
#include "filesystem/SdCard.h"

/**
 * @brief SdFat library implementation
 *
 * Uses the SdFat library's FsFile interface, typically used on
 * platforms with dedicated SD card SPI connections.
 */
class SdFatFileSystem : public IFileSystem
{
  public:
    virtual ~SdFatFileSystem() override = default;

    virtual bool open(const std::string &path, const std::string &mode) override;
    virtual void close() override;
    virtual bool isOpen() const override;
    virtual int printf(const char *format, ...) override;
    virtual int write(const void *data, size_t size) override;
    virtual int read(void *buffer, size_t size) override;
    virtual bool readLine(char *buffer, size_t maxLen) override;
    virtual bool mkdir(const std::string &path) override;
    virtual std::string getLastError() const override;

  private:
    FsFile currentFile;
    std::string lastError;
};

#endif // HAS_SDCARD
