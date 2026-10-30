#pragma once

#include "storage/IStorage.h"
#include <memory>
#include <mutex>
#include <string>
#include <vector>

/**
 * How a card is attached (SDMMC, SPI, bit-banged SPI). A bus brings the card up,
 * mounts its FAT volume into FatFs and VFS, and tears it down again.
 */
class ISdBus
{
  public:
    virtual ~ISdBus() = default;
    // formatIfNeeded puts a fresh filesystem on a card that has none
    virtual IStorage::Status mount(bool formatIfNeeded) = 0;
    virtual void unmount(void) = 0;
    // FatFs physical drive of the mounted volume
    virtual uint8_t drive(void) const = 0;
    virtual StorageInfo::CardType cardType(void) = 0;
    virtual fs::FS *arduinoFS(void) = 0;
};

// one per card interface, defined only on boards that have it
std::unique_ptr<ISdBus> createSdmmcBus(void);
std::unique_ptr<ISdBus> createSpiBus(void);
std::unique_ptr<ISdBus> createSoftSpiBus(void);

class FatFsFile;

/**
 * SD card on ESP32, accessed through the FatFs API rather than VFS: VFS offsets are a
 * 32-bit off_t, FatFs with exFAT seeks with 64 bit.
 */
class FatFsStorage : public IStorage
{
  public:
    explicit FatFsStorage(std::unique_ptr<ISdBus> bus);
    ~FatFsStorage() override;

    Status mount(void) override;
    bool eject(void) override;
    bool format(void) override;
    bool info(StorageInfo &out) override;

    std::unique_ptr<IFile> open(const char *path, OpenMode mode) override;
    bool exists(const char *path) override;
    bool mkdir(const char *path) override;
    bool remove(const char *path) override;
    bool list(const char *dir, const std::function<void(const char *name, bool isDir)> &cb) override;

    fs::FS *arduinoFS(void) override;

  private:
    friend class FatFsFile;

    std::string fatPath(const char *path) const;
    // closes all open files and unmounts
    void release(void);

    std::unique_ptr<ISdBus> bus;
    bool mounted = false;
    // guards the open file list and every file operation against release()
    std::mutex ioLock;
    std::vector<FatFsFile *> files;
};
