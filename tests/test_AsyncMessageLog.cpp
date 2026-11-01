#include "graphics/common/ResponseHandler.h"
#include "util/AsyncMessageLog.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <doctest/doctest.h>
#include <mutex>
#include <vector>

namespace
{
std::unique_ptr<LogMessageEnv> message(uint32_t from = 1, uint32_t to = 2, uint8_t channel = 0)
{
    return std::make_unique<LogMessageEnv>(from, to, channel, 123, LogMessage::eDefault, false, 4,
                                           reinterpret_cast<const uint8_t *>("text"));
}

std::unique_ptr<LogMessageEnv> roundTrip(const LogMessageEnv &entry)
{
    std::vector<uint8_t> bytes;
    entry.serialize([&](const uint8_t *data, size_t length) {
        bytes.insert(bytes.end(), data, data + length);
        return length;
    });
    size_t position = 0;
    auto restored = std::make_unique<LogMessageEnv>();
    restored->deserialize([&](uint8_t *data, size_t length) {
        const size_t available = std::min(length, bytes.size() - position);
        memcpy(data, bytes.data() + position, available);
        position += available;
        return available;
    });
    return restored;
}

struct Recorder {
    std::mutex mutex;
    std::condition_variable changed;
    std::vector<std::unique_ptr<LogMessageEnv>> entries;

    bool write(const LogMessageEnv &entry)
    {
        std::lock_guard<std::mutex> guard(mutex);
        entries.push_back(roundTrip(entry));
        changed.notify_all();
        return true;
    }

    bool waitFor(size_t count)
    {
        std::unique_lock<std::mutex> guard(mutex);
        return changed.wait_for(guard, std::chrono::seconds(2), [&] { return entries.size() >= count; });
    }
};
} // namespace

TEST_CASE("outgoing delivery results are stored and restored in message order")
{
    Recorder recorder;
    AsyncMessageLog log([&](const LogMessageEnv &entry) { return recorder.write(entry); }, [] { return true; });
    REQUIRE(log.append(message(), 10, 60000));
    REQUIRE(log.append(message(), 11, 60000));
    REQUIRE(log.append(message(), 12, 0));
    log.setStatus(10, LogMessage::eHeard, false);
    log.setStatus(10, LogMessage::eAcked, true);
    log.setStatus(11, LogMessage::eFailed, true);
    REQUIRE(log.start());
    REQUIRE(recorder.waitFor(3));
    REQUIRE(log.prepareShutdown(1000));
    CHECK(recorder.entries[0]->status == LogMessage::eAcked);
    CHECK(recorder.entries[1]->status == LogMessage::eFailed);
    CHECK(recorder.entries[2]->status == LogMessage::eNoResponse);
    CHECK(std::strcmp(reinterpret_cast<const char *>(recorder.entries[0]->bytes), "text") == 0);
    CHECK_FALSE(log.append(message()));
}

TEST_CASE("delivery timeout is rollover safe and preserves a heard response")
{
    Recorder recorder;
    std::atomic<uint32_t> clock{UINT32_MAX - 10};
    AsyncMessageLog log([&](const LogMessageEnv &entry) { return recorder.write(entry); }, [] { return true; },
                        [&] { return clock.load(); });
    REQUIRE(log.append(message(), 42, 20));
    log.setStatus(42, LogMessage::eHeard, false);
    log.setStatus(42, LogMessage::eNoResponse, false);
    clock = 9;
    REQUIRE(log.start());
    REQUIRE(recorder.waitFor(1));
    REQUIRE(log.prepareShutdown(1000));
    CHECK(recorder.entries[0]->status == LogMessage::eHeard);
}

TEST_CASE("broadcast heard responses settle without waiting for a destination acknowledgement")
{
    Recorder recorder;
    AsyncMessageLog log([&](const LogMessageEnv &entry) { return recorder.write(entry); }, [] { return true; });
    REQUIRE(log.append(message(1, UINT32_MAX), 42, 60000));
    log.setStatus(42, LogMessage::eHeard, false);
    REQUIRE(log.start());
    REQUIRE(recorder.waitFor(1));
    REQUIRE(log.prepareShutdown(1000));
    CHECK(recorder.entries[0]->status == LogMessage::eAcked);
}

TEST_CASE("shutdown saves unresolved status honestly and does not depend on the UI task")
{
    Recorder recorder;
    AsyncMessageLog log([&](const LogMessageEnv &entry) { return recorder.write(entry); }, [] { return true; });
    REQUIRE(log.append(message(), 42, 60000));
    REQUIRE(log.start());
    REQUIRE(log.prepareShutdown(1000));
    REQUIRE(recorder.entries.size() == 1);
    CHECK(recorder.entries[0]->status == LogMessage::eDefault);
}

