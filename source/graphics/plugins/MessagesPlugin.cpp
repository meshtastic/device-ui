#include "graphics/plugin/Plugins.h"

#ifdef MUI_MESSAGES_PLUGIN

#include "Arduino.h"
#include "graphics/plugin/MessagesPlugin.h"
#include "images.h"
#include "lv_i18n.h"
#include "lvgl.h"
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
                const char *txt = lv_textarea_get_text(p->messageInput);
                uint32_t len = strlen(txt);
                if (len) {
                    time_t curTime;
                    time(&curTime);
                    p->addMessage(p->activeMsgContainer, (uint32_t)curTime, txt);
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

void MessagesPlugin::handleAction(Action actionId, WidgetIndex /*idx*/, int /*event_code*/)
{
    switch (actionId) {
    default:
        invokeActionCallback(static_cast<GfxPlugin::Action>(actionId), nullptr);
        break;
    }
}

void MessagesPlugin::showChats(void)
{
    lv_obj_add_flag(chatPanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(chatsPanel, LV_OBJ_FLAG_HIDDEN);
}

void MessagesPlugin::showMessages(uint8_t ch)
{
    lv_obj_clear_flag(chatPanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(chatsPanel, LV_OBJ_FLAG_HIDDEN);
    // find and show container
    auto it = messages.find(ch);
    if (it != messages.end() && it->second) {
        lv_obj_clear_flag(it->second, LV_OBJ_FLAG_HIDDEN);
        if (messageInput) {
            lv_group_focus_obj(messageInput);
        }
    }
}

void MessagesPlugin::showMessages(uint32_t nodeId)
{
    ILOG_DEBUG("showMessages: nodeId:0x%08x, to:0x%08x, ch:%d, time:%d", nodeId);
    lv_obj_clear_flag(chatPanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(chatsPanel, LV_OBJ_FLAG_HIDDEN);
    // find and show container
    auto it = messages.find(nodeId);
    if (it != messages.end() && it->second) {
        lv_obj_clear_flag(it->second, LV_OBJ_FLAG_HIDDEN);
    } else {
        // create message container
        lv_obj_t *container = newMessageContainer(ownNode, nodeId, 0); // TODO ch
        lv_obj_clear_flag(container, LV_OBJ_FLAG_HIDDEN);
    }
    if (messageInput) {
        lv_group_focus_obj(messageInput);
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
    widgetFactory.createNewMessageWidget(container, from, ch, buf);
}

void MessagesPlugin::restoreMessage(uint32_t from, uint32_t to, uint8_t ch, const char *msg, uint32_t msgTime, bool trashFlag)
{
#if 1 // requires restoreMessage impl in PluggableView
    if (from == ownNode) {
        lv_obj_t *container = nullptr;
        if (to == UINT32_MAX) {
#if 1
            if (trashFlag && chats.find(ch) != chats.end()) {
                ILOG_DEBUG("trashFlag set for channel %d", ch);
                eraseChat(ch);
                return;
            } else
#endif
            {
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
            addMessage(container, msgTime, msg);
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
        newMessage(container, from, ch, buf);
    }

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
    newMessage(container, from, ch, buf);
#endif
}

void MessagesPlugin::clearChatHistory(void) {}

// protected methods

lv_obj_t *MessagesPlugin::newMessageContainer(uint32_t from, uint32_t to, uint8_t ch)
{
    if (to == UINT32_MAX || from == 0) {
        if (messages[ch] != nullptr)
            return messages[ch];
    } else {
        auto it = messages.find(from);
        if (it != messages.end() && it->second)
            return it->second;
    }

    // create container for new messages
    lv_obj_t *chatPanel = getWidget(static_cast<WidgetIndex>(Widget::ChatPanel));
    lv_obj_t *container = lv_obj_create(chatPanel);
#if 0
    lv_obj_remove_style_all(container);
    lv_obj_set_width(container, lv_pct(100));
    lv_obj_set_height(container, lv_pct(88));
    lv_obj_set_x(container, 0);
    lv_obj_set_y(container, 0);
    lv_obj_set_align(container, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(container, lv_obj_flag_t(LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                                               LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLL_ELASTIC)); /// Flags
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_ACTIVE);
    lv_obj_set_scroll_dir(container, LV_DIR_VER);
    lv_obj_set_style_pad_left(container, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(container, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(container, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
#endif
    // store new message container
    if (to == UINT32_MAX || from == 0) {
        messages[ch] = container;
    } else {
        messages[from] = container;
    }

    // optionally add chat to chatPanel to access the container
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
    lv_obj_t *chatsPanel = getWidget(static_cast<WidgetIndex>(Widget::ChatsPanel));
    lv_obj_t *parent_obj = chatsPanel;
#if 0 // TODO add widget 
    // ChatsButton
    lv_obj_t *chatBtn = lv_btn_create(parent_obj);
    lv_obj_set_pos(chatBtn, 0, 0);
    lv_obj_set_size(chatBtn, LV_PCT(100), buttonSize);
    lv_obj_add_flag(chatBtn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(chatBtn, LV_OBJ_FLAG_SCROLLABLE);
    add_style_home_button_style(chatBtn);
    lv_obj_set_style_align(chatBtn, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(chatBtn, colorMidGray, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(chatBtn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(chatBtn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(chatBtn, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(chatBtn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(chatBtn, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(chatBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(chatBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(chatBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(chatBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(chatBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    char buf[64];
    if (to == UINT32_MAX || from == 0) {
        sprintf(buf, "%d: %s", (int)ch, lv_label_get_text(channel[ch]));
    } else {
        auto it = nodes.find(from);
        if (it != nodes.end()) {
            sprintf(buf, "%s: %s", lv_label_get_text(it->second->LV_OBJ_IDX(node_lbs_idx)),
                    lv_label_get_text(it->second->LV_OBJ_IDX(node_lbl_idx)));
        } else {
            sprintf(buf, "!%08x", from);
        }
    }

    {
        lv_obj_t *parent_obj = chatBtn;
        {
            // ChatsButtonLabel
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.chats_button_label = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            lv_label_set_text(obj, buf);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // ChatDelButton
            lv_obj_t *obj = lv_btn_create(parent_obj);
            chatDelBtn = obj;
            lv_obj_set_pos(obj, -3, -1);
            lv_obj_set_size(obj, 40, 23);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, colorDarkRed, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // DelLabel
                    lv_obj_t *chatDelBtn = lv_label_create(parent_obj);
                    lv_obj_set_pos(chatDelBtn, 0, 0);
                    lv_obj_set_size(chatDelBtn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(chatDelBtn, _("DEL"));
                    lv_obj_set_style_align(chatDelBtn, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }

    chats[index] = chatBtn;
    updateActiveChats();
    if (index > c_max_channels) {
        if (nodes.find(index) != nodes.end())
            applyNodesFilter(index);
    }

    lv_obj_add_event_cb(chatBtn, ui_event_ChatButton, LV_EVENT_ALL, (void *)index);
    lv_obj_add_event_cb(chatDelBtn, ui_event_ChatDelButton, LV_EVENT_CLICKED, (void *)index);
#endif
}

uint32_t MessagesPlugin::timestamp(char *buf, uint32_t datetime, bool update)
{
    time_t local = datetime;
    if (update) {
#ifdef ARCH_PORTDUINO
        time(&local);
#else
        if (VALID_TIME(actTime))
            local = actTime;
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

void MessagesPlugin::addMessage(lv_obj_t *container, uint32_t time, const char *msg)
{
    lv_group_t *oldGroup = lv_group_get_default();
    lv_group_set_default(group);
    lv_indev_set_group(indev, group);
    lv_obj_t *widget = widgetFactory.createAddMessageWidget(container, time, 0, msg);
    lv_group_set_default(oldGroup);
}

void MessagesPlugin::newMessage(lv_obj_t *container, uint32_t nodeNum, uint8_t ch, const char *msg)
{
    lv_group_t *oldGroup = lv_group_get_default();
    lv_group_set_default(group);
    lv_indev_set_group(indev, group);
    lv_obj_t *widget = widgetFactory.createNewMessageWidget(container, nodeNum, ch, msg);
    lv_group_set_default(oldGroup);
}

#endif