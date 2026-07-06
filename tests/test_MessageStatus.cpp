#include "graphics/common/MessageStatus.h"
#include <doctest/doctest.h>
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
    CHECK(MessageStatus::presentation(State::NoChannel).text == std::string("No channel selected"));
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
}
