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
constexpr Presentation noChannel{"No channel selected", Tone::RetryableFailure, true};
constexpr Presentation genericEncryptedSendFailure{"Could not send encrypted message", Tone::RetryableFailure, true};
constexpr Presentation recipientKeyUnavailable{"Recipient key unavailable", Tone::RetryableFailure, true};
constexpr Presentation recipientNeedsSenderKey{"Recipient needs your key", Tone::RetryableFailure, true};
constexpr Presentation messageTooLarge{"Message is too large to send", Tone::PermanentFailure, false};
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
} // namespace MessageStatus
