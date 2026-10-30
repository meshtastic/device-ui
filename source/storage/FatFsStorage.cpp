#ifdef ARDUINO_ARCH_ESP32

#include "storage/FatFsStorage.h"
#include "ff.h"
#include "util/ILog.h"
#include "util/ISpiLock.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>

class FatFsFile : public IFile
{
  public:
    FatFsFile(FatFsStorage &owner, FIL *fil) : owner(owner), fil(fil) {}

    ~FatFsFile() override
    {
        ISpiLock::Guard bus;
        std::lock_guard<std::mutex> io(owner.ioLock);
        close();
        owner.files.erase(std::remove(owner.files.begin(), owner.files.end(), this), owner.files.end());
    }

    int32_t read(void *buf, uint32_t len) override
    {
        ISpiLock::Guard bus;
        std::lock_guard<std::mutex> io(owner.ioLock);
        UINT n = 0;
        if (!fil || f_read(fil, buf, len, &n) != FR_OK)
            return -1;
        return (int32_t)n;
    }

    int32_t write(const void *buf, uint32_t len) override
    {
        ISpiLock::Guard bus;
        std::lock_guard<std::mutex> io(owner.ioLock);
        UINT n = 0;
        if (!fil || f_write(fil, buf, len, &n) != FR_OK)
            return -1;
        return (int32_t)n;
    }

    bool seek(uint64_t pos) override
    {
        ISpiLock::Guard bus;
        std::lock_guard<std::mutex> io(owner.ioLock);
        // FSIZE_t is 32 bit unless FatFs was built with exFAT
        if (!fil || pos > (uint64_t)(FSIZE_t)-1)
            return false;
        // f_lseek clamps past EOF instead of failing, so confirm where we landed
        return f_lseek(fil, (FSIZE_t)pos) == FR_OK && (uint64_t)f_tell(fil) == pos;
    }

    uint64_t size(void) override
    {
        std::lock_guard<std::mutex> io(owner.ioLock);
        return fil ? (uint64_t)f_size(fil) : 0;
    }

    // caller holds ioLock
    void close(void)
    {
        if (fil) {
            f_close(fil);
            free(fil);
            fil = nullptr;
        }
    }

  private:
    FatFsStorage &owner;
    FIL *fil;
};

FatFsStorage::FatFsStorage(std::unique_ptr<ISdBus> bus) : bus(std::move(bus)) {}

FatFsStorage::~FatFsStorage()
{
    ISpiLock::Guard guard;
    release();
}

void FatFsStorage::release(void)
{
    {
        std::lock_guard<std::mutex> io(ioLock);
        for (FatFsFile *file : files)
            file->close();
    }
    if (mounted) {
        bus->unmount();
        mounted = false;
    }
    gen++;
}

std::string FatFsStorage::fatPath(const char *path) const
{
    std::string p(1, (char)('0' + bus->drive()));
    p += ':';
    if (!path || path[0] != '/')
        p += '/';
    if (path)
        p += path;
    while (p.size() > 3 && p.back() == '/')
        p.pop_back();
    return p;
}

IStorage::Status FatFsStorage::mount(void)
{
    ISpiLock::Guard guard;
    release();
    Status status = bus->mount(false);
    mounted = status == Status::Ok;
    return status;
}

bool FatFsStorage::eject(void)
{
    ISpiLock::Guard guard;
    if (!mounted)
        return false;
    release();
    return true;
}

bool FatFsStorage::format(void)
{
    ISpiLock::Guard guard;
    release();
    mounted = bus->mount(true) == Status::Ok;
    return mounted;
}

bool FatFsStorage::info(StorageInfo &out)
{
    ISpiLock::Guard guard;
    if (!mounted)
        return false;
    FATFS *fs = nullptr;
    DWORD freeClusters = 0;
    char drv[3] = {(char)('0' + bus->drive()), ':', '\0'};
    if (f_getfree(drv, &freeClusters, &fs) != FR_OK || !fs)
        return false;
#if FF_MAX_SS != FF_MIN_SS
    uint64_t clusterBytes = (uint64_t)fs->csize * fs->ssize;
#else
    uint64_t clusterBytes = (uint64_t)fs->csize * FF_MAX_SS;
#endif
    out = StorageInfo();
    out.card = bus->cardType();
    out.fat = fs->fs_type == FS_EXFAT   ? StorageInfo::eExFat
              : fs->fs_type == FS_FAT32 ? StorageInfo::eFat32
              : fs->fs_type != 0        ? StorageInfo::eFat16
                                        : StorageInfo::eNA;
    out.size = (uint64_t)(fs->n_fatent - 2) * clusterBytes;
    out.free = (uint64_t)freeClusters * clusterBytes;
    out.used = out.size - out.free;
    return true;
}

std::unique_ptr<IFile> FatFsStorage::open(const char *path, OpenMode mode)
{
    ISpiLock::Guard guard;
    if (!mounted || !path)
        return nullptr;
    // FIL embeds a sector cache of FF_MAX_SS bytes, so it never goes on the stack
    FIL *fil = (FIL *)malloc(sizeof(FIL));
    if (!fil)
        return nullptr;
    BYTE flags = mode == OpenMode::Read    ? FA_READ
                 : mode == OpenMode::Write ? (BYTE)(FA_WRITE | FA_CREATE_ALWAYS)
                                           : (BYTE)(FA_WRITE | FA_OPEN_APPEND);
    if (f_open(fil, fatPath(path).c_str(), flags) != FR_OK) {
        free(fil);
        return nullptr;
    }
    FatFsFile *file = new FatFsFile(*this, fil);
    std::lock_guard<std::mutex> io(ioLock);
    files.push_back(file);
    return std::unique_ptr<IFile>(file);
}

bool FatFsStorage::exists(const char *path)
{
    ISpiLock::Guard guard;
    if (!mounted)
        return false;
    std::string p = fatPath(path);
    if (p.size() <= 3) // the root has no directory entry
        return true;
    FILINFO fno;
    return f_stat(p.c_str(), &fno) == FR_OK;
}

bool FatFsStorage::mkdir(const char *path)
{
    ISpiLock::Guard guard;
    if (!mounted)
        return false;
    std::string p = fatPath(path);
    if (p.size() <= 3)
        return true;
    for (size_t pos = 3; pos <= p.size(); pos++) {
        if (pos == p.size() || p[pos] == '/') {
            FRESULT res = f_mkdir(p.substr(0, pos).c_str());
            if (res != FR_OK && res != FR_EXIST)
                return false;
        }
    }
    return true;
}

bool FatFsStorage::remove(const char *path)
{
    ISpiLock::Guard guard;
    return mounted && f_unlink(fatPath(path).c_str()) == FR_OK;
}

bool FatFsStorage::list(const char *dir, const std::function<void(const char *name, bool isDir)> &cb)
{
    ISpiLock::Guard guard;
    if (!mounted)
        return false;
    FF_DIR d;
    if (f_opendir(&d, fatPath(dir).c_str()) != FR_OK)
        return false;
    FILINFO fno;
    while (f_readdir(&d, &fno) == FR_OK && fno.fname[0])
        cb(fno.fname, (fno.fattrib & AM_DIR) != 0);
    f_closedir(&d);
    return true;
}

fs::FS *FatFsStorage::arduinoFS(void)
{
    return mounted ? bus->arduinoFS() : nullptr;
}

#endif
