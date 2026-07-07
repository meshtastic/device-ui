#include "graphics/common/MessageStatus.h"
#include <doctest/doctest.h>
#include <optional>
#include <string>

using MessageStatus::State;
using MessageStatus::Tone;

TEST_CASE("MessageStatus canonical inline text")
{
    CHECK(MessageStatus::presentation(State::Sending).text == std::string("Sending..."));
    CHECK(MessageStatus::presentation(State::ChannelImplicitAck).text == std::string("Delivered to mesh"));
    CHECK(MessageStatus::presentation(State::DirectImplicitAck).text ==
          std::string("Relayed, not confirmed by recipient"));
    CHECK(MessageStatus::presentation(State::ExplicitAck).text == std::string("Delivered to recipient"));
    CHECK(MessageStatus::presentation(State::NoAck).text == std::string("Failed to deliver to mesh"));
    CHECK(MessageStatus::presentation(State::NoChannel).text == std::string("Channel/key mismatch"));
    CHECK(MessageStatus::presentation(State::GenericEncryptedSendFailure).text ==
          std::string("Could not send encrypted message"));
    CHECK(MessageStatus::presentation(State::RecipientKeyUnavailable).text == std::string("Recipient key unavailable"));
    CHECK(MessageStatus::presentation(State::RecipientNeedsSenderKey).text == std::string("Recipient needs your key"));
    CHECK(MessageStatus::presentation(State::MessageTooLarge).text == std::string("Message is too large to send"));
}

TEST_CASE("MessageStatus distinguishes retryable and permanent failures")
{
    CHECK(MessageStatus::presentation(State::NoAck).retryable);
    CHECK(MessageStatus::presentation(State::NoChannel).retryable);
    CHECK(MessageStatus::presentation(State::RecipientKeyUnavailable).retryable);
    CHECK(MessageStatus::presentation(State::RecipientNeedsSenderKey).retryable);
    CHECK_FALSE(MessageStatus::presentation(State::MessageTooLarge).retryable);

    CHECK(MessageStatus::presentation(State::NoAck).tone == Tone::RetryableFailure);
    CHECK(MessageStatus::presentation(State::MessageTooLarge).tone == Tone::PermanentFailure);
}

TEST_CASE("MessageStatus maps delivery context")
{
    CHECK(MessageStatus::deliveredState(true, false) == State::ChannelImplicitAck);
    CHECK(MessageStatus::deliveredState(true, true) == State::ChannelImplicitAck);
    CHECK(MessageStatus::deliveredState(false, false) == State::DirectImplicitAck);
    CHECK(MessageStatus::deliveredState(false, true) == State::ExplicitAck);

    CHECK(MessageStatus::isImplicitDelivery(State::ChannelImplicitAck));
    CHECK(MessageStatus::isImplicitDelivery(State::DirectImplicitAck));
    CHECK_FALSE(MessageStatus::isImplicitDelivery(State::ExplicitAck));
    CHECK_FALSE(MessageStatus::isImplicitDelivery(State::NoAck));

    CHECK(MessageStatus::preserveImplicitDelivery(State::ChannelImplicitAck, State::NoAck) == State::ChannelImplicitAck);
    CHECK(MessageStatus::preserveImplicitDelivery(State::DirectImplicitAck, State::NoAck) == State::DirectImplicitAck);
    CHECK(MessageStatus::preserveImplicitDelivery(State::ExplicitAck, State::NoAck) == State::NoAck);
    CHECK(MessageStatus::preserveImplicitDelivery(std::nullopt, State::NoAck) == State::NoAck);
}

TEST_CASE("MessageStatus maps log status only when inline status is meaningful")
{
    CHECK(MessageStatus::inlineStateForLogStatus(LogMessage::eNone, true) == State::Sending);
    CHECK_FALSE(MessageStatus::inlineStateForLogStatus(LogMessage::eNone, false).has_value());
    CHECK_FALSE(MessageStatus::inlineStateForLogStatus(LogMessage::eDefault, false).has_value());
    CHECK_FALSE(MessageStatus::inlineStateForLogStatus(LogMessage::eDeleted, false).has_value());
    CHECK_FALSE(MessageStatus::inlineStateForLogStatus(LogMessage::eUnread, false).has_value());

    CHECK(MessageStatus::inlineStateForLogStatus(LogMessage::eHeard, false) == State::DirectImplicitAck);
    CHECK(MessageStatus::inlineStateForLogStatus(LogMessage::eAcked, false) == State::ExplicitAck);
    CHECK(MessageStatus::inlineStateForLogStatus(LogMessage::eNoResponse, false) == State::NoAck);
    CHECK(MessageStatus::inlineStateForLogStatus(LogMessage::eFailed, false) == State::NoAck);
}

TEST_CASE("MessageStatus persists final inline status")
{
    CHECK(MessageStatus::logStatusForState(State::ChannelImplicitAck) == LogMessage::eHeard);
    CHECK(MessageStatus::logStatusForState(State::DirectImplicitAck) == LogMessage::eHeard);
    CHECK(MessageStatus::logStatusForState(State::ExplicitAck) == LogMessage::eAcked);
    CHECK(MessageStatus::logStatusForState(State::NoAck) == LogMessage::eNoResponse);
    CHECK(MessageStatus::logStatusForState(State::RecipientKeyUnavailable) == LogMessage::eFailed);
    CHECK(MessageStatus::logStatusForState(State::MessageTooLarge) == LogMessage::eFailed);

    const uint32_t channelImplicitAck = MessageStatus::persistedLogState(State::ChannelImplicitAck);
    const uint32_t recipientKeyUnavailable = MessageStatus::persistedLogState(State::RecipientKeyUnavailable);
    const uint32_t messageTooLarge = MessageStatus::persistedLogState(State::MessageTooLarge);

    CHECK(MessageStatus::stateFromPersistedLogState(channelImplicitAck) == State::ChannelImplicitAck);
    CHECK(MessageStatus::stateFromPersistedLogState(recipientKeyUnavailable) == State::RecipientKeyUnavailable);
    CHECK(MessageStatus::stateFromPersistedLogState(messageTooLarge) == State::MessageTooLarge);
    CHECK_FALSE(MessageStatus::stateFromPersistedLogState(0).has_value());

    CHECK(MessageStatus::inlineStateForLogStatus(LogMessage::eHeard, channelImplicitAck, false) ==
          State::ChannelImplicitAck);
    CHECK(MessageStatus::inlineStateForLogStatus(LogMessage::eFailed, recipientKeyUnavailable, false) ==
          State::RecipientKeyUnavailable);
    CHECK(MessageStatus::inlineStateForLogStatus(LogMessage::eFailed, messageTooLarge, false) ==
          State::MessageTooLarge);
}
