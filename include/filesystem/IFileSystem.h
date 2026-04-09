#pragma once

#include <string>
#include <memory>
#include <cstdint>

/**
 * @brief Abstract file system interface to handle platform-specific file I/O
 * 
 * Provides a unified abstraction over different file system implementations:
 * - P4 POSIX (IDF VFS mounted at /sdcard)
 * - Arduino File API (ESP32-S3, nRF52, etc.)
 * - SdFat library interface
 */
class IFileSystem
{
  public:
    virtual ~IFileSystem() = default;

    /**
     * @brief Open a file
     * @param path File path
     * @param mode "r" for read, "w" for write, "a" for append
     * @return true if successful, false otherwise
     */
    virtual bool open(const std::string &path, const std::string &mode) = 0;

    /**
     * @brief Close current file
     */
    virtual void close() = 0;

    /**
     * @brief Check if file is open
     * @return true if a file is currently open
     */
    virtual bool isOpen() const = 0;

    /**
     * @brief Write formatted string to file (like fprintf)
     * @return number of characters written, or -1 on error
     */
    virtual int printf(const char *format, ...) = 0;

    /**
     * @brief Write raw bytes to file
     * @param data Pointer to data
     * @param size Number of bytes to write
     * @return number of bytes written, or -1 on error
     */
    virtual int write(const void *data, size_t size) = 0;

    /**
     * @brief Read bytes from file
     * @param buffer Buffer to store data
     * @param size Maximum bytes to read
     * @return number of bytes read, or -1 on error
     */
    virtual int read(void *buffer, size_t size) = 0;

    /**
     * @brief Read a line from file (up to newline)
     * @param buffer Buffer to store line
     * @param maxLen Maximum length including null terminator
     * @return true if line was read, false on EOF or error
     */
    virtual bool readLine(char *buffer, size_t maxLen) = 0;

    /**
     * @brief Create directory recursively
     * @param path Directory path
     * @return true if successful or already exists, false on error
     */
    virtual bool mkdir(const std::string &path) = 0;

    /**
     * @brief Get last error message
     * @return Error string describing last operation failure
     */
    virtual std::string getLastError() const = 0;
};

/**
 * @brief Get platform-appropriate file system implementation
 * @return Unique pointer to IFileSystem instance
 */
std::unique_ptr<IFileSystem> createFileSystem();
