#include "graphics/map/MapFileSystem.h"

#if defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC) || defined(HAS_SDCARD) || defined(SENSECAP_INDICATOR)

#include "util/ISpiLock.h"
#include <cstring>

#if (defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC) || defined(SDCARD_SHARE_SPI)) && !defined(SENSECAP_INDICATOR)

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
    if (offset > UINT32_MAX)
        return false;
    ISpiLock::Guard bus;
    return file && file.seek((uint32_t)offset) && file.read(buf, len) == (size_t)len;
}

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
    if (offset > UINT32_MAX)
        return false;
    ISpiLock::Guard bus;
    return file && file.seekSet((uint32_t)offset) && file.read(buf, len) == (size_t)len;
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