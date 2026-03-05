#include "graphics/plugin/Plugins.h"

#ifdef MUI_MESSAGES_PLUGIN

#include "Arduino.h"
#include "graphics/common/ResponseHandler.h"
#include "graphics/plugin/MessagesPlugin.h"
#include "images.h"
#include "input/InputDriver.h"
#include "lv_i18n.h"
#include "lvgl.h"
#include "lvgl_private.h"
#include "util/Colors.h"
#include "util/ILog.h"
#include <ctime>
#include <locale>
#include <time.h>

#ifndef MESSAGES_PLUGIN_CUSTOM_WIDGET_NAMES
#include "screens.h"
#endif

#define VALID_TIME(T) (T > 1000000 && T < UINT32_MAX)

static MessagesPlugin *p = nullptr;

MessagesPlugin::MessagesPlugin(IMessagesWidgetFactory &factory) : GfxPlugin("Messages"), widgetFactory(factory) {}

MessagesPlugin::~MessagesPlugin() = default;

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

void MessagesPlugin::registerStandardWidgets(void)
{
#ifndef MESSAGES_PLUGIN_CUSTOM_WIDGET_NAMES
    // register standard widgets that were created by the generated UI
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::ChatsLabel), objects.top_chat_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::ChatsPanel), objects.chats_panel);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::ChatPanel), objects.chat_panel);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::MessagesLabel), objects.top_chat_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::MessageInputArea), objects.message_input_area);
#endif

    // cache widgets
    messageInput = getWidget(static_cast<WidgetIndex>(Widget::MessageInputArea));
    chatsPanel = p->getWidget(static_cast<WidgetIndex>(Widget::ChatsPanel));
    chatPanel = p->getWidget(static_cast<WidgetIndex>(Widget::ChatPanel));
}

void MessagesPlugin::registerStandardWidgetActions(void)
{
    // registerWidgetAction(static_cast<WidgetIndex>(Widget::XXX), static_cast<GfxPlugin::Action>(Action::onMessageInput));
}

void MessagesPlugin::registerStandardEventCallbacks(void)
{
    if (messageInput)
        lv_obj_add_event_cb(messageInput, ui_event_message_ready, LV_EVENT_ALL, (void *)&onMessageInput);
}

void MessagesPlugin::ui_event_message_ready(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (p->messageInput) {
        if (event_code == LV_EVENT_KEY) {
            uint32_t c = *(unsigned long *)lv_event_get_param(e);
            switch (c) {
            case LV_KEY_ESC: // SYM BACK
                lv_obj_remove_state(p->messageInput, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
                lv_textarea_set_text(p->messageInput, "");
                if (p->onCancel) {
                    p->onCancel(e);
                }
                break;
            case 0x0D: { // SYM RETURN
                char *txt = (char *)lv_textarea_get_text(p->messageInput);
                uint32_t len = strlen(txt);
                if (len >= 1) {
                    txt[len - 1] = '\0'; // overwrite '#' (0x0D)
                    p->sendMessage(txt);
                }
                lv_textarea_set_text(p->messageInput, "");
                break;
            }
            default:
                break;
            }

            ILOG_DEBUG("ui_event_message_ready -> LV_EVENT_KEY '%c'(0x%02x)", (char)c, c);
        }
    } else { // handle by callback if applicable
        MessagesPlugin::Callback *onMessage = (MessagesPlugin::Callback *)(lv_event_get_user_data(e));
        if (onMessage && *onMessage)
            (*onMessage)(e);
    }
}

void MessagesPlugin::ui_event_ChatButton(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
        uint32_t index = (unsigned long)lv_event_get_user_data(e);
        lv_obj_remove_state(p->chats[index], lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
        p->showMessages(index, (uint8_t)(unsigned long)lv_obj_get_user_data(p->chats[index]));
    }
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
    lv_obj_add_flag(chatPanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(chatsPanel, LV_OBJ_FLAG_HIDDEN);
    lv_group_focus_obj(objects.top_chat_back_button);
}

void MessagesPlugin::showMessages(uint32_t nodeId, uint8_t ch)
{
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
        container = newMessageContainer(ownNode, nodeId, ch);
    }
    if (activeMsgContainer)
        lv_obj_add_flag(activeMsgContainer, LV_OBJ_FLAG_HIDDEN);
    if (container) {
        activeMsgContainer = container;
        lv_obj_set_user_data(activeMsgContainer, (void *)index);
        lv_obj_clear_flag(activeMsgContainer, LV_OBJ_FLAG_HIDDEN);
    }
    // activate keyboard for message input
    if (messageInput) {
        lv_obj_remove_state(messageInput, lv_state_t(LV_STATE_CHECKED | LV_STATE_PRESSED));
        lv_group_focus_obj(messageInput);
        InputDriver *inputdriver = InputDriver::instance();
        if (inputdriver->hasKeyboardDevice())
            lv_indev_set_group(inputdriver->getKeyboard(), group);
        else {
            // TODO: virtual keyboard
        }
    }
}

void MessagesPlugin::newMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t &msgTime)
{
    ILOG_DEBUG("newMessage: from:0x%08x, to:0x%08x, ch:%d, time:%d", from, to, ch, msgTime);
    int pos = 0;
    char buf[284]; // 237 + 4 + 40 + 2 + 1
    lv_obj_t *container = nullptr;
    if (to == UINT32_MAX) { // message for group, prepend short name to msg
#if 0
        if (nodes.find(from) == nodes.end()) {
            pos += sprintf(buf, "%04x ", from & 0xffff);
        } else {
            // original short name is held in userData, extract it and add msg
            char *userData = (char *)&(nodes[from]->LV_OBJ_IDX(node_lbs_idx)->user_data);
            while (pos < 4 && userData[pos] != 0) {
                buf[pos] = userData[pos];
                pos++;
            }
        }
        buf[pos++] = ' ';
#endif
        container = messages[ch];
    } else { // message for us
        container = messages[from];
    }

    // if it's the first message we need a container
    if (!container) {
        container = newMessageContainer(from, to, ch);
    }

    pos += timestamp(&buf[pos], msgTime, false);
    sprintf(&buf[pos], "%s", msg);

    // place message into container
    newMessage(container, msgTime, from, ch, buf);
}

