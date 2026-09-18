#include "graphics/plugin/Plugins.h"

#ifdef MUI_MESSAGES_PLUGIN

#include "Arduino.h"
#include "graphics/common/ResponseHandler.h"
#include "graphics/plugin/ListRowStyle.h"
#include "graphics/plugin/MessagesPlugin.h"
#include "images.h"
#include "input/InputDriver.h"
#include "lv_i18n.h"
#include "lvgl.h"
#include "lvgl_private.h"
#include "util/Colors.h"
#include "util/ILog.h"
#include <ctime>
#include <time.h>

#ifndef MESSAGES_PLUGIN_CUSTOM_WIDGET_NAMES
#include "screens.h"
#endif

static MessagesPlugin *p = nullptr;

MessagesPlugin::MessagesPlugin(IMessagesWidgetFactory &factory) : GfxPlugin("Messages"), widgetFactory(factory) {}

MessagesPlugin::~MessagesPlugin()
{
    dismissNotification();
    if (kb)
        t9_kb_del(kb);
    if (p == this)
        p = nullptr;
}

void MessagesPlugin::init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount, lv_group_t *group,
                          lv_indev_t *indev, GfxPlugin::RegisterWidget registerWidget)
{
    p = this;
    // init the 8 channel containers
    messages[0] = nullptr;
    messages[1] = nullptr;
    messages[2] = nullptr;
    messages[3] = nullptr;
    messages[4] = nullptr;
    messages[5] = nullptr;
    messages[6] = nullptr;
    messages[7] = nullptr;
    GfxPlugin::init(parent, resolver, widgetCount, group, indev, registerWidget);
}

void MessagesPlugin::loadScreen(lv_screen_load_anim_t anim, uint32_t time)
{
    GfxPlugin::loadScreen(anim, time);
    showChats();
}

void MessagesPlugin::onHide()
{
    spacePending = false;
    automaticPeriodSpace = false;
    if (group)
        lv_group_set_editing(group, false);
}

void MessagesPlugin::task_handler(time_t millis)
{
    GfxPlugin::task_handler(millis);
    if (notification && (notificationsSuppressed || overlayIsVisible()))
        dismissNotification();
    if (unreadCount && activeMsgContainer) {
        const auto index = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(lv_obj_get_user_data(activeMsgContainer)));
        if (conversationIsVisible(index) && !isReadingHistory(activeMsgContainer))
            clearUnread(index);
    }
}

void MessagesPlugin::registerStandardWidgets(void)
{
#ifndef MESSAGES_PLUGIN_CUSTOM_WIDGET_NAMES
    // register standard widgets that were created by the generated UI
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::ChatsLabel), objects.top_chat_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::ChatsPanel), objects.chats_panel);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::ChatPanel), objects.chat_panel);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::MessagesLabel), objects.top_chat_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::MessageInputArea), objects.message_input_area);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::KeyboardPanel), objects.keyboard_panel);
#endif

    // cache widgets
    messageInput = getWidget(static_cast<WidgetIndex>(Widget::MessageInputArea));
    chatsPanel = getWidget(static_cast<WidgetIndex>(Widget::ChatsPanel));
    chatPanel = getWidget(static_cast<WidgetIndex>(Widget::ChatPanel));
    ListRowStyle::container(chatsPanel);
    if (chatPanel)
        lv_obj_remove_flag(chatPanel, LV_OBJ_FLAG_SCROLLABLE);
    if (messageInput)
        ListRowStyle::focus(messageInput);
}

void MessagesPlugin::registerStandardWidgetActions(void)
{
    // registerWidgetAction(static_cast<WidgetIndex>(Widget::XXX),
    // static_cast<GfxPlugin::Action>(Action::onMessageInput));
}

void MessagesPlugin::registerStandardEventCallbacks(void)
{
    if (messageInput) {
        lv_obj_add_event_cb(messageInput, ui_event_message_ready, LV_EVENT_ALL, this);
        lv_obj_add_event_cb(messageInput, ui_event_message_key, static_cast<lv_event_code_t>(LV_EVENT_KEY | LV_EVENT_PREPROCESS),
                            this);
    }
    if (parent)
        lv_obj_add_event_cb(parent, ui_event_screen_unloaded, LV_EVENT_SCREEN_UNLOAD_START, this);
}

