#include "graphics/common/ChatHistory.h"

#include <algorithm>

const ChatHistory::Entry &ChatHistory::append(Entry &&entry, bool shown)
{
    // hiding is only valid while nothing is shown, otherwise the shown range would no longer match the bubbles
    bool hide = !shown && shownCount() == 0;
    entries.push_back(std::move(entry));
    if (hide) {
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