/**
 * @brief insert a message into a <channel group> or <from node> container
 *
 * @param from source node
 * @param to destination node
 * @param ch channel
 * @param msg text message
 * @param msgTime in/out: message time (maybe overwritten when 0)
 * @param trashFlag marked as delete previous chat
 */
void MessagesPlugin::restoreMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t msgTime, bool trashFlag)
{
    if (from == ownNode) {
        lv_obj_t *container = nullptr;
        if (to == UINT32_MAX) {
            if (trashFlag && chats.find(ch) != chats.end()) {
                ILOG_DEBUG("trashFlag set for channel %d", ch);
                eraseChat(ch);
                return;
            } else {
                container = newMessageContainer(from, to, ch);
            }
        }
#if 0
        else {
            if (nodes.find(to) != nodes.end()) {
                if (trashFlag && chats.find(to) != chats.end()) {
                    ILOG_DEBUG("trashFlag set for node %08x", to);
                    eraseChat(to);
                    return;
                } else {
                    container = newMessageContainer(to, from, ch);
                }
            } else {
                ILOG_DEBUG("to node 0x%08x not in db", to);
                meshtastic_NodeInfo node{};
                MeshtasticView::addOrUpdateNode(to, ch, node);
            }
        }
#endif
        if (container != nullptr) {
            if (container != activeMsgContainer)
                lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
            addMessage(container, msgTime, 0, msg); // TODO recover requestId from log
        }

    }
#if 0
    else if (nodes.find(from) != nodes.end()) {
        if (msg.trashFlag && chats.find(from) != chats.end()) {
            ILOG_DEBUG("trashFlag set for node %08x", from);
            eraseChat(from);
            return;
        } else {
            uint32_t time = msgTime ? msgTime : UINT32_MAX; // don't overwrite 0 with actual time
            newMessage(from, to, ch, (const char *)msg.bytes, time);
        }
    }
#endif
    else {
        int pos = 0;
        char buf[284]; // 237 + 4 + 40 + 2 + 1
        if (to != UINT32_MAX) {
            // from node not in db
            ILOG_WARN("from node 0x%08x not in db", from); // TODO
            // meshtastic_NodeInfo node{};
            // MeshtasticView::addOrUpdateNode(from, ch, node);
        } else {
            ILOG_DEBUG("from node 0x%08x not in db and no need to insert", from);
            pos += sprintf(buf, "%04x ", from & 0xffff);
        }
        uint32_t len = timestamp(buf + pos, msgTime, false);
        strcpy(buf + pos + len, msg);
        lv_obj_t *container = newMessageContainer(from, to, ch);
        lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
        newMessage(container, msgTime, from, ch, buf);
    }
#if 0
    ILOG_DEBUG("newMessage: from:0x%08x, to:0x%08x, ch:%d, time:%d", from, to, ch, msgTime);
    int pos = 0;
    char buf[284]; // 237 + 4 + 40 + 2 + 1
    lv_obj_t *container = nullptr;
    if (to == UINT32_MAX) { // message for group, prepend short name to msg
#if 0
        if (nodes.find(from) == nodes.end()) {
            pos += sprintf(buf, "%04x ", from & 0xffff);
        } else {
            // original short name is held in userData, extract it and add msg
            char *userData = (char *)&(nodes[from]->LV_OBJ_IDX(node_lbs_idx)->user_data);
            while (pos < 4 && userData[pos] != 0) {
                buf[pos] = userData[pos];
                pos++;
            }
        }
        buf[pos++] = ' ';
#endif
        container = messages[ch];
    } else { // message for us
        container = messages[from];
    }

    // if it's the first message we need a container
    if (!container) {
        container = newMessageContainer(from, to, ch);
    }

    pos += timestamp(&buf[pos], msgTime, false);
    sprintf(&buf[pos], "%s", msg);

    // place message into container
    newMessage(container, msgTime, from, ch, buf);
#endif
}

