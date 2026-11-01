#pragma once

#include "graphics/common/NodeStore.h"
#include "util/LogMessage.h"
#include <algorithm>
#include <string>
#include <vector>

class ChatHistory
{
  public:
    struct Entry {
        std::basic_string<char, std::char_traits<char>, NodeStoreAllocator<char>> text;
        uint32_t node = 0;
        uint32_t requestId = 0;
        LogMessage::MsgStatus status = LogMessage::eNone;
        bool sent = false;
    };
    static constexpr size_t window = 100;
    static constexpr size_t step = 25;

    void append(Entry entry, bool follow)
    {
        entries.push_back(std::move(entry));
        if (follow)
            latest();
    }
    void latest()
    {
        end = entries.size();
        begin = end > window ? end - window : 0;
    }
    bool older()
    {
        if (!begin)
            return false;
        begin -= std::min(begin, step);
        end = std::min(entries.size(), begin + window);
        return true;
    }
    bool newer()
    {
        if (end == entries.size())
            return false;
        end += std::min(entries.size() - end, step);
        begin = end > window ? end - window : 0;
        return true;
    }
    void setStatus(uint32_t requestId, LogMessage::MsgStatus status)
    {
        if (!requestId)
            return;
        for (auto position = entries.rbegin(); position != entries.rend(); ++position) {
            if (position->sent && position->requestId == requestId) {
                if (status != LogMessage::eNoResponse || position->status == LogMessage::eNone)
                    position->status = status;
                return;
            }
        }
    }
    const Entry &at(size_t index) const { return entries.at(index); }
    size_t size() const { return entries.size(); }
    size_t first() const { return begin; }
    size_t last() const { return end; }
    bool atLatest() const { return end == entries.size(); }
    int32_t scrollY = 0;
    bool opened = false;

  private:
    std::vector<Entry, NodeStoreAllocator<Entry>> entries;
    size_t begin = 0;
    size_t end = 0;
};