void MessagesPlugin::ui_event_screen_unloaded(lv_event_t *e)
{
    static_cast<MessagesPlugin *>(lv_event_get_user_data(e))->onHide();
}

void MessagesPlugin::ui_event_message_key(lv_event_t *e)
{
    auto *plugin = static_cast<MessagesPlugin *>(lv_event_get_user_data(e));
    const uint32_t key = lv_event_get_key(e);
    InputDriver *input = InputDriver::instance();
    lv_indev_t *active = lv_indev_active();
    if (active && lv_indev_get_type(active) == LV_INDEV_TYPE_ENCODER && input->hasKeyboardDevice()) {
        if (plugin->activeMsgContainer && (key == LV_KEY_LEFT || key == LV_KEY_RIGHT)) {
            const int32_t step = lv_obj_get_style_text_font(plugin->messageInput, LV_PART_MAIN)->line_height + 6;
            const int32_t offset = lv_obj_get_scroll_y(plugin->activeMsgContainer) + (key == LV_KEY_RIGHT ? step : -step);
            lv_obj_scroll_to_y(plugin->activeMsgContainer, offset, LV_ANIM_OFF);
            lv_event_stop_processing(e);
        } else if (key == LV_KEY_ENTER) {
            // The knob operates history/navigation; only the keyboard submits a
            // draft.
            lv_event_stop_processing(e);
        }
        return;
    }
    if (key == LV_KEY_ESC) {
        lv_textarea_set_text(plugin->messageInput, "");
        plugin->spacePending = false;
        plugin->automaticPeriodSpace = false;
        if (plugin->onCancel)
            plugin->onCancel(e);
        lv_event_stop_processing(e);
    } else if (key == LV_KEY_ENTER || key == '\r') {
        plugin->submitMessage();
        lv_event_stop_processing(e);
    }
}

