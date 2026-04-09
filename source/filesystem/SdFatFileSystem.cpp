#if defined(HAS_SDCARD) && !defined(HAS_SD_MMC)

#include "filesystem/SdFatFileSystem.h"
#include <stdarg.h>
#include <cstdarg>

bool SdFatFileSystem::open(const std::string &path, const std::string &mode)
{
    close(); // Close any previously opened file

    uint8_t oflag;
    if (mode == "r") {
        oflag = O_RDONLY;
    } else if (mode == "w") {
        oflag = O_RDWR | O_CREAT | O_TRUNC;
    } else if (mode == "a") {
        oflag = O_RDWR | O_CREAT | O_APPEND;
    } else {
        lastError = "Unknown mode: " + mode;
        return false;
    }

    currentFile = SDFs.open(path.c_str(), oflag);
    if (!currentFile) {
        lastError = "Failed to open '" + path + "'";
        return false;
    }
    return true;
}

void SdFatFileSystem::close()
{
    if (currentFile) {
        currentFile.close();
    }
}

bool SdFatFileSystem::isOpen() const
{
    return (bool)currentFile;
}

int SdFatFileSystem::printf(const char *format, ...)
{
    if (!currentFile) {
        lastError = "No file open";
        return -1;
    }
    va_list args;
    va_start(args, format);
    char buffer[512];
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (len < 0) {
        lastError = "vsnprintf failed";
        return -1;
    }

    size_t written = currentFile.write(buffer, len);
    return written;
}

int SdFatFileSystem::write(const void *data, size_t size)
{
    if (!currentFile) {
        lastError = "No file open";
        return -1;
    }
    size_t written = currentFile.write(data, size);
    return written;
}

int SdFatFileSystem::read(void *buffer, size_t size)
{
    if (!currentFile) {
        lastError = "No file open";
        return -1;
    }
    int nread = currentFile.read(buffer, size);
    return nread;
}

bool SdFatFileSystem::readLine(char *buffer, size_t maxLen)
{
    if (!currentFile) {
        lastError = "No file open";
        return false;
    }
    // Read bytes until newline or buffer full
    size_t pos = 0;
    while (pos < maxLen - 1 && currentFile.available()) {
        int c = currentFile.read();
        if (c < 0)
            break;
        buffer[pos++] = c;
        if (c == '\n')
            break;
    }
    buffer[pos] = '\0';

    if (pos == 0) {
        lastError = "EOF";
        return false;
    }
    return true;
}

bool SdFatFileSystem::mkdir(const std::string &path)
{
    if (!SDFs.mkdir(path.c_str())) {
        lastError = "mkdir failed";
        return false;
    }
    return true;
}

std::string SdFatFileSystem::getLastError() const
{
    return lastError;
}

#endif // HAS_SDCARD
