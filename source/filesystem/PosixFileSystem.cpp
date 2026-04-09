#include "filesystem/PosixFileSystem.h"
#include <cerrno>
#include <stdarg.h>
#include <sys/stat.h>

PosixFileSystem::~PosixFileSystem()
{
    close();
}

bool PosixFileSystem::open(const std::string &path, const std::string &mode)
{
    close(); // Close any previously opened file
    currentFile = ::fopen(path.c_str(), mode.c_str());
    if (!currentFile) {
        lastError = "Failed to open '" + path + "'";
        return false;
    }
    return true;
}

void PosixFileSystem::close()
{
    if (currentFile) {
        ::fclose(currentFile);
        currentFile = nullptr;
    }
}

bool PosixFileSystem::isOpen() const
{
    return currentFile != nullptr;
}

int PosixFileSystem::printf(const char *format, ...)
{
    if (!currentFile) {
        lastError = "No file open";
        return -1;
    }
    va_list args;
    va_start(args, format);
    int result = ::vfprintf(currentFile, format, args);
    va_end(args);
    return result;
}

int PosixFileSystem::write(const void *data, size_t size)
{
    if (!currentFile) {
        lastError = "No file open";
        return -1;
    }
    size_t written = ::fwrite(data, 1, size, currentFile);
    if (written != size) {
        lastError = "Write error: wrote " + std::to_string(written) + " of " + std::to_string(size);
    }
    return written;
}

int PosixFileSystem::read(void *buffer, size_t size)
{
    if (!currentFile) {
        lastError = "No file open";
        return -1;
    }
    size_t nread = ::fread(buffer, 1, size, currentFile);
    return nread;
}

bool PosixFileSystem::readLine(char *buffer, size_t maxLen)
{
    if (!currentFile) {
        lastError = "No file open";
        return false;
    }
    if (::fgets(buffer, maxLen, currentFile) == nullptr) {
        if (::feof(currentFile)) {
            lastError = "EOF";
        } else {
            lastError = "Read error";
        }
        return false;
    }
    return true;
}

bool PosixFileSystem::mkdir(const std::string &path)
{
    if (::mkdir(path.c_str(), 0775) != 0 && errno != EEXIST) {
        lastError = "mkdir failed with errno " + std::to_string(errno);
        return false;
    }
    return true;
}

std::string PosixFileSystem::getLastError() const
{
    return lastError;
}
