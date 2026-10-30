#include "graphics/common/ChatHistory.h"
#include <doctest/doctest.h>

namespace
{
ChatHistory historyWithMessages(size_t count)
{
    ChatHistory history;
    for (size_t index = 0; index < count; ++index) {
        ChatHistory::Entry entry;
        entry.text = std::to_string(index).c_str();
        history.append(std::move(entry), true);
    }
    return history;
}
} // namespace

TEST_CASE("chat pages stay bounded while traversing the entire history in both directions")
{
    auto history = historyWithMessages(1000);
    CHECK(history.first() == 900);
    CHECK(history.last() == 1000);
    while (history.older())
        CHECK(history.last() - history.first() <= ChatHistory::window);
    CHECK(history.first() == 0);
    CHECK(history.at(0).text == "0");
    while (history.newer())
        CHECK(history.last() - history.first() <= ChatHistory::window);
    CHECK(history.last() == 1000);
    CHECK(history.size() == 1000);
}

TEST_CASE("incoming messages preserve a historical page and its reading bookmark")
{
    auto history = historyWithMessages(250);
    REQUIRE(history.older());
    const size_t first = history.first();
    const size_t last = history.last();
    history.scrollY = 37;
    history.opened = true;
    history.append(ChatHistory::Entry{}, false);
    CHECK(history.first() == first);
    CHECK(history.last() == last);
    CHECK(history.scrollY == 37);
    CHECK(history.opened);
    while (history.newer()) {
    }
    CHECK(history.last() == 251);
    CHECK(history.last() - history.first() == ChatHistory::window);
}

TEST_CASE("chat delivery status survives paging and timeout does not downgrade a response")
{
    ChatHistory history;
    ChatHistory::Entry entry;
    entry.sent = true;
    entry.requestId = 42;
    history.append(std::move(entry), true);
    for (size_t index = 0; index < 250; ++index)
        history.append(ChatHistory::Entry{}, true);
    history.setStatus(42, LogMessage::eHeard);
    history.setStatus(42, LogMessage::eNoResponse);
    CHECK(history.at(0).status == LogMessage::eHeard);
    history.setStatus(42, LogMessage::eAcked);
    while (history.older()) {
    }
    CHECK(history.at(0).status == LogMessage::eAcked);
    history.setStatus(0, LogMessage::eFailed);
    CHECK(history.at(1).status == LogMessage::eNone);
}

TEST_CASE("small chat pages and partial page boundaries retain every message")
{
    auto history = historyWithMessages(103);
    CHECK(history.first() == 3);
    REQUIRE(history.older());
    CHECK(history.first() == 0);
    CHECK(history.last() == 100);
    REQUIRE(history.newer());
    CHECK(history.first() == 3);
    CHECK(history.last() == 103);
    auto small = historyWithMessages(3);
    CHECK_FALSE(small.older());
    CHECK_FALSE(small.newer());
    CHECK(small.last() == 3);
}