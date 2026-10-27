#include "doctest.h"
#include "graphics/common/ChatHistory.h"

namespace
{

ChatHistory::Entry received(const char *text, uint32_t from = 0x1234)
{
    ChatHistory::Entry entry;
    entry.text = text;
    entry.nodeNum = from;
    return entry;
}

ChatHistory::Entry sent(const char *text, uint32_t requestId)
{
    ChatHistory::Entry entry;
    entry.text = text;
    entry.requestId = requestId;
    entry.sent = true;
    return entry;
}

ChatHistory restoredHistory(size_t count)
{
    ChatHistory history;
    for (size_t i = 0; i < count; i++) {
        history.append(received(std::to_string(i).c_str()), false);
    }
    return history;
}

} // namespace

TEST_CASE("ChatHistory keeps restored entries hidden until revealed")
{
    ChatHistory history = restoredHistory(250);
    CHECK(history.size() == 250);
    CHECK(history.shownCount() == 0);
    CHECK(history.hiddenOlderCount() == 250);

    SUBCASE("revealing the newest window")
    {
        CHECK(history.revealOlder(100) == 100);
        CHECK(history.firstShown() == 150);
        CHECK(history.at(history.firstShown()).text == "150");
        CHECK(history.hiddenOlderCount() == 150);
    }

    SUBCASE("revealing stops at the oldest entry")
    {
        history.revealOlder(100);
        CHECK(history.revealOlder(100) == 100);
        CHECK(history.revealOlder(100) == 50);
        CHECK(history.revealOlder(100) == 0);
        CHECK(history.firstShown() == 0);
        CHECK(history.shownCount() == 250);
    }
}

TEST_CASE("ChatHistory trims the shown range to the window")
{
    ChatHistory history = restoredHistory(250);
    history.revealOlder(100);

    SUBCASE("appending a shown entry grows the range until trimmed")
    {
        history.append(received("new"), true);
        CHECK(history.shownCount() == 101);
        CHECK(history.trimTo(100) == 1);
        CHECK(history.shownCount() == 100);
        CHECK(history.at(history.firstShown()).text == "151");
        CHECK(history.at(history.size() - 1).text == "new");
    }

    SUBCASE("trimming after loading older entries hides only the extra ones")
    {
        history.revealOlder(25);
        CHECK(history.trimTo(100) == 25);
        CHECK(history.firstShown() == 150);
        CHECK(history.trimTo(100) == 0);
    }
}

TEST_CASE("ChatHistory updates the status of sent entries")
{
    ChatHistory history;
    history.append(sent("a", 7), true);
    history.append(received("b"), true);
    history.append(sent("c", 9), true);

    history.setStatus(7, LogMessage::eAcked);
    history.setStatus(0, LogMessage::eFailed); // unknown request id is ignored
    history.setStatus(42, LogMessage::eFailed);

    CHECK(history.at(0).status == LogMessage::eAcked);
    CHECK(history.at(1).status == LogMessage::eNone);
    CHECK(history.at(2).status == LogMessage::eNone);
}
