#if defined(ARCH_RP2040) && defined(HAS_SDCARD)

#include "storage/RP2040Storage.h"
#include "SdFat.h"
#if SPI_DRIVER_SELECT != 2
#include "SPI.h"
#endif
#include "util/ISpiLock.h"
#include <algorithm>
#include <cstring>
#include <mutex>
#include <new>
#include <vector>

#ifndef SD_SPI_FREQUENCY
#define SD_SPI_FREQUENCY 25000000U
#endif

struct RP2040Storage::State {
    SdFs filesystem;
    std::mutex ioMutex;
    std::vector<class RP2040StorageFile *> openFiles;
    bool mounted = false;
};

class RP2040StorageFile : public IFile
{
  public:
    RP2040StorageFile(RP2040Storage &storage, FsFile &&file) : storage(storage), file(std::move(file)) {}

    ~RP2040StorageFile() override
    {
        ISpiLock::Guard bus;
        std::lock_guard<std::mutex> io(storage.state->ioMutex);
        closeUnlocked();
        auto &files = storage.state->openFiles;
        files.erase(std::remove(files.begin(), files.end(), this), files.end());
    }

    int32_t read(void *buf, uint32_t len) override
    {
        ISpiLock::Guard bus;
        std::lock_guard<std::mutex> io(storage.state->ioMutex);
        if (!file)
            return -1;
        int n = file.read(static_cast<uint8_t *>(buf), len);
        return n < 0 ? -1 : n;
    }

    int32_t write(const void *buf, uint32_t len) override
    {
        ISpiLock::Guard bus;
        std::lock_guard<std::mutex> io(storage.state->ioMutex);
        if (!file)
            return -1;
        size_t n = file.write(static_cast<const uint8_t *>(buf), len);
        return n == 0 && len != 0 ? -1 : (int32_t)n;
    }

    bool seek(uint64_t pos) override
    {
        ISpiLock::Guard bus;
        std::lock_guard<std::mutex> io(storage.state->ioMutex);
        return file && file.seekSet(pos);
    }

    uint64_t size(void) override
    {
        ISpiLock::Guard bus;
        std::lock_guard<std::mutex> io(storage.state->ioMutex);
        return file ? file.fileSize() : 0;
    }

    void closeUnlocked(void)
    {
        if (file)
            file.close();
    }

  private:
    RP2040Storage &storage;
    FsFile file;
};

namespace
{
#if SPI_DRIVER_SELECT == 2
SoftSpiDriver<SPI_MISO, SPI_MOSI, SPI_SCK> sdSpi;
#endif

SdSpiConfig sdConfig(void)
{
#if SPI_DRIVER_SELECT == 2
    return SdSpiConfig(SDCARD_CS, SHARED_SPI, SD_SPI_FREQUENCY, &sdSpi);
#else
    return SdSpiConfig(SDCARD_CS, SHARED_SPI, SD_SPI_FREQUENCY, &SPI1);
#endif
}

bool configureSpi(void)
{
#if SPI_DRIVER_SELECT == 2
    return true;
#else
    return SPI1.setSCK(SPI_SCK) && SPI1.setTX(SPI_MOSI) && SPI1.setRX(SPI_MISO);
#endif
}

IStorage::Status mountResult(SdFs &filesystem, bool mounted)
{
    if (mounted)
        return IStorage::Status::Ok;
    SdCard *card = filesystem.card();
    if (!card)
        return IStorage::Status::SlotEmpty;
    return card->errorCode() == 0 ? IStorage::Status::NoFilesystem : IStorage::Status::CardError;
}
} // namespace

RP2040Storage::RP2040Storage() : state(new State) {}

RP2040Storage::~RP2040Storage()
{
    ISpiLock::Guard bus;
    std::lock_guard<std::mutex> io(state->ioMutex);
    for (RP2040StorageFile *file : state->openFiles)
        file->closeUnlocked();
    state->openFiles.clear();
    state->filesystem.end();
}

IStorage::Status RP2040Storage::mount(void)
{
    ISpiLock::Guard bus;
    std::lock_guard<std::mutex> io(state->ioMutex);
    for (RP2040StorageFile *file : state->openFiles)
        file->closeUnlocked();
    state->openFiles.clear();
    state->filesystem.end();
    state->mounted = false;
    gen++;

    if (!configureSpi())
        return Status::CardError;
#if SPI_DRIVER_SELECT != 2
    SPI1.begin();
#endif
    state->mounted = state->filesystem.begin(sdConfig());
    return mountResult(state->filesystem, state->mounted);
}

