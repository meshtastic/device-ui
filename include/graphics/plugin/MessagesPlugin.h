#pragma once

#include "graphics/plugin/GfxPlugin.h"
#include "input/lv_t9_keyboard.h"
#include <array>
#include <cstdint>
#include <unordered_map>

/**
 * dynamic widgets, to be implemented by the view (ideally as a user widget in
 * eez-studio)
 */
class IMessagesWidgetFactory
{
  public:
    virtual lv_obj_t *createChatWidget(lv_obj_t *parent, uint32_t index) = 0;
    virtual lv_obj_t *createMessageContainerWidget(lv_obj_t *parent) = 0;
    virtual lv_obj_t *createAddMessageWidget(lv_obj_t *parent, uint32_t msgTime, uint32_t requestId, const char *msg) = 0;
    virtual lv_obj_t *createNewMessageWidget(lv_obj_t *parent, uint32_t msgTime, uint32_t nodeNum, uint8_t channel,
                                             const char *msg) = 0;
    virtual ~IMessagesWidgetFactory() = default;
};

/**
 * MessagesPlugin - implements business logic for the chat panel
 */
class MessagesPlugin : public GfxPlugin
{
  public:
    // widget slots used by chat/messages panels
    enum class Widget : GfxPlugin::WidgetIndex {
        ChatsLabel = 0,
        ChatsPanel,
        ChatPanel,
        MessagesLabel,
        MessagesPanel,
        MessageInputArea,
        KeyboardPanel,
        // add more as required but keep below MAX_WIDGETS
        Count
    };

    // action identifiers used by plugins and views
    enum class Action : uint8_t { None = 0, MessageInputReady, Cancel };

    static constexpr uint8_t c_max_channels = 8;
    static constexpr std::size_t WIDGET_COUNT = static_cast<std::size_t>(Widget::Count);

    using Callback = std::function<void(lv_event_t *)>;
    using SendCallback = std::function<uint32_t(uint32_t to, uint8_t ch, uint32_t msgTime, const char *msg)>;
    using NodeNameResolver = std::function<std::string(uint32_t)>;

    MessagesPlugin(IMessagesWidgetFactory &factory);
    virtual ~MessagesPlugin();

