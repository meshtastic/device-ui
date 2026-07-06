#pragma once

#include "util/LogMessage.h"
#include <optional>

namespace MessageStatus
{
enum class State {
    Sending,
    ChannelImplicitAck,
    DirectImplicitAck,
    ExplicitAck,
    NoAck,
    NoChannel,
    GenericEncryptedSendFailure,
    RecipientKeyUnavailable,
    RecipientNeedsSenderKey,
    MessageTooLarge,
};

enum class Tone {
    Pending,
    Success,
    Warning,
    RetryableFailure,
    PermanentFailure,
};

struct Presentation {
    const char *text;
    Tone tone;
    bool retryable;
};

const Presentation &presentation(State state);
State deliveredState(bool channelMessage, bool explicitAck);
LogMessage::MsgStatus logStatusForState(State state);
uint32_t persistedLogState(State state);
std::optional<State> stateFromPersistedLogState(uint32_t state);
std::optional<State> inlineStateForLogStatus(LogMessage::MsgStatus status, bool livePending);
std::optional<State> inlineStateForLogStatus(LogMessage::MsgStatus status, uint32_t persistedState, bool livePending);
} // namespace MessageStatus
