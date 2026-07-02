#include "input/policy/DefaultInputPolicyFactory.h"
#include "input/policy/IActionBindingResolver.h"
#include "input/policy/IInputContextProvider.h"
#include "input/policy/IUICommandDispatcher.h"
#include "input/policy/InputPipeline.h"
#include "input/policy/InputSourceRegistry.h"
#include <doctest/doctest.h>

namespace input_policy
{

namespace
{

class StaticContextProvider : public IInputContextProvider
{
  public:
    InputContextSnapshot getSnapshot() const override { return snapshot; }

    InputContextSnapshot snapshot{};
};

class RecordingDispatcher : public IUICommandDispatcher
{
  public:
    void dispatch(UICommand command, const CommandPayload &payload) override
    {
        lastCommand = command;
        lastPayload = payload;
        dispatchCount++;
    }

    UICommand lastCommand = UICommand::None;
    CommandPayload lastPayload{};
    int dispatchCount = 0;
};

} // namespace

TEST_CASE("DefaultInputPolicyFactory dispatches command actions")
{
    InputSourceRegistry registry;
    auto contextProvider = std::make_shared<StaticContextProvider>();
    auto dispatcher = std::make_shared<RecordingDispatcher>();

    DefaultInputPolicyFactory factory;
    auto buildResult = factory.build(registry, contextProvider, dispatcher);

    InputPipeline pipeline(buildResult.bindingResolver, contextProvider, dispatcher);
    pipeline.setPolicyChain(std::move(buildResult.chain));

    InputEvent event{};
    event.sourceId = "test";
    event.rawKeyCode = 0x101;
    event.resolvedKeyCode = 0x101;
    event.pressKind = PressKind::Press;

    std::vector<InputEvent> output;
    const bool forwarded = pipeline.process(event, InputCapabilities{}, output);

    CHECK_FALSE(forwarded);
    CHECK(output.empty());
    CHECK(event.action == InputAction::CommandHome);
    CHECK(dispatcher->dispatchCount == 1);
    CHECK(dispatcher->lastCommand == UICommand::GoHome);
    CHECK(dispatcher->lastPayload.command == UICommand::GoHome);
}

TEST_CASE("DefaultInputPolicyFactory still remaps focus traversal")
{
    InputSourceRegistry registry;
    auto contextProvider = std::make_shared<StaticContextProvider>();
    auto dispatcher = std::make_shared<RecordingDispatcher>();

    DefaultInputPolicyFactory factory;
    auto buildResult = factory.build(registry, contextProvider, dispatcher);

    InputPipeline pipeline(buildResult.bindingResolver, contextProvider, dispatcher);
    pipeline.setPolicyChain(std::move(buildResult.chain));

    InputEvent event{};
    event.sourceId = "test";
    event.rawKeyCode = 0x11;
    event.resolvedKeyCode = 0x11;
    event.pressKind = PressKind::Press;

    std::vector<InputEvent> output;
    const bool forwarded = pipeline.process(event, InputCapabilities{}, output);

    CHECK(forwarded);
    REQUIRE(output.size() == 1);
    CHECK(output.front().resolvedKeyCode == 11);
    CHECK(dispatcher->dispatchCount == 0);
}

} // namespace input_policy