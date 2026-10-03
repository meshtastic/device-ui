#include "Arduino.h"

#if (defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC)) && !defined(CONFIG_IDF_TARGET_ESP32P4)

#include "filesystem/ArduinoFileSystem.h"
#include <cstdarg>
#include <stdarg.h>

bool ArduinoFileSystem::open(const std::string &path, const std::string &mode)
{
    close(); // Close any previously opened file
#if 0
    uint8_t arduino_mode;
    if (mode == "r") {
        arduino_mode = FILE_READ;
    } else if (mode == "w") {
        arduino_mode = FILE_WRITE;
    } else if (mode == "a") {
        arduino_mode = FILE_APPEND;
    } else {
        lastError = "Unknown mode: " + mode;
        return false;
    }
#endif
    currentFile = SDFs.open(path.c_str(), mode.c_str());
    if (!currentFile) {
        lastError = "Failed to open '" + path + "'";
        return false;
    }
    return true;
}

void ArduinoFileSystem::close()
{
    if (currentFile) {
        currentFile.close();
    }
}

bool ArduinoFileSystem::isOpen() const
{
    return (bool)currentFile;
}

int ArduinoFileSystem::printf(const char *format, ...)
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

    size_t written = currentFile.write((const uint8_t *)buffer, len);
    return written;
}

int ArduinoFileSystem::write(const void *data, size_t size)
{
    if (!currentFile) {
        lastError = "No file open";
        return -1;
    }
    size_t written = currentFile.write((const uint8_t *)data, size);
    return written;
}

int ArduinoFileSystem::read(void *buffer, size_t size)
{
    if (!currentFile) {
        lastError = "No file open";
        return -1;
    }
    int nread = currentFile.read((uint8_t *)buffer, size);
    return nread;
}

bool ArduinoFileSystem::readLine(char *buffer, size_t maxLen)
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

bool ArduinoFileSystem::mkdir(const std::string &path)
{
    if (!SDFs.mkdir(path.c_str())) {
        // mkdir can return false even if dir exists, so this is non-fatal
        lastError = "mkdir may have failed or dir exists";
    }
    return true;
}

std::string ArduinoFileSystem::getLastError() const
{
    return lastError;
}

#endif // ARCH_PORTDUINO || HAS_SD_MMC
