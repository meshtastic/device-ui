#pragma once

#include "storage/IRemoteFS.h"
#include "storage/IStorage.h"

/**
 * SD card behind a co-processor, accessed chunk-wise through an IRemoteFS backend.
 * Offsets are limited to 32 bit by the interdevice protocol.
 */
class RemoteStorage : public IStorage
{
  public:
    static void setBackend(IRemoteFS *fs);
    static IRemoteFS *backend(void);

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
    static IRemoteFS *remoteFS;
};
