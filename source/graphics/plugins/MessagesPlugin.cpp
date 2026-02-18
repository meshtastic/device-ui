#include "graphics/plugin/Plugins.h"

#ifdef MUI_MESSAGES_PLUGIN

#include "Arduino.h"
#include "graphics/plugin/MessagesPlugin.h"
#include "images.h"
#include "lv_i18n.h"
#include "lvgl.h"
#include "util/ILog.h"

#ifndef MESSAGES_PLUGIN_CUSTOM_WIDGET_NAMES
#include "screens.h"
#endif

static MessagesPlugin *p = nullptr;

MessagesPlugin::MessagesPlugin() : GfxPlugin("Messages") {}

MessagesPlugin::~MessagesPlugin() = default;

void MessagesPlugin::init(lv_obj_t *parent, WidgetResolver resolver, std::size_t widgetCount, lv_group_t *group,
                          lv_indev_t *indev, GfxPlugin::RegisterWidget registerWidget)
{
    p = this;
    GfxPlugin::init(parent, resolver, widgetCount, group, indev, registerWidget);
}

void MessagesPlugin::loadScreen(lv_screen_load_anim_t anim, uint32_t time)
{
    GfxPlugin::loadScreen(anim, time);
    if (messageInput) {
        lv_group_focus_obj(messageInput);
    }
}

void MessagesPlugin::registerStandardWidgets(void)
{
#ifndef MESSAGES_PLUGIN_CUSTOM_WIDGET_NAMES
    // register standard widgets that were created by the generated UI
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::ChatsLabel), objects.top_chat_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::ChatPanel), objects.chat_panel);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::MessagesLabel), objects.top_chat_label);
    setWidget(static_cast<GfxPlugin::WidgetIndex>(MessagesPlugin::Widget::MessageInputArea), objects.message_input_area);
#endif

    // cache input area widget
    messageInput = getWidget(static_cast<WidgetIndex>(Widget::MessageInputArea));
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
                    p->addMessage(txt);
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

void MessagesPlugin::updateChats(uint32_t chats) {}

void MessagesPlugin::openChat() {}

void MessagesPlugin::newMessage(const char *msg)
{
    // to be implemented in view
}

void MessagesPlugin::addMessage(const char *msg)
{
    lv_group_set_default(group);
    lv_indev_set_group(indev, group);
    if (p->onAddMessage) {
        // note: the argument is lv_event_t* but we pass the char* and convert it in the callee
        p->onAddMessage((lv_event_t *)msg);
    }
}
#endif