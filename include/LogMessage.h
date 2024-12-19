#pragma once

#include "ILogEntry.h"
#include <assert.h>
#include <ctime>
#include <memory.h>

constexpr uint32_t messagePayloadSize = 233;

/**
 * @brief Header for storing message logs containing the actual size of the payload
 * Note: this struct does have vtable pointers, i.e. sizeof(LogMessageHeader)-8 is the real data size
 */
struct LogMessageHeader : public ILogEntry {
    uint16_t _size;
    time_t time;
    uint32_t from;
    uint32_t to;
    uint8_t ch;
    enum MsgStatus : uint8_t { eDefault, eHeard, eNoResponse, eAcked, eFailed, eDeleted, eUnread } status;
    bool trashFlag;
    uint32_t reserved;
};

/**
 * @brief Structure for storing message logs containing the actual payload
 */
struct LogMessage : public LogMessageHeader {
    uint8_t bytes[messagePayloadSize];
};

/**
 * Log message envelope that implements the actual interface for ILogEntry
 * (size, serialize and deserialize)
 */
class LogMessageEnv : public LogMessage
{
  public:
    LogMessageEnv(void) = default;
    LogMessageEnv(uint32_t _from, uint32_t _to, uint16_t _ch, time_t _time, MsgStatus _status, bool _trashFlag, uint32_t _len,
                  const uint8_t *msg)
    {
        assert(_len < messagePayloadSize);
        _size = (uint16_t)_len;
        time = _time;
        from = _from;
        to = _to;
        ch = _ch;
        status = _status;
        trashFlag = _trashFlag;
        reserved = 0;
        memcpy(bytes, msg, _len);
    }

    size_t size(void) const override { return sizeof(LogMessageHeader) - 8 + _size; }

    virtual size_t serialize(std::function<size_t(const uint8_t *, size_t)> write) const override
    {
        return write((uint8_t *)&_size, sizeof(LogMessageHeader) - 8) + write(bytes, _size);
    }

    virtual size_t deserialize(std::function<size_t(uint8_t *, size_t)> read) override
    {
        return read((uint8_t *)&_size, sizeof(LogMessageHeader) - 8) + read(bytes, _size);
    }
};