void MessagesPlugin::ui_event_message_ready(lv_event_t *e)
{
    auto *plugin = static_cast<MessagesPlugin *>(lv_event_get_user_data(e));
    const lv_event_code_t code = lv_event_get_code(e);
    plugin->handleMessageInput(e);
    if (code == LV_EVENT_READY) {
        plugin->submitMessage();
    } else if (code == LV_EVENT_FOCUSED) {
        lv_obj_t *kbPanel = plugin->getWidget(static_cast<WidgetIndex>(Widget::KeyboardPanel));
        InputDriver *input = InputDriver::instance();
        if (input->hasKeyboardDevice()) {
            if (kbPanel)
                lv_obj_add_flag(kbPanel, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(plugin->messageInput, LV_OBJ_FLAG_HIDDEN);
        } else if (kbPanel && !plugin->kb) {
            lv_obj_remove_flag(kbPanel, LV_OBJ_FLAG_HIDDEN);
            lv_group_t *previous = lv_group_get_default();
            lv_group_set_default(plugin->group);
            plugin->kb = t9_kb_create(kbPanel, plugin->messageInput);
            lv_group_set_default(previous);
            lv_obj_remove_state(plugin->messageInput, LV_STATE_FOCUSED);
        }
    }
}

void MessagesPlugin::submitMessage()
{
    const char *text = lv_textarea_get_text(messageInput);
    const size_t length = strlen(text);
    if (!length || !activeMsgContainer)
        return;
    // Preserve explicit Space+Enter for a newline. An automatically added period
    // leaves a trailing space too, but Enter should submit that sentence.
    if (text[length - 1] == ' ' && !automaticPeriodSpace) {
        lv_textarea_add_char(messageInput, '\n');
        return;
    }
    sendMessage(text);
    lv_textarea_set_text(messageInput, "");
    spacePending = false;
    automaticPeriodSpace = false;
    if (kb) {
        t9_kb_del(kb);
        kb = nullptr;
        lv_obj_add_flag(getWidget(static_cast<WidgetIndex>(Widget::KeyboardPanel)), LV_OBJ_FLAG_HIDDEN);
    }
}

void MessagesPlugin::handleMessageInput(lv_event_t *e)
{
    const auto code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
        automaticPeriodSpace = false;
    if (code == LV_EVENT_DEFOCUSED || code == LV_EVENT_PRESSED || (code == LV_EVENT_KEY && lv_event_get_key(e) != ' ')) {
        spacePending = false;
        automaticPeriodSpace = false;
        return;
    }
    if (code != LV_EVENT_INSERT)
        return;
    const char *insert = static_cast<const char *>(lv_event_get_param(e));
    lv_indev_t *keyboard = lv_indev_active();
    if (!doubleSpacePeriod || !keyboard || keyboard != InputDriver::instance()->getKeyboard() || keyboard->long_pr_sent ||
        !insert || insert[0] != ' ' || insert[1] != '\0') {
        spacePending = false;
        return;
    }
    const uint32_t cursor = lv_textarea_get_cursor_pos(messageInput);
    const char *text = lv_textarea_get_text(messageInput);
    const uint32_t maxLength = lv_textarea_get_max_length(messageInput);
    if (maxLength && lv_text_get_encoded_length(text) >= maxLength) {
        spacePending = false;
        return;
    }
    uint32_t byte = lv_text_encoded_get_byte_id(text, cursor);
    if (spacePending && lv_tick_elaps(lastSpaceAt) <= 500 && cursor == lastSpaceCursor + 1 && byte && text[byte - 1] == ' ') {
        spacePending = false;
        lv_textarea_delete_char(messageInput);
        lv_textarea_add_text(messageInput, ". ");
        automaticPeriodSpace = true;
        lv_textarea_set_insert_replace(messageInput, "");
        return;
    }
    const uint32_t previous = byte ? lv_text_encoded_prev(text, &byte) : 0;
    spacePending = (previous >= 'a' && previous <= 'z') || (previous >= 'A' && previous <= 'Z') ||
                   (previous >= '0' && previous <= '9') ||
                   (previous >= 0x80 && previous != 0xA0 && !(previous >= 0x2000 && previous <= 0x206F) &&
                    !(previous >= 0x3000 && previous <= 0x303F));
    lastSpaceCursor = cursor;
    lastSpaceAt = lv_tick_get();
}

void MessagesPlugin::ui_event_ChatButton(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_SHORT_CLICKED)
        return;
    const uint32_t index = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(lv_event_get_user_data(e)));
    lv_obj_t *button = lv_event_get_target_obj(e);
    lv_obj_remove_state(button, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
    const uint8_t channel = static_cast<uint8_t>(reinterpret_cast<uintptr_t>(lv_obj_get_user_data(button)));
    p->showMessages(index < c_max_channels ? 0 : index, channel);
}

void MessagesPlugin::handleAction(Action actionId, WidgetIndex /*idx*/, int /*event_code*/)
{
    switch (actionId) {
    default:
        invokeActionCallback(static_cast<GfxPlugin::Action>(actionId), nullptr);
        break;
    }
}

void MessagesPlugin::updateChats(void)
{
    lv_obj_t *chatsLabel = getWidget(static_cast<WidgetIndex>(Widget::ChatsLabel));
    lv_label_set_text_fmt(chatsLabel, _("%d active chat(s)"), chats.size());
}

