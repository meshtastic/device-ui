#pragma once

#include "graphics/common/NodeStore.h"
#include "util/LogMessage.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Every message of one chat, of which only the newest ones are backed by LVGL bubbles.
 *
 * Each LVGL layout pass walks the whole screen tree, hidden chats included, so a bubble per
 * stored message makes every send, receive and chat switch cost in proportion to the total
 * history. The bubbles shown are the contiguous range [firstShown(), size()) of the entries;
 * older entries are kept as text only (in the LVGL heap) and turned into bubbles on demand.
 */
class ChatHistory
{
  public:
    using Text = std::basic_string<char, std::char_traits<char>, NodeStoreAllocator<char>>;

    struct Entry {
        Text text;                                        // formatted bubble text, including timestamp
        uint32_t nodeNum = 0;                             // sender of a received message
        uint32_t requestId = 0;                           // request id of a sent message, 0 if unknown
        LogMessage::MsgStatus status = LogMessage::eNone; // delivery status of a sent message
        bool sent = false;                                // own message
    };

    // append newest entry; shown entries get a bubble, others only extend the hidden older range
    // (only allowed while nothing is shown)
    const Entry &append(Entry &&entry, bool shown);
    // mark up to count of the newest hidden entries as shown; returns the number revealed,
    // which are then at [firstShown(), firstShown() + n)
    size_t revealOlder(size_t count);
    // hide the oldest shown entries so that at most window remain; returns the number hidden
    size_t trimTo(size_t window);
    // update the status of the sent entry with requestId
    void setStatus(uint32_t requestId, LogMessage::MsgStatus status);

    const Entry &at(size_t index) const { return entries[index]; }
    size_t size() const { return entries.size(); }
    size_t firstShown() const { return shownBegin; }
    size_t shownCount() const { return entries.size() - shownBegin; }
    size_t hiddenOlderCount() const { return shownBegin; }

  private:
    std::vector<Entry, NodeStoreAllocator<Entry>> entries;
    size_t shownBegin = 0;
};
