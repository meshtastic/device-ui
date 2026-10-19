#pragma once

#include "IFileSystem.h"
#include <cstdio>

/**
 * @brief POSIX file system implementation for ESP32-P4
 *
 * Uses POSIX file APIs (fopen, fprintf, fgets, etc.) which work with
 * ESP-IDF's VFS-mounted file systems at /sdcard.
 */
class PosixFileSystem : public IFileSystem
{
  public:
    virtual ~PosixFileSystem() override;

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
    FILE *currentFile = nullptr;
    std::string lastError;
};