void MessagesPlugin::showChats(void)
{
    ILOG_DEBUG("showChats %d", chats.size());
    updateChats();
    onHide();
    lv_obj_add_flag(chatPanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(messageInput, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(getWidget(static_cast<WidgetIndex>(Widget::KeyboardPanel)), LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(chatsPanel, LV_OBJ_FLAG_HIDDEN);
    lv_group_focus_obj(objects.top_chat_back_button);
}

void MessagesPlugin::showMessages(uint32_t nodeId, uint8_t ch)
{
    const uint32_t targetIndex = nodeId ? nodeId : ch;
    clearUnread(targetIndex);
    if (notification && notificationConversation == targetIndex)
        dismissNotification();
    ILOG_DEBUG("showMessages: nodeId:0x%08x, ch:%d", nodeId, ch);
    lv_obj_clear_flag(chatPanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(chatsPanel, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *container = nullptr;
    // find and show container
    uint32_t index = (nodeId == 0 ? ch : nodeId);
    auto it = messages.find(index);
    if (it != messages.end() && it->second) {
        lv_obj_clear_flag(it->second, LV_OBJ_FLAG_HIDDEN);
        container = it->second;
    } else {
        container = newMessageContainer(ownNode, nodeId ? nodeId : UINT32_MAX, ch);
    }
    if (activeMsgContainer)
        lv_obj_add_flag(activeMsgContainer, LV_OBJ_FLAG_HIDDEN);
    if (container) {
        activeMsgContainer = container;
        lv_obj_clear_flag(activeMsgContainer, LV_OBJ_FLAG_HIDDEN);
    }
    // activate keyboard for message input
    if (messageInput) {
        lv_obj_remove_flag(messageInput, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_state(messageInput, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
        InputDriver *inputdriver = InputDriver::instance();
        if (inputdriver->hasKeyboardDevice()) {
            lv_indev_set_group(inputdriver->getKeyboard(), group);
            if (inputdriver->hasEncoderDevice())
                lv_indev_set_group(inputdriver->getEncoder(), group);
            lv_group_focus_obj(messageInput);
            lv_group_set_editing(group, true);
        }
    }
}

uint32_t MessagesPlugin::conversationIndex(uint32_t from, uint32_t to, uint8_t ch) const
{
    if (to == UINT32_MAX || to == 0 || from == 0)
        return ch;
    return from == ownNode ? to : from;
}

void MessagesPlugin::newMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t &msgTime)
{
    receiveMessage(from, to, ch, msg, msgTime, true);
}

void MessagesPlugin::receiveMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t msgTime, bool notify)
{
    lv_obj_t *container = newMessageContainer(from, to, ch);
    const uint32_t index = conversationIndex(from, to, ch);
    const bool wasVisible = conversationIsVisible(index) && !isReadingHistory(container);
    std::string text;
    if (to == UINT32_MAX) {
        if (nodeNameResolver)
            text = nodeNameResolver(from);
        if (text.empty()) {
            char fallback[5];
            snprintf(fallback, sizeof(fallback), "%04x", from & 0xffff);
            text = fallback;
        }
        text += '\n';
    }
    text += msg;
    newMessage(container, msgTime, from, ch, text.c_str());
    if (notify && from != ownNode && !wasVisible) {
        if (unreadCount < UINT32_MAX) {
            ++unread[index];
            ++unreadCount;
        }
        updateUnread(index);
        if (notificationsEnabled && !notificationsSuppressed && !overlayIsVisible())
            showNotification(from, index, ch, msg);
    }
}

void MessagesPlugin::restoreMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t msgTime, bool trashFlag)
{
    if (trashFlag) {
        eraseChat(conversationIndex(from, to, ch));
        return;
    }
    if (from == ownNode) {
        addMessage(newMessageContainer(from, to, ch), msgTime, 0, msg);
    } else {
        receiveMessage(from, to, ch, msg, msgTime, false);
    }
}

void MessagesPlugin::clearChatHistory()
{
    dismissNotification();
    unread.clear();
    unreadBadges.clear();
    unreadCount = 0;
    if (unreadChanged)
        unreadChanged(0);
    if (activeMsgContainer)
        showChats();
    activeMsgContainer = nullptr;
    for (const auto &entry : messages) {
        if (entry.second)
            lv_obj_delete(entry.second);
    }
    for (const auto &entry : chats)
        lv_obj_delete(entry.second);
    messages.clear();
    chats.clear();
    updateChats();
}

void MessagesPlugin::eraseChat(uint8_t ch)
{
    eraseChat(static_cast<uint32_t>(ch));
}

void MessagesPlugin::eraseChat(uint32_t index)
{
    clearUnread(index);
    unreadBadges.erase(index);
    if (notification && notificationConversation == index)
        dismissNotification();
    auto message = messages.find(index);
    if (message != messages.end()) {
        if (activeMsgContainer && activeMsgContainer == message->second) {
            showChats();
            activeMsgContainer = nullptr;
        }
        if (message->second)
            lv_obj_delete(message->second);
        messages.erase(message);
    }
    auto chat = chats.find(index);
    if (chat != chats.end()) {
        lv_obj_delete(chat->second);
        chats.erase(chat);
    }
    updateChats();
}

// protected methods

lv_obj_t *MessagesPlugin::newMessageContainer(uint32_t from, uint32_t to, uint8_t ch)
{
    ILOG_DEBUG("newMessageContainer: from:0x%08x, to:0x%08x, ch:%d", from, to, ch);
    lv_obj_t *container = nullptr;
    uint32_t index = conversationIndex(from, to, ch);

    auto it = messages.find(index);
    if (it != messages.end() && it->second)
        return it->second;
    else {
        ILOG_DEBUG("create new container");
        container = widgetFactory.createMessageContainerWidget(chatPanel);
        lv_obj_set_user_data(container, reinterpret_cast<void *>(static_cast<uintptr_t>(index)));
        lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
        messages[index] = container;
    }
    // add chat entry to chatPanel to access the container
    addChat(from, to, ch);

    return container;
}

/**
 * Add chat entry if not exist
 *
 */
void MessagesPlugin::addChat(uint32_t from, uint32_t to, uint8_t ch)
{
    uint32_t index = conversationIndex(from, to, ch);
    auto it = chats.find(index);
    if (it != chats.end())
        return;

    //    lv_obj_t *chatDelBtn = nullptr;
    lv_group_t *oldGroup = lv_group_get_default();
    lv_group_set_default(group);
    lv_obj_t *btn = widgetFactory.createChatWidget(chatsPanel, index);
    lv_group_set_default(oldGroup);

    chats[index] = btn;
    auto *badge = lv_label_create(btn);
    lv_obj_set_style_bg_color(badge, lv_color_hex(0x67ea94), 0);
    lv_obj_set_style_bg_opa(badge, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(badge, lv_color_hex(0x15171a), 0);
    lv_obj_set_style_radius(badge, 8, 0);
    lv_obj_set_style_pad_hor(badge, 5, 0);
    lv_obj_set_style_pad_ver(badge, 2, 0);
    lv_obj_align(badge, LV_ALIGN_RIGHT_MID, 0, 0);
    ListRowStyle::text(badge);
    lv_obj_add_flag(badge, LV_OBJ_FLAG_HIDDEN);
    unreadBadges[index] = badge;
    // Reserve the trailing column without changing the factory's child order.
    for (uint32_t child = 0; child + 1 < lv_obj_get_child_count(btn); ++child) {
        auto *label = lv_obj_get_child(btn, child);
        if (lv_obj_check_type(label, &lv_label_class))
            lv_obj_set_width(label, LV_PCT(75));
    }
    lv_obj_set_user_data(btn, (void *)(unsigned long)ch);
    lv_obj_add_event_cb(btn, ui_event_ChatButton, LV_EVENT_SHORT_CLICKED, (void *)index);
    // lv_obj_add_event_cb(chatDelBtn, ui_event_ChatDelButton, LV_EVENT_CLICKED,
    // (void *)index);
    updateChats();
}

void MessagesPlugin::setNotificationsEnabled(bool enabled)
{
    notificationsEnabled = enabled;
    if (!enabled)
        dismissNotification();
}

void MessagesPlugin::setNotificationsSuppressed(bool suppressed)
{
    notificationsSuppressed = suppressed;
    if (suppressed)
        dismissNotification();
}

void MessagesPlugin::setOnUnreadChanged(const std::function<void(uint32_t)> &callback)
{
    unreadChanged = callback;
    if (unreadChanged)
        unreadChanged(unreadCount);
}

bool MessagesPlugin::overlayIsVisible() const
{
    auto *layer = lv_layer_top();
    for (uint32_t i = 0; i < lv_obj_get_child_count(layer); ++i) {
        auto *child = lv_obj_get_child(layer, i);
        if (child != notification && lv_obj_is_visible(child))
            return true;
    }
    return false;
}

bool MessagesPlugin::conversationIsVisible(uint32_t index) const
{
    const auto found = messages.find(index);
    return !notificationsSuppressed && !overlayIsVisible() && parent == lv_screen_active() && chatPanel &&
           lv_obj_is_visible(chatPanel) && found != messages.end() && found->second == activeMsgContainer;
}

void MessagesPlugin::clearUnread(uint32_t index)
{
    const auto found = unread.find(index);
    if (found == unread.end())
        return;
    unreadCount -= found->second;
    unread.erase(found);
    updateUnread(index);
}

void MessagesPlugin::updateUnread(uint32_t index)
{
    const auto found = unread.find(index);
    const uint32_t count = found == unread.end() ? 0 : found->second;
    const auto badge = unreadBadges.find(index);
    if (badge != unreadBadges.end()) {
        if (count) {
            if (count > 99)
                lv_label_set_text(badge->second, "99+");
            else
                lv_label_set_text_fmt(badge->second, "%u", count);
            lv_obj_remove_flag(badge->second, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(badge->second, LV_OBJ_FLAG_HIDDEN);
        }
    }
    if (unreadChanged)
        unreadChanged(unreadCount);
}

void MessagesPlugin::dismissNotification()
{
    if (notificationTimer) {
        lv_timer_delete(notificationTimer);
        notificationTimer = nullptr;
    }
    if (notification) {
        auto *old = notification;
        notification = nullptr;
        lv_obj_delete(old);
    }
}

void MessagesPlugin::showNotification(uint32_t from, uint32_t index, uint8_t channel, const char *message)
{
    dismissNotification();
    auto *previousGroup = lv_group_get_default();
    lv_group_set_default(nullptr);
    notification = lv_button_create(lv_layer_top());
    lv_group_set_default(previousGroup);
    notificationConversation = index;
    notificationChannel = channel;
    lv_obj_set_size(notification, LV_PCT(96), 34);
    lv_obj_align(notification, LV_ALIGN_TOP_MID, 0, 3);
    lv_obj_set_style_pad_all(notification, 6, 0);
    lv_obj_add_style(notification, &ListRowStyle::baseStyle(), 0);
    lv_obj_set_style_border_color(notification, lv_color_hex(0x67ea94), 0);
    lv_obj_set_style_border_width(notification, 2, 0);
    lv_obj_remove_flag(notification, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE));
    lv_obj_add_event_cb(notification, notificationEvent, LV_EVENT_ALL, this);
    auto *label = lv_label_create(notification);
    lv_obj_set_size(label, LV_PCT(100), LV_SIZE_CONTENT);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    ListRowStyle::text(label);
    std::string title = nodeNameResolver ? nodeNameResolver(from) : std::string();
    if (title.empty()) {
        char fallback[10];
        snprintf(fallback, sizeof(fallback), "!%08x", from);
        title = fallback;
    }
    title += ": ";
    title += message ? message : "";
    for (auto &character : title)
        if (character == '\n' || character == '\r')
            character = ' ';
    lv_label_set_text(label, title.c_str());
    notificationTimer = lv_timer_create(notificationTimeout, 4000, this);
}

void MessagesPlugin::notificationEvent(lv_event_t *event)
{
    auto *plugin = static_cast<MessagesPlugin *>(lv_event_get_user_data(event));
    if (lv_event_get_code(event) == LV_EVENT_DELETE) {
        plugin->notification = nullptr;
        if (plugin->notificationTimer) {
            lv_timer_delete(plugin->notificationTimer);
            plugin->notificationTimer = nullptr;
        }
    } else if (lv_event_get_code(event) == LV_EVENT_SHORT_CLICKED) {
        const uint32_t index = plugin->notificationConversation;
        const uint8_t channel = plugin->notificationChannel;
        plugin->dismissNotification();
        if (plugin->notificationsSuppressed || plugin->overlayIsVisible())
            return;
        plugin->loadScreen(LV_SCR_LOAD_ANIM_NONE, 0);
        plugin->showMessages(index < c_max_channels ? 0 : index, channel);
    }
}

void MessagesPlugin::notificationTimeout(lv_timer_t *timer)
{
    auto *plugin = static_cast<MessagesPlugin *>(lv_timer_get_user_data(timer));
    plugin->notificationTimer = nullptr;
    lv_timer_delete(timer);
    plugin->dismissNotification();
}

// --- private callbacks ---

bool MessagesPlugin::isReadingHistory(lv_obj_t *container) const
{
    lv_obj_update_layout(container);
    lv_point_t end;
    lv_obj_get_scroll_end(container, &end);
    const int32_t bottom = lv_obj_get_scroll_y(container) + lv_obj_get_scroll_bottom(container);
    return container == activeMsgContainer && lv_obj_is_visible(chatPanel) && end.y < bottom - 20;
}

void MessagesPlugin::scrollToLatest(lv_obj_t *container, lv_obj_t *message)
{
    lv_obj_update_layout(container);
    lv_obj_scroll_to_view(message, container == activeMsgContainer && lv_obj_is_visible(chatPanel) ? LV_ANIM_ON : LV_ANIM_OFF);
}

void MessagesPlugin::addMessage(lv_obj_t *container, uint32_t time, uint32_t requestId, const char *msg)
{
    const bool readingHistory = isReadingHistory(container);
    lv_group_t *oldGroup = lv_group_get_default();
    // History bubbles are content, not controls in the encoder focus order.
    lv_group_set_default(nullptr);
    lv_obj_t *btn = widgetFactory.createAddMessageWidget(container, time, requestId, msg);
    lv_obj_set_user_data(btn, reinterpret_cast<void *>(static_cast<uintptr_t>(requestId)));
    lv_group_set_default(oldGroup);
    if (!readingHistory)
        scrollToLatest(container, btn);
}

void MessagesPlugin::newMessage(lv_obj_t *container, uint32_t time, uint32_t nodeNum, uint8_t ch, const char *msg)
{
    const bool readingHistory = isReadingHistory(container);
    lv_group_t *oldGroup = lv_group_get_default();
    lv_group_set_default(nullptr);
    lv_obj_t *btn = widgetFactory.createNewMessageWidget(container, time, nodeNum, ch, msg);
    lv_obj_set_user_data(btn, nullptr);
    lv_group_set_default(oldGroup);
    if (!readingHistory)
        scrollToLatest(container, btn);
}

void MessagesPlugin::sendMessage(const char *msg)
{
    if (!activeMsgContainer || !onSendMessage)
        return;
    // retrieve nodeNum + channel from activeMsgContainer
    uint32_t to = UINT32_MAX;
    uint8_t ch = 0;
    uint32_t channelOrNode = (unsigned long)lv_obj_get_user_data(activeMsgContainer);

    if (channelOrNode < c_max_channels) {
        ch = (uint8_t)channelOrNode;
    } else {
        ch = 0; // to be set to the node channel by the view callback (onSendMessage)
        to = channelOrNode;
    }

    time_t curTime;
    time(&curTime);
    if (onSendMessage) {
        uint32_t requestId = onSendMessage(to, ch, curTime, msg);
        addMessage(activeMsgContainer, (uint32_t)curTime, requestId, msg);
    }
}

void MessagesPlugin::handleResponse(uint32_t channelOrNode, const uint32_t id, bool ack, bool err)
{
    lv_obj_t *msgContainer = messages[channelOrNode];
    if (!msgContainer) {
        ILOG_WARN("received unexpected response nodeNum/channel 0x%08x for request "
                  "id 0x%08x",
                  channelOrNode, id);
        return;
    }
    // go through all panels(buttons) and search for requestId
    uint16_t i = msgContainer->spec_attr->child_cnt;
    while (i-- > 0) {
        lv_obj_t *panel = msgContainer->spec_attr->children[i];
        uint32_t requestId = (unsigned long)panel->user_data;
        if (requestId == id) {
            ILOG_DEBUG("found requestId %u(%d)", requestId, i);
            ack = ack || (channelOrNode < c_max_channels); // treat messages sent to group channel same as ack
            // now give the textlabel border another color
            // lv_obj_t *textLabel = panel->spec_attr->children[0];
            lv_obj_set_style_border_color(panel,
                                          err   ? colorRed
                                          : ack ? colorBlueGreen
                                                : colorYellow,
                                          LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(panel, 1, 0);
            break;
        }
    }
}

#endif