#include "graphics/common/MessageStatus.h"

namespace MessageStatus
{
namespace
{
constexpr Presentation sending{"Sending...", nullptr, Tone::Pending, false};
constexpr Presentation channelImplicitAck{"Delivered to mesh", nullptr, Tone::Success, false};
constexpr Presentation directImplicitAck{"Relayed, not confirmed by recipient",
                                         "A node relayed this message, but the recipient has not confirmed it.", Tone::Warning,
                                         true};
constexpr Presentation explicitAck{"Delivered to recipient", nullptr, Tone::Success, false};
constexpr Presentation noAck{"Failed to deliver to mesh",
                             "No node confirmed this message. Try again when you have better signal or more mesh coverage.",
                             Tone::RetryableFailure, true};
constexpr Presentation noChannel{"Channel/key mismatch",
                                 "The sender or recipient could not use a matching channel/key for this message.",
                                 Tone::RetryableFailure, true};
constexpr Presentation genericEncryptedSendFailure{"Could not send encrypted message",
                                                   "The radio could not send this as an encrypted message. Wait for key info "
                                                   "to sync, then try again.",
                                                   Tone::RetryableFailure, true};
constexpr Presentation recipientKeyUnavailable{
    "Recipient key unavailable",
    "Your node does not have the recipient's public key yet. Wait for node info to sync, then try again.", Tone::RetryableFailure,
    true};
constexpr Presentation recipientNeedsSenderKey{
    "Recipient needs your key",
    "The recipient does not know your public key yet. Your node may share its info automatically; try again after it syncs.",
    Tone::RetryableFailure, true};
constexpr Presentation messageTooLarge{"Message is too large to send", "Shorten the message and send it again.",
                                       Tone::PermanentFailure, false};
constexpr Presentation noRadioInterface{"No radio interface", "No usable radio interface was available to transmit this message.",
                                        Tone::RetryableFailure, true};
constexpr Presentation dutyCycleLimit{"Duty cycle limit", "The radio hit its airtime duty-cycle limit. Try again later.",
                                      Tone::RetryableFailure, true};
constexpr Presentation rateLimited{"Rate limited",
                                   "The app sent too quickly and the radio rate limited this request. Try again shortly.",
                                   Tone::RetryableFailure, true};
constexpr Presentation noAppResponse{"No app response", "The destination received the request, but no app or module responded.",
                                     Tone::RetryableFailure, true};
constexpr Presentation invalidRequest{"Invalid request", "The destination rejected this message request as invalid.",
                                      Tone::RetryableFailure, true};
constexpr Presentation notAuthorized{"Not authorized", "The destination refused this request.", Tone::RetryableFailure, true};
constexpr Presentation adminSessionExpired{"Admin session expired", "The admin session expired. Try the request again.",
                                           Tone::RetryableFailure, true};
constexpr Presentation adminKeyNotAuthorized{"Admin key not authorized", "The destination refused this admin key.",
                                             Tone::RetryableFailure, true};
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
    case State::NoRadioInterface:
        return noRadioInterface;
    case State::DutyCycleLimit:
        return dutyCycleLimit;
    case State::RateLimited:
        return rateLimited;
    case State::NoAppResponse:
        return noAppResponse;
    case State::InvalidRequest:
        return invalidRequest;
    case State::NotAuthorized:
        return notAuthorized;
    case State::AdminSessionExpired:
        return adminSessionExpired;
    case State::AdminKeyNotAuthorized:
        return adminKeyNotAuthorized;
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
    case State::NoRadioInterface:
    case State::DutyCycleLimit:
    case State::RateLimited:
    case State::NoAppResponse:
    case State::InvalidRequest:
    case State::NotAuthorized:
    case State::AdminSessionExpired:
    case State::AdminKeyNotAuthorized:
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
    if (value == 0 || value > static_cast<uint32_t>(State::AdminKeyNotAuthorized) + 1)
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
