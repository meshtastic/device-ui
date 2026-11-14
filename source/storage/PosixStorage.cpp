#if defined(ARCH_PORTDUINO) || !defined(ARDUINO)

// 64-bit off_t on 32-bit Linux hosts, so archives beyond 2 GiB stay addressable
#define _FILE_OFFSET_BITS 64

#include "storage/PosixStorage.h"
#include <cerrno>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

namespace
{
class PosixFile : public IFile
{
  public:
    explicit PosixFile(FILE *f) : f(f) {}
    ~PosixFile() override { fclose(f); }

    int32_t read(void *buf, uint32_t len) override
    {
        size_t n = fread(buf, 1, len, f);
        return n == 0 && ferror(f) ? -1 : (int32_t)n;
    }

    int32_t write(const void *buf, uint32_t len) override
    {
        size_t n = fwrite(buf, 1, len, f);
        return n == 0 && len > 0 ? -1 : (int32_t)n;
    }

    bool seek(uint64_t pos) override { return fseeko(f, (off_t)pos, SEEK_SET) == 0; }

    uint64_t size(void) override
    {
        struct stat st;
        return fstat(fileno(f), &st) == 0 ? (uint64_t)st.st_size : 0;
    }

  private:
    FILE *f;
};
} // namespace

PosixStorage::PosixStorage(const char *root) : root(root ? root : "")
{
    while (!this->root.empty() && this->root.back() == '/')
        this->root.pop_back();
}

std::string PosixStorage::full(const char *path) const
{
    if (!path || !*path)
        return root.empty() ? "/" : root;
    return path[0] == '/' ? root + path : root + "/" + path;
}

IStorage::Status PosixStorage::mount(void)
{
    gen++;
    struct stat st;
    return stat(full("/").c_str(), &st) == 0 && S_ISDIR(st.st_mode) ? Status::Ok : Status::SlotEmpty;
}

bool PosixStorage::info(StorageInfo &out)
{
    struct statvfs vfs;
    if (statvfs(full("/").c_str(), &vfs) != 0)
        return false;
    out = StorageInfo();
    out.card = StorageInfo::eUnknown;
    out.size = (uint64_t)vfs.f_blocks * vfs.f_frsize;
    out.free = (uint64_t)vfs.f_bavail * vfs.f_frsize;
    out.used = out.size - (uint64_t)vfs.f_bfree * vfs.f_frsize;
    return true;
}

std::unique_ptr<IFile> PosixStorage::open(const char *path, OpenMode mode)
{
    const char *m = mode == OpenMode::Read ? "rb" : mode == OpenMode::Write ? "wb" : "ab";
    FILE *f = fopen(full(path).c_str(), m);
    if (!f)
        return nullptr;
    return std::unique_ptr<IFile>(new PosixFile(f));
}

bool PosixStorage::exists(const char *path)
{
    struct stat st;
    return stat(full(path).c_str(), &st) == 0;
}

bool PosixStorage::mkdir(const char *path)
{
    std::string p = full(path);
    for (size_t pos = root.size() + 1; pos <= p.size(); pos++) {
        if (pos == p.size() || p[pos] == '/') {
            std::string part = p.substr(0, pos);
            if (::mkdir(part.c_str(), 0775) != 0 && errno != EEXIST)
                return false;
        }
    }
    return true;
}

bool PosixStorage::remove(const char *path)
{
    return ::remove(full(path).c_str()) == 0;
}

bool PosixStorage::list(const char *dir, const std::function<void(const char *name, bool isDir)> &cb)
{
    std::string base = full(dir);
    DIR *d = opendir(base.c_str());
    if (!d)
        return false;
    while (struct dirent *entry = readdir(d)) {
        if (entry->d_name[0] == '.' && (entry->d_name[1] == '\0' || (entry->d_name[1] == '.' && entry->d_name[2] == '\0')))
            continue;
        struct stat st;
        bool isDir = stat((base + "/" + entry->d_name).c_str(), &st) == 0 && S_ISDIR(st.st_mode);
        cb(entry->d_name, isDir);
    }
    closedir(d);
    return true;
}

#endif
