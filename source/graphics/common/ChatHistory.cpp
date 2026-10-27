#include "graphics/common/ChatHistory.h"

#include <algorithm>
#include <cassert>

const ChatHistory::Entry &ChatHistory::append(Entry &&entry, bool shown)
{
    assert(shown || shownCount() == 0);
    entries.push_back(std::move(entry));
    if (!shown) {
        shownBegin = entries.size();
    }
    return entries.back();
}

size_t ChatHistory::revealOlder(size_t count)
{
    size_t revealed = std::min(count, shownBegin);
    shownBegin -= revealed;
    return revealed;
}

size_t ChatHistory::trimTo(size_t window)
{
    if (shownCount() <= window) {
        return 0;
    }
    size_t hidden = shownCount() - window;
    shownBegin += hidden;
    return hidden;
}

void ChatHistory::setStatus(uint32_t requestId, LogMessage::MsgStatus status)
{
    if (requestId == 0) {
        return;
    }
    for (auto it = entries.rbegin(); it != entries.rend(); ++it) {
        if (it->sent && it->requestId == requestId) {
            it->status = status;
            return;
        }
    }
}