void MessagesPlugin::clearChatHistory(void) {}

// protected methods

lv_obj_t *MessagesPlugin::newMessageContainer(uint32_t from, uint32_t to, uint8_t ch)
{
    ILOG_DEBUG("newMessageContainer: from:0x%08x, to:0x%08x, ch:%d", from, to, ch);
    lv_obj_t *container = nullptr;
    uint32_t index = ((to == UINT32_MAX || from == 0) ? ch : from);

    auto it = messages.find(index);
    if (it != messages.end() && it->second)
        return it->second;
    else {
        ILOG_DEBUG("create new container");
        lv_obj_t *chatPanel = getWidget(static_cast<WidgetIndex>(Widget::ChatPanel));
        container = widgetFactory.createMessageContainerWidget(chatPanel);
        lv_obj_set_user_data(container, (void *)(unsigned long)ch);
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
    uint32_t index = ((to == UINT32_MAX || from == 0) ? ch : from);
    auto it = chats.find(index);
    if (it != chats.end())
        return;

    //    lv_obj_t *chatDelBtn = nullptr;
    lv_group_t *oldGroup = lv_group_get_default();
    lv_group_set_default(group);
    lv_obj_t *chatsPanel = getWidget(static_cast<WidgetIndex>(Widget::ChatsPanel));
    lv_obj_t *btn = widgetFactory.createChatWidget(chatsPanel, index);
    lv_group_set_default(oldGroup);

    chats[index] = btn;
    lv_obj_set_user_data(btn, (void *)(unsigned long)ch);
    lv_obj_add_event_cb(btn, ui_event_ChatButton, LV_EVENT_ALL, (void *)index);
    // lv_obj_add_event_cb(chatDelBtn, ui_event_ChatDelButton, LV_EVENT_CLICKED, (void *)index);
    updateChats();
}

uint32_t MessagesPlugin::timestamp(char *buf, uint32_t datetime, bool update)
{
    time_t local = datetime;
    if (update) {
#ifdef ARCH_PORTDUINO
        time(&local);
#else
        if (VALID_TIME(curtime))
            local = curtime;
#endif
    }
    if (VALID_TIME(local)) {
        std::tm date_tm{};
        localtime_r(&local, &date_tm);
        if (!update)
            return strftime(buf, 20, "%y/%m/%d %R\n", &date_tm);
        else
            return strftime(buf, 20, "%R\n", &date_tm);
    } else
        return 0;
}

// --- private callbacks ---

void MessagesPlugin::addMessage(lv_obj_t *container, uint32_t time, uint32_t requestId, const char *msg)
{
    ILOG_INFO("addMessage %d, %d, '%s'", time, requestId, msg);
    lv_group_t *oldGroup = lv_group_get_default();
    lv_group_set_default(group);
    lv_obj_t *btn = widgetFactory.createAddMessageWidget(container, time, 0, msg);
    lv_obj_set_user_data(btn, (void *)requestId);
    lv_group_set_default(oldGroup);
}

void MessagesPlugin::newMessage(lv_obj_t *container, uint32_t time, uint32_t nodeNum, uint8_t ch, const char *msg)
{
    ILOG_INFO("newMessage %d, %d, %d, '%s'", time, nodeNum, (int)ch, msg);
    lv_group_t *oldGroup = lv_group_get_default();
    lv_group_set_default(group);
    lv_obj_t *btn = widgetFactory.createNewMessageWidget(container, time, nodeNum, ch, msg);
    lv_obj_set_user_data(btn, (void *)0); // TODO set to recovered requestId
    lv_group_set_default(oldGroup);
}

void MessagesPlugin::sendMessage(const char *msg)
{
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
        ILOG_WARN("received unexpected response nodeNum/channel 0x%08x for request id 0x%08x", channelOrNode, id);
        return;
    }
    // go through all panels(buttons) and search for requestId
    uint16_t i = msgContainer->spec_attr->child_cnt;
    while (i-- > 0) {
        lv_obj_t *panel = msgContainer->spec_attr->children[i];
        uint32_t requestId = (unsigned long)panel->user_data;
        if (requestId == id) {
            ILOG_DEBUG("found requestId %u(%d)", requestId, i);
            bool ack = ack || (channelOrNode < c_max_channels); // treat messages sent to group channel same as ack
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