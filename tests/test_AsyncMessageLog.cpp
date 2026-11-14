#include "graphics/common/MeshtasticView.h"
#include "graphics/common/ResponseHandler.h"
#include "graphics/common/ViewController.h"
#include "input/InputDriver.h"
#include "util/AsyncMessageLog.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <doctest/doctest.h>
#include <mutex>
#include <thread>
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

TEST_CASE("ViewController sleep delegates to client without shutting down message log")
{
    struct MockSleepClient : IClientBase {
        int16_t lastSleepPin = -1;
        bool sleepResult = true;
        bool disconnected = false;
        void init(void) override {}
        bool connect(void) override { return true; }
        bool disconnect(void) override
        {
            disconnected = true;
            return true;
        }
        bool isConnected(void) override { return !disconnected; }
        bool isStandalone(void) override { return false; }
        bool sleep(int16_t pin) override
        {
            lastSleepPin = pin;
            return sleepResult;
        }
        bool send(meshtastic_ToRadio &&) override { return true; }
        meshtastic_FromRadio receive(void) override { return meshtastic_FromRadio_init_zero; }
        void setNotifyCallback(NotifyCallback) override {}
    };

    struct TestableViewController : ViewController {
        void attachClient(IClientBase *c) { client = c; }
        bool appendMessage(std::unique_ptr<LogMessageEnv> entry, uint32_t requestId = 0, uint32_t timeoutMs = 0)
        {
            return messageLog.append(std::move(entry), requestId, timeoutMs);
        }
    };

    MockSleepClient mockClient;
    TestableViewController controller;

    CHECK_FALSE(controller.sleep(42));
    controller.attachClient(&mockClient);
    CHECK(controller.sleep(42));
    CHECK(mockClient.lastSleepPin == 42);
    CHECK(controller.appendMessage(message(1, 2, 0), 99, 60000));

    controller.stop();
    CHECK(mockClient.disconnected);
    CHECK_FALSE(controller.appendMessage(message(1, 2, 0)));
}

TEST_CASE("flush persists pending messages immediately while keeping log active")
{
    Recorder recorder;
    AsyncMessageLog log([&](const LogMessageEnv &entry) { return recorder.write(entry); }, [] { return true; });
    REQUIRE(log.append(message(1, 2, 0), 42, 60000));
    REQUIRE(log.start());

    // Flush before the 60s request timeout expires (e.g. when the screen goes dark)
    REQUIRE(log.flush(1000));
    REQUIRE(recorder.entries.size() == 1);
    CHECK(recorder.entries[0]->status == LogMessage::eDefault);

    // Late status callbacks for already-flushed items must be safe no-ops
    log.setStatus(42, LogMessage::eAcked, true);

    // Worker stays active and accepts new outgoing/incoming messages after wake
    REQUIRE(log.append(message(2, 1, 0), 43, 60000));
    log.setStatus(43, LogMessage::eAcked, true);
    REQUIRE(recorder.waitFor(2));
    REQUIRE(log.prepareShutdown(1000));
    CHECK(recorder.entries[1]->status == LogMessage::eAcked);
}

TEST_CASE("flush before start does not clear pending request ids and flush failure keeps accepting")
{
    std::atomic<bool> allowWrite{false};
    Recorder recorder;
    AsyncMessageLog log(
        [&](const LogMessageEnv &entry) {
            if (!allowWrite)
                return false;
            return recorder.write(entry);
        },
        [] { return true; });

    // Before start(): flush returns false without corrupting unresolved requestIds
    REQUIRE(log.append(message(1, 2, 0), 51, 60000));
    CHECK_FALSE(log.flush(100));
    log.setStatus(51, LogMessage::eAcked, true);

    REQUIRE(log.start());
    // Write failure returns false cleanly without disabling append()
    CHECK_FALSE(log.flush(150));
    REQUIRE(log.append(message(1, 2, 0), 52, 60000));

    allowWrite = true;
    log.setStatus(52, LogMessage::eFailed, true);
    REQUIRE(log.flush(1000));
    REQUIRE(recorder.entries.size() == 2);
    CHECK(recorder.entries[0]->status == LogMessage::eAcked);
    CHECK(recorder.entries[1]->status == LogMessage::eFailed);
}

TEST_CASE("flush ignores newer unresolved appends arriving concurrently after flush starts")
{
    Recorder recorder;
    std::mutex startMutex;
    std::condition_variable writeStarted;
    std::mutex releaseMutex;
    std::condition_variable releaseWrite;
    bool inFirstWrite = false;
    bool allowFirstWrite = false;
    std::atomic<int> writes{0};

    AsyncMessageLog log(
        [&](const LogMessageEnv &entry) {
            if (writes.fetch_add(1) == 0) {
                {
                    std::lock_guard<std::mutex> guard(startMutex);
                    inFirstWrite = true;
                }
                writeStarted.notify_all();
                std::unique_lock<std::mutex> guard(releaseMutex);
                releaseWrite.wait(guard, [&] { return allowFirstWrite; });
            }
            return recorder.write(entry);
        },
        [] { return true; });

    REQUIRE(log.append(message(1, 2, 0), 70, 60000));
    REQUIRE(log.start());

    std::atomic<bool> flushResult{false};
    std::thread flusher([&] { flushResult = log.flush(200); });
    {
        std::unique_lock<std::mutex> guard(startMutex);
        REQUIRE(writeStarted.wait_for(guard, std::chrono::seconds(2), [&] { return inFirstWrite; }));
    }

    // Appending a new 60s unresolved DM mid-flush must not force the active flush to time out
    REQUIRE(log.append(message(1, 3, 0), 71, 60000));
    {
        std::lock_guard<std::mutex> guard(releaseMutex);
        allowFirstWrite = true;
    }
    releaseWrite.notify_all();
    flusher.join();

    CHECK(flushResult.load());
    CHECK(recorder.entries.size() == 1);
    CHECK(log.prepareShutdown(1000));
    CHECK(recorder.entries.size() == 2);
}

