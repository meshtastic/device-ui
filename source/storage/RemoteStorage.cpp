#include "storage/RemoteStorage.h"
#include <algorithm>
#include <cstring>
#include <new>
#include <string>

IRemoteFS *RemoteStorage::remoteFS = nullptr;

namespace
{
constexpr uint32_t CHUNK_SIZE = 4096; // matches FileTransfer.filedata max_size

class RemoteFile : public IFile
{
  public:
    RemoteFile(IRemoteFS &fs, const char *path, bool writable) : fs(fs), path(path), writable(writable) {}

    // fetches the first chunk: proves the file exists and yields its size
    bool probe(void) { return fill(0); }

    void appendAt(uint32_t end)
    {
        created = true;
        pos = fileSize = end;
    }

    int32_t read(void *buf, uint32_t len) override
    {
        if (writable)
            return -1;
        uint8_t *dst = static_cast<uint8_t *>(buf);
        uint32_t total = 0;
        while (total < len && pos < fileSize) {
            if (cacheLen && pos >= cacheOffset && pos < cacheOffset + cacheLen) {
                uint32_t n = std::min(len - total, cacheOffset + cacheLen - pos);
                memcpy(dst + total, cache.get() + (pos - cacheOffset), n);
                pos += n;
                total += n;
            } else if (len - total >= CHUNK_SIZE) {
                uint32_t got = 0, size = fileSize;
                if (!fs.readChunk(path.c_str(), pos, dst + total, CHUNK_SIZE, &got, &size) || got == 0)
                    return total ? (int32_t)total : -1;
                fileSize = size;
                pos += got;
                total += got;
            } else if (!fill(pos)) {
                return total ? (int32_t)total : -1;
            }
        }
        return (int32_t)total;
    }

    int32_t write(const void *buf, uint32_t len) override
    {
        if (!writable)
            return -1;
        const uint8_t *src = static_cast<const uint8_t *>(buf);
        uint32_t total = 0;
        while (total < len) {
            uint32_t n = std::min(len - total, CHUNK_SIZE);
            if ((uint64_t)pos + n > UINT32_MAX || !fs.writeChunk(path.c_str(), pos, src + total, n, !created))
                return total ? (int32_t)total : -1;
            created = true;
            pos += n;
            total += n;
        }
        fileSize = std::max(fileSize, pos);
        return (int32_t)total;
    }

    bool seek(uint64_t to) override
    {
        // the protocol only appends, so a writer cannot move
        if (to > UINT32_MAX || (writable && to != pos))
            return false;
        pos = (uint32_t)to;
        return true;
    }

    uint64_t size(void) override { return fileSize; }

  private:
    bool fill(uint32_t at)
    {
        if (!cache)
            cache.reset(new (std::nothrow) uint8_t[CHUNK_SIZE]);
        uint32_t got = 0, size = 0;
        if (!cache || !fs.readChunk(path.c_str(), at, cache.get(), CHUNK_SIZE, &got, &size) || got == 0) {
            cacheLen = 0;
            return false;
        }
        cacheOffset = at;
        cacheLen = got;
        fileSize = size;
        return true;
    }

    IRemoteFS &fs;
    std::string path;
    bool writable;
    bool created = false;
    uint32_t pos = 0;
    uint32_t fileSize = 0;
    // one chunk of read-ahead: the PNG decoder and the pmtiles reader ask for small pieces
    std::unique_ptr<uint8_t[]> cache;
    uint32_t cacheOffset = 0;
    uint32_t cacheLen = 0;
};
} // namespace

void RemoteStorage::setBackend(IRemoteFS *fs)
{
    remoteFS = fs;
}

IRemoteFS *RemoteStorage::backend(void)
{
    return remoteFS;
}

IStorage::Status RemoteStorage::mount(void)
{
    gen++;
    RemoteSdInfo sd;
    if (!remoteFS)
        return Status::SlotEmpty;
    // revives a card that was ejected or replaced; mounting a mounted card is a no-op
    remoteFS->sdMount();
    if (!remoteFS->sdInfo(sd))
        return Status::SlotEmpty;
    if (sd.present)
        return Status::Ok;
    return sd.unformatted ? Status::NoFilesystem : Status::SlotEmpty;
}

bool RemoteStorage::eject(void)
{
    if (!remoteFS || !remoteFS->sdEject())
        return false;
    gen++;
    return true;
}

bool RemoteStorage::format(void)
{
    return remoteFS && remoteFS->sdFormat();
}

bool RemoteStorage::info(StorageInfo &out)
{
    RemoteSdInfo sd;
    if (!remoteFS || !remoteFS->sdInfo(sd) || !sd.present)
        return false;
    // numeric values follow the meshtastic.SdCardInfo protobuf enums
    static const StorageInfo::CardType cards[] = {StorageInfo::eNone, StorageInfo::eMMC,  StorageInfo::eSD,
                                                  StorageInfo::eSDHC, StorageInfo::eSDXC, StorageInfo::eUnknown};
    static const StorageInfo::FatType fats[] = {StorageInfo::eNA, StorageInfo::eFat16, StorageInfo::eFat32, StorageInfo::eExFat};
    out = StorageInfo();
    out.card = sd.cardType < sizeof(cards) / sizeof(cards[0]) ? cards[sd.cardType] : StorageInfo::eNone;
    out.fat = sd.fatType < sizeof(fats) / sizeof(fats[0]) ? fats[sd.fatType] : StorageInfo::eNA;
    out.size = sd.cardSize;
    out.used = sd.usedBytes;
    out.free = sd.freeBytes;
    out.statsValid = sd.statsValid;
    return true;
}

std::unique_ptr<IFile> RemoteStorage::open(const char *path, OpenMode mode)
{
    if (!remoteFS || !path)
        return nullptr;
    std::unique_ptr<RemoteFile> file(new RemoteFile(*remoteFS, path, mode != OpenMode::Read));
    if (mode == OpenMode::Read) {
        if (!file->probe())
            return nullptr;
    } else if (mode == OpenMode::Append) {
        uint8_t byte;
        uint32_t got = 0, size = 0;
        if (remoteFS->readChunk(path, 0, &byte, 1, &got, &size) && got > 0)
            file->appendAt(size);
    }
    return file;
}

bool RemoteStorage::exists(const char *path)
{
    if (!remoteFS || !path)
        return false;
    uint8_t byte;
    uint32_t got = 0, size = 0;
    if (remoteFS->readChunk(path, 0, &byte, 1, &got, &size) && got > 0)
        return true;
    std::set<std::string> entries;
    return remoteFS->listDir(path, entries);
}

bool RemoteStorage::mkdir(const char *path)
{
    // the protocol has no mkdir: the co-processor creates parent directories on write
    return remoteFS != nullptr;
}

bool RemoteStorage::remove(const char *path)
{
    return remoteFS && path && remoteFS->remove(path);
}

bool RemoteStorage::list(const char *dir, const std::function<void(const char *name, bool isDir)> &cb)
{
    std::set<std::string> entries;
    if (!remoteFS || !dir || !remoteFS->listDir(dir, entries))
        return false;
    for (const auto &entry : entries) {
        if (entry.empty())
            continue;
        bool isDir = entry.back() == '/';
        cb(isDir ? entry.substr(0, entry.size() - 1).c_str() : entry.c_str(), isDir);
    }
    return true;
}