TEST_CASE("chat deletion cancels unresolved entries before writing its tombstone")
{
    Recorder recorder;
    AsyncMessageLog log([&](const LogMessageEnv &entry) { return recorder.write(entry); }, [] { return true; });
    REQUIRE(log.append(message(), 42, 60000));
    REQUIRE(log.append(message(2, 1, 3)));
    REQUIRE(log.remove(1, 2, 0));
    REQUIRE(log.start());
    REQUIRE(log.prepareShutdown(1000));
    REQUIRE(recorder.entries.size() == 1);
    CHECK(recorder.entries[0]->trashFlag);
    CHECK(recorder.entries[0]->length() == 0);
}

TEST_CASE("broadcast deletion cancels queued messages from every sender on that channel")
{
    Recorder recorder;
    AsyncMessageLog log([&](const LogMessageEnv &entry) { return recorder.write(entry); }, [] { return true; });
    REQUIRE(log.append(message(1, UINT32_MAX, 3), 42, 60000));
    REQUIRE(log.append(message(99, UINT32_MAX, 3)));
    REQUIRE(log.remove(1, UINT32_MAX, 3));
    REQUIRE(log.start());
    REQUIRE(log.prepareShutdown(1000));
    REQUIRE(recorder.entries.size() == 1);
    CHECK(recorder.entries[0]->trashFlag);
}

TEST_CASE("clear history discards pending messages and precedes subsequent messages")
{
    Recorder recorder;
    bool cleared = false;
    bool writtenAfterClear = false;
    AsyncMessageLog log(
        [&](const LogMessageEnv &entry) {
            writtenAfterClear = cleared;
            return recorder.write(entry);
        },
        [&] {
            cleared = true;
            return true;
        });
    REQUIRE(log.append(message(), 42, 60000));
    REQUIRE(log.remove(0, 0, 0));
    REQUIRE(log.append(message()));
    REQUIRE(log.start());
    REQUIRE(log.prepareShutdown(1000));
    CHECK(cleared);
    CHECK(writtenAfterClear);
    CHECK(recorder.entries.size() == 1);
}

TEST_CASE("a failed write is retained and a shutdown timeout is not reported as success")
{
    Recorder recorder;
    std::atomic<bool> available{false};
    std::mutex mutex;
    std::condition_variable attempted;
    bool failed = false;
    AsyncMessageLog log(
        [&](const LogMessageEnv &entry) {
            if (available)
                return recorder.write(entry);
            std::lock_guard<std::mutex> guard(mutex);
            failed = true;
            attempted.notify_all();
            return false;
        },
        [] { return true; });
    REQUIRE(log.append(message()));
    REQUIRE(log.start());
    {
        std::unique_lock<std::mutex> guard(mutex);
        REQUIRE(attempted.wait_for(guard, std::chrono::seconds(2), [&] { return failed; }));
    }
    CHECK_FALSE(log.prepareShutdown(0));
    available = true;
    REQUIRE(log.prepareShutdown(1000));
    CHECK(recorder.entries.size() == 1);
}

TEST_CASE("truncated persisted messages are not replayed as complete records")
{
    auto entry = message();
    std::vector<uint8_t> bytes;
    entry->serialize([&](const uint8_t *data, size_t length) {
        bytes.insert(bytes.end(), data, data + length);
        return length;
    });
    bytes.pop_back();
    size_t position = 0;
    LogMessageEnv restored;
    CHECK(restored.deserialize([&](uint8_t *data, size_t length) {
        const size_t available = std::min(length, bytes.size() - position);
        memcpy(data, bytes.data() + position, available);
        position += available;
        return available;
    }) == 0);
}

TEST_CASE("shutdown waits for a write already in progress and its deadline only bounds the wait")
{
    std::mutex mutex;
    std::condition_variable changed;
    bool entered = false;
    bool release = false;
    AsyncMessageLog log(
        [&](const LogMessageEnv &) {
            std::unique_lock<std::mutex> guard(mutex);
            entered = true;
            changed.notify_all();
            changed.wait(guard, [&] { return release; });
            return true;
        },
        [] { return true; });
    REQUIRE(log.append(message()));
    REQUIRE(log.start());
    {
        std::unique_lock<std::mutex> guard(mutex);
        CHECK(changed.wait_for(guard, std::chrono::seconds(2), [&] { return entered; }));
    }
    CHECK_FALSE(log.prepareShutdown(0));
    {
        std::lock_guard<std::mutex> guard(mutex);
        release = true;
        changed.notify_all();
    }
    CHECK(log.prepareShutdown(1000));
}

TEST_CASE("response timeout callbacks identify the packet rather than its destination")
{
    ResponseHandler requests(0);
    uint32_t expiredPacket = 0;
    const uint32_t packetId =
        requests.addRequest(2, ResponseHandler::TextMessageRequest, nullptr,
                            [&](const ResponseHandler::Request &request, ResponseHandler::EventType event, int32_t) {
                                if (event == ResponseHandler::timeout)
                                    expiredPacket = request.packetId;
                            });
    requests.task_handler();
    CHECK(expiredPacket == packetId);
}