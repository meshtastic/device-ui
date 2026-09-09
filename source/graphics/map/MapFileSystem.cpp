#include "graphics/map/MapFileSystem.h"

#if defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC) || defined(HAS_SDCARD) || defined(SENSECAP_INDICATOR)

#include "util/ISpiLock.h"
#include <cstring>

#if (defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC) || defined(SDCARD_SHARE_SPI)) && !defined(SENSECAP_INDICATOR)

#if defined(MAPFILE_USE_FATFS)

#include "ff.h"
#include <cstdlib>
#include <string>

// SD_MMC mounts the card as FatFs drive 0; SDMMCFS itself assumes the same
#define MAPFILE_FATFS_DRIVE "0:"

bool SDMapFileSystem::open(const char *path)
{
    ISpiLock::Guard bus;
    if (fil) {
        f_close((FIL *)fil);
        free(fil);
        fil = nullptr;
    }
    if (!path)
        return false;

    // FIL embeds a FF_MAX_SS sector cache (4 KB here), so it never goes on the stack
    FIL *f = (FIL *)malloc(sizeof(FIL));
    if (!f)
        return false;

    std::string volpath = std::string(MAPFILE_FATFS_DRIVE) + path;
    if (f_open(f, volpath.c_str(), FA_READ) != FR_OK) {
        free(f);
        return false;
    }
    fil = f;
    return true;
}

void SDMapFileSystem::close(void)
{
    ISpiLock::Guard bus;
    if (fil) {
        f_close((FIL *)fil);
        free(fil);
        fil = nullptr;
    }
}

bool SDMapFileSystem::readAt(uint64_t offset, uint8_t *buf, uint32_t len)
{
    ISpiLock::Guard bus;
    if (!fil)
        return false;
    // FSIZE_t is 32 bit unless FatFs was built with exFAT; folds away when it is 64
    if (offset > (uint64_t)(FSIZE_t)-1)
        return false;
    FIL *f = (FIL *)fil;
    // f_lseek clamps past EOF instead of failing, so confirm where we landed
    if (f_lseek(f, (FSIZE_t)offset) != FR_OK || (uint64_t)f_tell(f) != offset)
        return false;
    UINT read = 0;
    return f_read(f, buf, len, &read) == FR_OK && read == len;
}

#else

#if defined(SDCARD_SHARE_SPI) && !defined(ARCH_PORTDUINO) && !defined(HAS_SD_MMC)
#include "SD.h"
#define MAPFILE_OPEN(path) SD.open(path, FILE_READ)
#else
#define MAPFILE_OPEN(path) SDFs.open(path, FILE_READ)
#endif

bool SDMapFileSystem::open(const char *path)
{
    ISpiLock::Guard bus;
    if (file)
        file.close();
    file = MAPFILE_OPEN(path);
    return file;
}

void SDMapFileSystem::close(void)
{
    ISpiLock::Guard bus;
    if (file)
        file.close();
}

bool SDMapFileSystem::readAt(uint64_t offset, uint8_t *buf, uint32_t len)
{
    // seek() is uint32_t and ends in fseek(long), so this tops out at 2 GiB
    if (offset > INT32_MAX)
        return false;
    ISpiLock::Guard bus;
    return file && file.seek((uint32_t)offset) && file.read(buf, len) == (size_t)len;
}

#endif // MAPFILE_USE_FATFS

#elif defined(HAS_SDCARD) && !defined(SENSECAP_INDICATOR)

bool SdFatMapFileSystem::open(const char *path)
{
    ISpiLock::Guard bus;
    if (file)
        file.close();
    file = SDFs.open(path, O_RDONLY);
    return file;
}

void SdFatMapFileSystem::close(void)
{
    ISpiLock::Guard bus;
    if (file)
        file.close();
}

bool SdFatMapFileSystem::readAt(uint64_t offset, uint8_t *buf, uint32_t len)
{
    ISpiLock::Guard bus;
    return file && file.seekSet(offset) && file.read(buf, len) == (size_t)len;
}

#elif defined(SENSECAP_INDICATOR)

#include "graphics/map/RemoteSDService.h"

RemoteMapFileSystem::RemoteMapFileSystem()
{
    path[0] = '\0';
}

bool RemoteMapFileSystem::open(const char *filePath)
{
    IRemoteFS *fs = RemoteSDService::backend();
    if (!fs || !filePath)
        return false;

    uint8_t probe = 0;
    uint32_t bytesRead = 0, fileSize = 0;
    if (!fs->readChunk(filePath, 0, &probe, 1, &bytesRead, &fileSize) || bytesRead == 0 || fileSize == 0)
        return false;

    strncpy(path, filePath, sizeof(path) - 1);
    path[sizeof(path) - 1] = '\0';
    return true;
}

void RemoteMapFileSystem::close(void)
{
    path[0] = '\0';
}

bool RemoteMapFileSystem::readAt(uint64_t offset, uint8_t *buf, uint32_t len)
{
    static constexpr uint32_t READ_CHUNK = 4096;
    IRemoteFS *fs = RemoteSDService::backend();
    // the remote protocol addresses chunks with a uint32_t offset
    if (!fs || !path[0] || offset > UINT32_MAX)
        return false;

    uint32_t pos = (uint32_t)offset;
    uint32_t total = 0;
    while (total < len) {
        uint32_t bytesRead = 0, fileSize = 0;
        uint32_t req = len - total > READ_CHUNK ? READ_CHUNK : len - total;
        if (!fs->readChunk(path, pos, buf + total, req, &bytesRead, &fileSize) || bytesRead == 0)
            return false;
        pos += bytesRead;
        total += bytesRead;
    }
    return true;
}

#endif

#endif