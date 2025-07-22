#pragma once

#include <functional>
#include <stdint.h>

/**
 * Generic interface base class for any log entries (stored via class LogRotate)
 */
class ILogEntry
{
  public:
    virtual size_t size(void) const = 0;   // size of the log entry, including header
    virtual size_t length(void) const = 0; // length of the payload (without header)
    virtual size_t serialize(std::function<size_t(const uint8_t *, size_t)> write) const = 0;
    virtual size_t deserialize(std::function<size_t(uint8_t *, size_t)> read) = 0;
    virtual ~ILogEntry() = default;

  protected:
    ILogEntry(void) = default;

  private:
    ILogEntry(const ILogEntry &) = delete;
    ILogEntry &operator=(const ILogEntry &) = delete;
};