TEST_CASE("MeshtasticView and ViewController sleep flush pending messages without blocking wake or retry backoff")
{
    struct MockSleepClient : IClientBase {
        int16_t lastSleepPin = -1;
        int sleepCalls = 0;
        bool sleepResult = false;
        bool disconnected = false;
        void init(void) override {}
        bool connect(void) override { return true; }
        bool disconnect(void) override
        {
            disconnected = true;
            return true;
        }
        bool isConnected(void) override { return !disconnected; }
        bool isStandalone(void) override { return false; }
        bool sleep(int16_t pin) override
        {
            lastSleepPin = pin;
            sleepCalls++;
            return sleepResult;
        }
        bool send(meshtastic_ToRadio &&) override { return true; }
        meshtastic_FromRadio receive(void) override { return meshtastic_FromRadio_init_zero; }
        void setNotifyCallback(NotifyCallback) override {}
    };

    struct TestableViewController : ViewController {
        TestableViewController(AsyncMessageLog::Writer writer, AsyncMessageLog::Clear clear)
            : ViewController(std::move(writer), std::move(clear))
        {
        }
        void attachClient(IClientBase *c) { client = c; }
        bool startLog() { return messageLog.start(); }
        bool appendMessage(std::unique_ptr<LogMessageEnv> entry, uint32_t requestId = 0, uint32_t timeoutMs = 0)
        {
            return messageLog.append(std::move(entry), requestId, timeoutMs);
        }
        bool inBackoff() const { return sleepFlushBackoff.load(); }
    };

    struct TrackingInputDriver : InputDriver {
        bool sleepPrepared = false;
        ~TrackingInputDriver() override { driver = nullptr; }
        void prepareSleep(void) override { sleepPrepared = true; }
    };

    struct TestableMeshtasticView : MeshtasticView {
        TestableMeshtasticView(ViewController *ctrl) : MeshtasticView(nullptr, nullptr, ctrl) {}
    };

    auto *trackingInput = new TrackingInputDriver;
    (void)trackingInput;

    Recorder recorder;
    std::atomic<bool> allowWrite{true};
    TestableViewController controller(
        [&](const LogMessageEnv &entry) {
            if (!allowWrite)
                return false;
            return recorder.write(entry);
        },
        [] { return true; });
    MockSleepClient mockClient;
    controller.attachClient(&mockClient);
    REQUIRE(controller.startLog());

    TestableMeshtasticView view(&controller);

    // 1. Display powersave via screenSaving(true) flushes an unresolved DM as eDefault
    REQUIRE(controller.appendMessage(message(1, 2, 0), 101, 60000));
    view.screenSaving(true);
    CHECK(recorder.entries.size() == 1);
    CHECK(recorder.entries[0]->status == LogMessage::eDefault);

    // 2. Display powersave via blankScreen(true) flushes another unresolved DM
    REQUIRE(controller.appendMessage(message(1, 3, 0), 102, 60000));
    view.blankScreen(true);
    CHECK(recorder.entries.size() == 2);

    // 3. Hardware light-sleep hook prepareSleep() delegates input sleep and flushes
    REQUIRE(controller.appendMessage(message(1, 4, 0), 103, 60000));
    view.prepareSleep();
    CHECK(trackingInput->sleepPrepared);
    CHECK(recorder.entries.size() == 3);

    // 4. Subsequent wake / send continues accepting without "shutting down" error
    view.screenSaving(false);
    REQUIRE(controller.appendMessage(message(1, 5, 0), 104, 60000));

    // 5. Polling ViewController::sleep() flushes queued message and enters backoff on write failure
    mockClient.sleepResult = true;
    CHECK(controller.sleep(42));
    CHECK(mockClient.lastSleepPin == 42);
    CHECK(mockClient.sleepCalls == 1);
    CHECK(recorder.entries.size() == 4);
    CHECK_FALSE(controller.inBackoff());

    mockClient.sleepResult = false;
    allowWrite = false;
    REQUIRE(controller.appendMessage(message(1, 6, 0), 105, 60000));
    CHECK_FALSE(controller.flushMessageLog(100));
    CHECK(controller.inBackoff());

    // Subsequent poll calls sleep() without stalling inside flush while in backoff
    CHECK_FALSE(controller.sleep(42));
    CHECK(mockClient.sleepCalls == 2);

    allowWrite = true;
    controller.stop();
    CHECK(mockClient.disconnected);
    CHECK(recorder.entries.size() == 5);
}