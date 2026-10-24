#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace fs
{
class FS;
}

enum class OpenMode {
    Read,
    Write, // truncates
    Append,
};

/**
 * An open file, closed by the destructor. Offsets are 64 bit so that exFAT
 * archives beyond 4 GiB can be addressed.
 */
class IFile
{
  public:
    virtual ~IFile() = default;

    // bytes transferred, -1 on error
    virtual int32_t read(void *buf, uint32_t len) = 0;
    virtual int32_t write(const void *buf, uint32_t len) = 0;
    virtual bool seek(uint64_t pos) = 0;
    virtual uint64_t size(void) = 0;

    bool readAt(uint64_t pos, void *buf, uint32_t len) { return seek(pos) && read(buf, len) == (int32_t)len; }
    // reads up to (excluding) the next '\n'; false at end of file
    bool readLine(char *buf, uint32_t len);
    bool print(const char *text);
};

struct StorageInfo {
    enum CardType : uint8_t { eNone, eMMC, eSD, eSDHC, eSDXC, eUnknown };
    enum FatType : uint8_t { eNA, eFat16, eFat32, eExFat };

    CardType card = eNone;
    FatType fat = eNA;
    uint64_t size = 0;
    uint64_t used = 0;
    uint64_t free = 0;
    // false while used/free are still being computed (e.g. by a co-processor)
    bool statsValid = true;
};

/**
 * A storage volume (SD card or a host folder) with the file operations the UI needs.
 * Paths are absolute ("/maps/..."), relative to the root of the volume.
 */
class IStorage
{
  public:
    enum class Status { Ok, SlotEmpty, CardError, NoFilesystem };

    virtual ~IStorage() = default;

    // false when there is no removable card to show in the UI
    virtual bool hasSlot(void) const { return true; }
    // (re-)detects and mounts the card
    virtual Status mount(void) = 0;
    // releases the card so it can be pulled safely
    virtual bool eject(void) { return false; }
    // puts a fresh filesystem on a card that has none
    virtual bool format(void) { return false; }
    // false when the card or the link to it is gone
    virtual bool info(StorageInfo &out) = 0;

    virtual std::unique_ptr<IFile> open(const char *path, OpenMode mode) = 0;
    virtual bool exists(const char *path) = 0;
    // creates missing parent directories as well
    virtual bool mkdir(const char *path) = 0;
    virtual bool remove(const char *path) = 0;
    virtual bool list(const char *dir, const std::function<void(const char *name, bool isDir)> &cb) = 0;

    // Arduino view of the volume (FTP server); nullptr when not available
    virtual fs::FS *arduinoFS(void) { return nullptr; }

    // changes whenever the volume is (re-)mounted or released; open files do not survive that
    uint32_t generation(void) const { return gen; }

  protected:
    uint32_t gen = 0;
};

/**
 * Storage without a card: every operation fails.
 */
class NullStorage : public IStorage
{
  public:
    bool hasSlot(void) const override { return false; }
    Status mount(void) override { return Status::SlotEmpty; }
    bool info(StorageInfo &out) override { return false; }
    std::unique_ptr<IFile> open(const char *, OpenMode) override { return nullptr; }
    bool exists(const char *) override { return false; }
    bool mkdir(const char *) override { return false; }
    bool remove(const char *) override { return false; }
    bool list(const char *, const std::function<void(const char *, bool)> &) override { return false; }
};

// the storage backend of this platform; the only place that selects one
std::unique_ptr<IStorage> createStorage(void);