    // init override: store resolver/parent and optionally auto-register widgets
    // by name
    void init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount = WIDGET_COUNT, lv_group_t *group = nullptr,
              lv_indev_t *indev = nullptr, GfxPlugin::RegisterWidget registerWidget = GfxPlugin::RegisterWidget::All) override;

    // load screen (parent)
    virtual void loadScreen(lv_screen_load_anim_t anim = LV_SCR_LOAD_ANIM_MOVE_TOP, uint32_t time = 500);

    // Set view-level callbacks for plugin operations
    void setOnCancel(const Callback &cb) { onCancel = cb; }
    void setOnSendMessage(const SendCallback &cb) { onSendMessage = cb; }
    void setOwnNode(uint32_t nodeNum) { ownNode = nodeNum; }
    void setNodeNameResolver(const NodeNameResolver &resolver) { nodeNameResolver = resolver; }
    void setDoubleSpacePeriod(bool enabled)
    {
        doubleSpacePeriod = enabled;
        spacePending = false;
        automaticPeriodSpace = false;
    }
    bool getDoubleSpacePeriod() const { return doubleSpacePeriod; }
    void setNotificationsEnabled(bool enabled);
    void setNotificationsSuppressed(bool suppressed);
    void setOnUnreadChanged(const std::function<void(uint32_t)> &callback);
    uint32_t getUnreadCount() const { return unreadCount; }
    void onHide() override;
    void task_handler(time_t millis) override;

    // Register menu widgets with default names
    void registerStandardWidgets(void) override;
    // Register widget event standard action
    void registerStandardEventCallbacks(void) override;
    // Register widget indices to compact Action values in the base class.
    void registerStandardWidgetActions(void) override;

    virtual void updateChats(void);
    // show chats panel
    virtual void showChats(void);
    // show chat
    virtual void showMessages(uint32_t nodeId, uint8_t ch);
    // add newly received message
    virtual void newMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t &msgTime);
    // restore from saved message
    virtual void restoreMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t msgTime, bool trashFlag);
    // message response
    virtual void handleResponse(uint32_t channelOrNode, const uint32_t id, bool ack, bool err);
    // erase chats
    virtual void clearChatHistory(void);
    virtual void eraseChat(uint8_t ch);
    virtual void eraseChat(uint32_t nodeId);

  protected:
    virtual void addChat(uint32_t from, uint32_t to, uint8_t ch);
    virtual lv_obj_t *newMessageContainer(uint32_t from, uint32_t to, uint8_t ch);

    // handleAction: map compact Action values to plugin callbacks
    void handleAction(Action actionId, WidgetIndex idx, int event_code) /*override*/;

    uint32_t ownNode = 0;
    t9_kb_t *kb = nullptr;
    lv_obj_t *messageInput = nullptr;
    lv_obj_t *chatPanel = nullptr;
    lv_obj_t *chatsPanel = nullptr;
    lv_obj_t *activeMsgContainer = nullptr;

    std::unordered_map<uint32_t, lv_obj_t *> messages; // message containers (within ChatPanel)
    std::unordered_map<uint32_t, lv_obj_t *> chats;    // active chats (within ChatsPanel)

  private:
    // lvgl event handlers
    static void ui_event_message_ready(lv_event_t *e);
    static void ui_event_message_key(lv_event_t *e);
    static void ui_event_ChatButton(lv_event_t *e);
    static void ui_event_screen_unloaded(lv_event_t *e);

    // helpers
    virtual void sendMessage(const char *msg);
    virtual void addMessage(lv_obj_t *container, uint32_t time, uint32_t requestId,
                            const char *msg); // newly written message
    virtual void newMessage(lv_obj_t *container, uint32_t msgTime, uint32_t nodeNum, uint8_t ch, const char *msg);
    void submitMessage(void);
    void handleMessageInput(lv_event_t *e);
    uint32_t conversationIndex(uint32_t from, uint32_t to, uint8_t ch) const;
    bool isReadingHistory(lv_obj_t *container) const;
    void scrollToLatest(lv_obj_t *container, lv_obj_t *message);
    void receiveMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t msgTime, bool notify);
    bool conversationIsVisible(uint32_t index) const;
    bool overlayIsVisible() const;
    void clearUnread(uint32_t index);
    void updateUnread(uint32_t index);
    void showNotification(uint32_t from, uint32_t index, uint8_t channel, const char *message);
    void dismissNotification();
    static void notificationEvent(lv_event_t *event);
    static void notificationTimeout(lv_timer_t *timer);

    // plugin callback (default implementation can be overwritten by a specific
    // view)
    Callback onMessageInput = nullptr;
    Callback onCancel = nullptr;
    SendCallback onSendMessage = nullptr;
    NodeNameResolver nodeNameResolver = nullptr;
    std::function<void(uint32_t)> unreadChanged;
    std::unordered_map<uint32_t, uint32_t> unread;
    std::unordered_map<uint32_t, lv_obj_t *> unreadBadges;
    uint32_t unreadCount = 0;
    bool notificationsEnabled = false;
    bool notificationsSuppressed = false;
    lv_obj_t *notification = nullptr;
    lv_timer_t *notificationTimer = nullptr;
    uint32_t notificationConversation = 0;
    uint8_t notificationChannel = 0;
    bool doubleSpacePeriod = false;
    bool spacePending = false;
    bool automaticPeriodSpace = false;
    uint32_t lastSpaceAt = 0;
    uint32_t lastSpaceCursor = 0;

    // view reference that implements the dynamic widget
    IMessagesWidgetFactory &widgetFactory;
};
