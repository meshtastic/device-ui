#pragma once

#include "storage/IStorage.h"
#include <string>

/**
 * Storage in a host folder (Portduino: ~/.portduino/default), accessed with POSIX calls.
 */
class PosixStorage : public IStorage
{
  public:
    explicit PosixStorage(const char *root);

    bool hasSlot(void) const override { return false; }
    Status mount(void) override;
    bool info(StorageInfo &out) override;

    std::unique_ptr<IFile> open(const char *path, OpenMode mode) override;
    bool exists(const char *path) override;
    bool mkdir(const char *path) override;
    bool remove(const char *path) override;
    bool list(const char *dir, const std::function<void(const char *name, bool isDir)> &cb) override;

  private:
    std::string full(const char *path) const;

    std::string root;
};
