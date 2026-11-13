#include "graphics/common/PacketStatistics.h"
#include <doctest/doctest.h>

namespace
{
PacketStatistics::Entry text(uint32_t id)
{
    PacketStatistics::Entry e;
    e.id = id;
    e.txt = 1;
    return e;
}
} // namespace

TEST_CASE("PacketStatistics accumulates and sorts by packet count")
{
    PacketStatistics stats(10);
    CHECK(stats.add(text(1)));
    CHECK(stats.add(text(2)));
    CHECK(stats.add(text(2)));
    REQUIRE(stats.size() == 2);
    CHECK(stats.all()[0].id == 2);
    CHECK(stats.all()[0].sum == 2);
    CHECK(stats.all()[0].txt == 2);
    CHECK(stats.all()[1].id == 1);
    CHECK(stats.all()[1].row == 1); // row bookkeeping is left to the renderer
}

TEST_CASE("PacketStatistics enforces the node limit")
{
    PacketStatistics stats(2);
    CHECK(stats.add(text(1)));
    CHECK(stats.add(text(2)));
    CHECK_FALSE(stats.add(text(3)));
    CHECK(stats.add(text(1))); // known nodes still update
    CHECK(stats.size() == 2);
}

TEST_CASE("PacketStatistics removes purged nodes without releasing capacity")
{
    PacketStatistics stats(5);
    for (uint32_t id = 1; id <= 5; id++)
        stats.add(text(id));
    stats.add(text(3)); // node 3 leads the table
    size_t capacity = stats.capacity();
    REQUIRE(stats.all()[0].id == 3);
    CHECK_FALSE(stats.add(text(6))); // table is full

    CHECK(stats.remove(3) == 1); // reports the sorted row the node occupied
    CHECK(stats.remove(3) == 0);
    CHECK(stats.size() == 4);
    CHECK(stats.capacity() == capacity);
    for (auto &e : stats.all())
        CHECK(e.id != 3);

    CHECK(stats.add(text(6))); // slot is reused
    CHECK(stats.size() == 5);
    CHECK(stats.capacity() == capacity);

    stats.clear();
    CHECK(stats.size() == 0);
    CHECK(stats.capacity() == capacity);
}
