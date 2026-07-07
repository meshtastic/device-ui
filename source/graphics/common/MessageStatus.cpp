#include "graphics/common/MessageStatus.h"

namespace MessageStatus
{
namespace
{
constexpr Presentation sending{"Sending...", Tone::Pending, false};
constexpr Presentation channelImplicitAck{"Delivered to mesh", Tone::Success, false};
constexpr Presentation directImplicitAck{"Relayed, not confirmed by recipient", Tone::Warning, true};
constexpr Presentation explicitAck{"Delivered to recipient", Tone::Success, false};
constexpr Presentation noAck{"Failed to deliver to mesh", Tone::RetryableFailure, true};
constexpr Presentation noChannel{"Channel/key mismatch", Tone::RetryableFailure, true};
constexpr Presentation genericEncryptedSendFailure{"Could not send encrypted message", Tone::RetryableFailure, true};
constexpr Presentation recipientKeyUnavailable{"Recipient key unavailable", Tone::RetryableFailure, true};
constexpr Presentation recipientNeedsSenderKey{"Recipient needs your key", Tone::RetryableFailure, true};
constexpr Presentation messageTooLarge{"Message is too large to send", Tone::PermanentFailure, false};
constexpr uint32_t persistedLogStateMagic = 0x4d535400; // "MST" + state byte
constexpr uint32_t persistedLogStateMask = 0xffffff00;
} // namespace

const Presentation &presentation(State state)
{
    switch (state) {
    case State::Sending:
        return sending;
    case State::ChannelImplicitAck:
        return channelImplicitAck;
    case State::DirectImplicitAck:
        return directImplicitAck;
    case State::ExplicitAck:
        return explicitAck;
    case State::NoAck:
        return noAck;
    case State::NoChannel:
        return noChannel;
    case State::GenericEncryptedSendFailure:
        return genericEncryptedSendFailure;
    case State::RecipientKeyUnavailable:
        return recipientKeyUnavailable;
    case State::RecipientNeedsSenderKey:
        return recipientNeedsSenderKey;
    case State::MessageTooLarge:
        return messageTooLarge;
    }
    return noAck;
}

State deliveredState(bool channelMessage, bool explicitAck)
{
    if (channelMessage)
        return State::ChannelImplicitAck;
    return explicitAck ? State::ExplicitAck : State::DirectImplicitAck;
}

bool isImplicitDelivery(State state)
{
    return state == State::ChannelImplicitAck || state == State::DirectImplicitAck;
}

State preserveImplicitDelivery(std::optional<State> pendingStatus, State fallbackStatus)
{
    if (pendingStatus && isImplicitDelivery(*pendingStatus))
        return *pendingStatus;
    return fallbackStatus;
}

LogMessage::MsgStatus logStatusForState(State state)
{
    switch (state) {
    case State::Sending:
        return LogMessage::eNone;
    case State::ChannelImplicitAck:
    case State::DirectImplicitAck:
        return LogMessage::eHeard;
    case State::ExplicitAck:
        return LogMessage::eAcked;
    case State::NoAck:
        return LogMessage::eNoResponse;
    case State::NoChannel:
    case State::GenericEncryptedSendFailure:
    case State::RecipientKeyUnavailable:
    case State::RecipientNeedsSenderKey:
    case State::MessageTooLarge:
        return LogMessage::eFailed;
    }
    return LogMessage::eFailed;
}

uint32_t persistedLogState(State state)
{
    return persistedLogStateMagic | (static_cast<uint32_t>(state) + 1);
}

std::optional<State> stateFromPersistedLogState(uint32_t state)
{
    if ((state & persistedLogStateMask) != persistedLogStateMagic)
        return std::nullopt;

    const uint32_t value = state & ~persistedLogStateMask;
    if (value == 0 || value > static_cast<uint32_t>(State::MessageTooLarge) + 1)
        return std::nullopt;

    return static_cast<State>(value - 1);
}

std::optional<State> inlineStateForLogStatus(LogMessage::MsgStatus status, bool livePending)
{
    return inlineStateForLogStatus(status, 0, livePending);
}

std::optional<State> inlineStateForLogStatus(LogMessage::MsgStatus status, uint32_t persistedState, bool livePending)
{
    switch (status) {
    case LogMessage::eNone:
        return livePending ? std::optional<State>(State::Sending) : std::nullopt;
    case LogMessage::eHeard:
    case LogMessage::eAcked:
    case LogMessage::eNoResponse:
    case LogMessage::eFailed:
        if (const auto state = stateFromPersistedLogState(persistedState))
            return state;
        break;
    default:
        break;
    }

    switch (status) {
    case LogMessage::eHeard:
        return State::DirectImplicitAck;
    case LogMessage::eAcked:
        return State::ExplicitAck;
    case LogMessage::eNoResponse:
    case LogMessage::eFailed:
        return State::NoAck;
    default:
        return std::nullopt;
    }
}
} // namespace MessageStatus
