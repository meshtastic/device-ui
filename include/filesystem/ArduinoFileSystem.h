#pragma once

#include "IFileSystem.h"

#if defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC)
#include "filesystem/SdCard.h"

/**
 * @brief Arduino File API implementation
 *
 * Uses Arduino's File API (via SDFs) which provides a common interface
 * for ESP32, nRF52, and Portduino platforms.
 */
class ArduinoFileSystem : public IFileSystem
{
  public:
    virtual ~ArduinoFileSystem() override = default;

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
    File currentFile;
    std::string lastError;
};

#endif // ARCH_PORTDUINO || HAS_SD_MMC
