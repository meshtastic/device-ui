#pragma once

#include "GfxPlugin.h"
#include "meshtastic/clientonly.pb.h"
#include "meshtastic/mesh.pb.h"
#include <cstdint>
#include <string>

/**
 * MessagesPlugin - implements business logic for the chat panel
 */
class MessagesPlugin : public GfxPlugin
{
  public:
    // widget slots used by chat/messages panels
    enum class Widget : GfxPlugin::WidgetIndex {
        ChatsLabel = 0,
        ChatPanel,
        MessagesLabel,
        MessagesPanel,
        MessageInputArea,
        // add more as required but keep below MAX_WIDGETS
        Count
    };

    // action identifiers used by plugins and views
    enum class Action : uint8_t { None = 0, MessageInputReady, Cancel };

    static constexpr std::size_t WIDGET_COUNT = static_cast<std::size_t>(Widget::Count);

    using Callback = std::function<void(lv_event_t *)>;

    MessagesPlugin();
    virtual ~MessagesPlugin();

    // init override: store resolver/parent and optionally auto-register widgets by name
    void init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount = WIDGET_COUNT, lv_group_t *group = nullptr,
              lv_indev_t *indev = nullptr, GfxPlugin::RegisterWidget registerWidget = GfxPlugin::RegisterWidget::All) override;

    // load screen (parent)
    virtual void loadScreen(lv_screen_load_anim_t anim = LV_SCR_LOAD_ANIM_MOVE_TOP, uint32_t time = 500);

    // Set view-level callbacks for plugin operations
    void setOnAddMessage(const Callback &cb) { onAddMessage = cb; }
    void setOnCancel(const Callback &cb) { onCancel = cb; }

    // Register menu widgets with default names
    void registerStandardWidgets(void) override;
    // Register widget event standard action
    void registerStandardEventCallbacks(void) override;
    // Register widget indices to compact Action values in the base class.
    void registerStandardWidgetActions(void) override;

    // Business logic methods: update UI
    virtual void updateChats(uint32_t chats);
    virtual void openChat();
    virtual void addMessage(const char *msg); // newly written message
    virtual void newMessage(const char *msg); // add newly received message

  protected:
    // handleAction: map compact Action values to plugin callbacks
    void handleAction(Action actionId, WidgetIndex idx, int event_code) /*override*/;

  private:
    // lvgl event handlers
    static void ui_event_message_ready(lv_event_t *e);

    // often used
    lv_obj_t *messageInput = nullptr;

    // plugin callback (default implementation can be overwritten by a specific view)
    Callback onMessageInput;
    Callback onAddMessage;
    Callback onCancel;
};
