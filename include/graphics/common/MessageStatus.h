#pragma once

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
} // namespace MessageStatus
