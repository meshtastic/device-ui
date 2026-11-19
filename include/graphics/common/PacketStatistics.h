#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * Per-node packet counters shown in the statistics table.
 * Bounded container; removing a node keeps the vector capacity so no heap is churned.
 */
class PacketStatistics
{
  public:
    struct Entry {
        uint32_t id = 0;
        uint16_t row = 0; // table row currently showing this node (0 = not shown yet)
        uint16_t tel = 0;
        uint16_t pos = 0;
        uint16_t inf = 0;
        uint16_t trc = 0;
        uint16_t txt = 0;
        uint16_t nbr = 0;
        uint32_t sum = 0;

        Entry &operator+=(const Entry &rhs)
        {
            tel += rhs.tel;
            pos += rhs.pos;
            inf += rhs.inf;
            trc += rhs.trc;
            txt += rhs.txt;
            nbr += rhs.nbr;
            sum += 1;
            return *this;
        }
    };

    explicit PacketStatistics(size_t maxEntries) : maxEntries(maxEntries) {}

    /**
     * Account one packet for delta.id; returns false if the node is unknown and the table is full.
     * Entries are kept sorted by sum (descending), stable for equal sums.
     */
    bool add(const Entry &delta)
    {
        auto it = find(delta.id);
        if (it == entries.end()) {
            if (entries.size() >= maxEntries)
                return false;
            Entry e = delta;
            e.row = entries.size() + 1;
            e.sum = 1;
            entries.push_back(e);
        } else {
            *it += delta;
        }
        std::stable_sort(entries.begin(), entries.end(), [](const Entry &a, const Entry &b) { return a.sum > b.sum; });
        return true;
    }

    // drop a node; returns its former index + 1 (table row), or 0 if unknown
    size_t remove(uint32_t id)
    {
        auto it = find(id);
        if (it == entries.end())
            return 0;
        size_t row = (it - entries.begin()) + 1;
        entries.erase(it); // capacity is retained
        return row;
    }

    void clear(void) { entries.clear(); }
    size_t size(void) const { return entries.size(); }
    size_t capacity(void) const { return entries.capacity(); }
    size_t limit(void) const { return maxEntries; }
    std::vector<Entry> &all(void) { return entries; }

  private:
    std::vector<Entry>::iterator find(uint32_t id)
    {
        return std::find_if(entries.begin(), entries.end(), [id](const Entry &e) { return e.id == id; });
    }

    size_t maxEntries;
    std::vector<Entry> entries;
};
