#pragma once

#include <set>
#include <stdint.h>
#include <string>

/**
 * Statistics of the remote SD card; numeric values match the
 * meshtastic.SdCardInfo interdevice protobuf enums.
 */
struct RemoteSdInfo {
    bool present = false;
    uint8_t cardType = 0; // 0 none, 1 MMC, 2 SD, 3 SDHC, 4 SDXC, 5 unknown
    uint8_t fatType = 0;  // 0 unknown, 1 FAT16, 2 FAT32, 3 exFAT
    uint64_t cardSize = 0;
    uint64_t usedBytes = 0;
    uint64_t freeBytes = 0;
    // usedBytes/freeBytes are only meaningful when true; the scan behind
    // them runs in the background on the co-processor after mount
    bool statsValid = false;
    // a card is in the slot but carries no filesystem (present is false)
    bool unformatted = false;
};

/**
 * Transport for RemoteStorage: chunked file access on a filesystem that lives
 * on another MCU (e.g. the SD card behind the SenseCAP Indicator RP2040).
 * Implemented and registered by the firmware before the view starts.
 */
class IRemoteFS
{
  public:
    /**
     * Read up to len bytes at offset. Returns false on transport error or
     * missing file. *bytesRead receives the chunk size actually read,
     * *fileSize the total size of the file.
     */
    virtual bool readChunk(const char *path, uint32_t offset, uint8_t *buf, uint32_t len, uint32_t *bytesRead,
                           uint32_t *fileSize) = 0;
    /**
     * Sequential chunked write; create=true starts a new file (offset 0),
     * create=false appends the chunk at offset == current file size.
     */
    virtual bool writeChunk(const char *path, uint32_t offset, const uint8_t *buf, uint32_t len, bool create) = 0;
    /**
     * Delete a file. Returns false on transport error or when the file
     * does not exist.
     */
    virtual bool remove(const char *path) = 0;
    /**
     * List all entries of a directory; subdirectories carry a trailing
     * slash. Returns false on transport error or when path is not a
     * directory.
     */
    virtual bool listDir(const char *path, std::set<std::string> &entries) = 0;
    /**
     * Card statistics, answered from a cache on the co-processor so the
     * call stays fast. usedBytes/freeBytes carry real values once
     * statsValid is true; a later call returns them when the background
     * scan has finished. Returns false on transport error.
     */
    virtual bool sdInfo(RemoteSdInfo &info) = 0;
    /**
     * Release the card so it can be pulled safely, or mount it again. The
     * co-processor keeps it released until a mount is asked for.
     */
    virtual bool sdEject(void) = 0;
    virtual bool sdMount(void) = 0;
    /**
     * Put a fresh filesystem on the card, destroying what is on it.
     */
    virtual bool sdFormat(void) = 0;
    virtual ~IRemoteFS() {}
};
