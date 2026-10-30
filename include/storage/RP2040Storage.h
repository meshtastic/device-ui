#pragma once

#include "storage/IStorage.h"

/** Uses the arduino-pico core's bundled SdFat for RP2040/RP2350 SD cards. */
class RP2040Storage : public IStorage
{
  public:
    RP2040Storage();
    ~RP2040Storage() override;

    Status mount(void) override;
    bool eject(void) override;
    bool format(void) override;
    bool info(StorageInfo &out) override;

    std::unique_ptr<IFile> open(const char *path, OpenMode mode) override;
    bool exists(const char *path) override;
    bool mkdir(const char *path) override;
    bool remove(const char *path) override;
    bool list(const char *dir, const std::function<void(const char *name, bool isDir)> &cb) override;

  private:
    friend class RP2040StorageFile;
    struct State;
    std::unique_ptr<State> state;
};

std::unique_ptr<IStorage> createRP2040Storage(void);