bool RP2040Storage::eject(void)
{
    ISpiLock::Guard bus;
    std::lock_guard<std::mutex> io(state->ioMutex);
    if (!state->mounted)
        return false;
    for (RP2040StorageFile *file : state->openFiles)
        file->closeUnlocked();
    state->openFiles.clear();
    state->filesystem.end();
    state->mounted = false;
    gen++;
    return true;
}

bool RP2040Storage::format(void)
{
    ISpiLock::Guard bus;
    std::lock_guard<std::mutex> io(state->ioMutex);
    for (RP2040StorageFile *file : state->openFiles)
        file->closeUnlocked();
    state->openFiles.clear();

    if (!state->filesystem.card() || state->filesystem.card()->errorCode() != 0)
        return false;
    bool formatted = state->filesystem.format();
    state->mounted = formatted && state->filesystem.volumeBegin();
    gen++;
    return state->mounted;
}

bool RP2040Storage::info(StorageInfo &out)
{
    ISpiLock::Guard bus;
    std::lock_guard<std::mutex> io(state->ioMutex);
    if (!state->mounted || !state->filesystem.card())
        return false;

    uint8_t cardType = state->filesystem.card()->type();
    uint64_t cardBytes = (uint64_t)state->filesystem.card()->sectorCount() * 512;
    out = StorageInfo();
    out.card = cardType == SD_CARD_TYPE_SDHC ? (cardBytes > 32ULL * 1024 * 1024 * 1024 ? StorageInfo::eSDXC : StorageInfo::eSDHC)
                                             : StorageInfo::eSD;
    uint8_t fatType = state->filesystem.fatType();
    out.fat = fatType == FAT_TYPE_EXFAT   ? StorageInfo::eExFat
              : fatType == FAT_TYPE_FAT32 ? StorageInfo::eFat32
              : fatType == FAT_TYPE_FAT16 ? StorageInfo::eFat16
                                          : StorageInfo::eNA;
    uint64_t clusterBytes = state->filesystem.bytesPerCluster();
    uint64_t clusters = state->filesystem.clusterCount();
    int64_t freeClusters = state->filesystem.freeClusterCount();
    if (freeClusters < 0)
        return false;
    out.size = clusters * clusterBytes;
    out.free = (uint64_t)freeClusters * clusterBytes;
    out.used = out.size - out.free;
    return true;
}

std::unique_ptr<IFile> RP2040Storage::open(const char *path, OpenMode mode)
{
    ISpiLock::Guard bus;
    std::lock_guard<std::mutex> io(state->ioMutex);
    if (!state->mounted || !path)
        return nullptr;
    oflag_t flags = mode == OpenMode::Read    ? O_RDONLY
                    : mode == OpenMode::Write ? (oflag_t)(O_WRITE | O_CREAT | O_TRUNC)
                                              : (oflag_t)(O_WRITE | O_CREAT | O_APPEND | O_AT_END);
    FsFile file = state->filesystem.open(path, flags);
    if (!file)
        return nullptr;
    RP2040StorageFile *result = new (std::nothrow) RP2040StorageFile(*this, std::move(file));
    if (!result)
        return nullptr;
    state->openFiles.push_back(result);
    return std::unique_ptr<IFile>(result);
}

bool RP2040Storage::exists(const char *path)
{
    ISpiLock::Guard bus;
    std::lock_guard<std::mutex> io(state->ioMutex);
    return state->mounted && path && state->filesystem.exists(path);
}

bool RP2040Storage::mkdir(const char *path)
{
    ISpiLock::Guard bus;
    std::lock_guard<std::mutex> io(state->ioMutex);
    return state->mounted && path && state->filesystem.mkdir(path, true);
}

bool RP2040Storage::remove(const char *path)
{
    ISpiLock::Guard bus;
    std::lock_guard<std::mutex> io(state->ioMutex);
    return state->mounted && path && state->filesystem.remove(path);
}

bool RP2040Storage::list(const char *dir, const std::function<void(const char *name, bool isDir)> &cb)
{
    ISpiLock::Guard bus;
    std::lock_guard<std::mutex> io(state->ioMutex);
    if (!state->mounted || !dir)
        return false;

    FsFile directory = state->filesystem.open(dir, O_RDONLY);
    if (!directory)
        return false;
    if (!directory.isDir()) {
        directory.close();
        return false;
    }
    FsFile entry;
    char name[256];
    while (entry.openNext(&directory, O_RDONLY)) {
        entry.getName(name, sizeof(name));
        cb(name, entry.isDir());
        entry.close();
    }
    directory.close();
    return true;
}

std::unique_ptr<IStorage> createRP2040Storage(void)
{
    return std::unique_ptr<IStorage>(new RP2040Storage);
}

#endif
