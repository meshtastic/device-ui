#include "graphics/view/TFT/TFTView_Debug.h"

#if defined(LVGL_DEBUG_FOCUS)

#include "graphics/view/TFT/TFTView_320x240.h"
#include "lvgl_private.h"
#include "ui.h"
#include "util/ILog.h"

static lv_indev_t *findIndevForGroup(lv_group_t *group)
{
    if (!group) {
        return nullptr;
    }

    for (lv_indev_t *indev = lv_indev_get_next(nullptr); indev; indev = lv_indev_get_next(indev)) {
        if (lv_indev_get_group(indev) == group) {
            return indev;
        }
    }

    return nullptr;
}

const char *TFTView_Debug::lookupObjectName320x240(lv_obj_t *obj)
{
    if (!obj) {
        return nullptr;
    }

    struct NamedObject {
        const char *name;
        lv_obj_t *ptr;
    };

#define OBJ_ENTRY(member)                                                                                                        \
    {                                                                                                                            \
        #member, objects.member                                                                                                  \
    }
    static const NamedObject namedObjects[] = {
#include "object_name_entries_320x240.inc"
    };
#undef OBJ_ENTRY

    for (const auto &entry : namedObjects) {
        if (obj == entry.ptr) {
            return entry.name;
        }
    }

    return nullptr;
}

bool TFTView_Debug::isDescendantOf(lv_obj_t *obj, lv_obj_t *ancestor)
{
    if (!obj || !ancestor) {
        return false;
    }

    lv_obj_t *node = obj;
    while (node) {
        if (node == ancestor) {
            return true;
        }
        node = lv_obj_get_parent(node);
    }

    return false;
}

uint32_t TFTView_Debug::getObjFlagsMask320x240(const lv_obj_t *obj)
{
    if (!obj) {
        return 0;
    }

    uint32_t flags = 0;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN))
        flags |= LV_OBJ_FLAG_HIDDEN;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_CLICKABLE))
        flags |= LV_OBJ_FLAG_CLICKABLE;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE))
        flags |= LV_OBJ_FLAG_CLICK_FOCUSABLE;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE))
        flags |= LV_OBJ_FLAG_CHECKABLE;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE))
        flags |= LV_OBJ_FLAG_SCROLLABLE;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_ELASTIC))
        flags |= LV_OBJ_FLAG_SCROLL_ELASTIC;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_MOMENTUM))
        flags |= LV_OBJ_FLAG_SCROLL_MOMENTUM;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_ONE))
        flags |= LV_OBJ_FLAG_SCROLL_ONE;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR))
        flags |= LV_OBJ_FLAG_SCROLL_CHAIN_HOR;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_VER))
        flags |= LV_OBJ_FLAG_SCROLL_CHAIN_VER;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS))
        flags |= LV_OBJ_FLAG_SCROLL_ON_FOCUS;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW))
        flags |= LV_OBJ_FLAG_SCROLL_WITH_ARROW;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SNAPPABLE))
        flags |= LV_OBJ_FLAG_SNAPPABLE;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_PRESS_LOCK))
        flags |= LV_OBJ_FLAG_PRESS_LOCK;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE))
        flags |= LV_OBJ_FLAG_EVENT_BUBBLE;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE))
        flags |= LV_OBJ_FLAG_GESTURE_BUBBLE;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_ADV_HITTEST))
        flags |= LV_OBJ_FLAG_ADV_HITTEST;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_IGNORE_LAYOUT))
        flags |= LV_OBJ_FLAG_IGNORE_LAYOUT;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_FLOATING))
        flags |= LV_OBJ_FLAG_FLOATING;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS))
        flags |= LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE))
        flags |= LV_OBJ_FLAG_OVERFLOW_VISIBLE;
#if LV_USE_FLEX
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK))
        flags |= LV_OBJ_FLAG_FLEX_IN_NEW_TRACK;
#endif
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_LAYOUT_1))
        flags |= LV_OBJ_FLAG_LAYOUT_1;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_LAYOUT_2))
        flags |= LV_OBJ_FLAG_LAYOUT_2;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_WIDGET_1))
        flags |= LV_OBJ_FLAG_WIDGET_1;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_WIDGET_2))
        flags |= LV_OBJ_FLAG_WIDGET_2;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_USER_1))
        flags |= LV_OBJ_FLAG_USER_1;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_USER_2))
        flags |= LV_OBJ_FLAG_USER_2;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_USER_3))
        flags |= LV_OBJ_FLAG_USER_3;
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_USER_4))
        flags |= LV_OBJ_FLAG_USER_4;

    return flags;
}

std::string TFTView_Debug::flagsMaskToCompactString320x240(uint32_t flags)
{
    std::string out;
    auto add = [&](uint32_t bit, const char *label) {
        if (!(flags & bit)) {
            return;
        }
        if (!out.empty()) {
            out += "|";
        }
        out += label;
    };

    add(LV_OBJ_FLAG_HIDDEN, "HID");
    add(LV_OBJ_FLAG_CLICKABLE, "CLK");
    add(LV_OBJ_FLAG_CLICK_FOCUSABLE, "CF");
    add(LV_OBJ_FLAG_CHECKABLE, "CHK");
    add(LV_OBJ_FLAG_SCROLLABLE, "SCR");
    add(LV_OBJ_FLAG_SCROLL_ELASTIC, "ELS");
    add(LV_OBJ_FLAG_SCROLL_MOMENTUM, "MOM");
    add(LV_OBJ_FLAG_SCROLL_ONE, "ONE");
    add(LV_OBJ_FLAG_SCROLL_CHAIN_HOR, "SCH");
    add(LV_OBJ_FLAG_SCROLL_CHAIN_VER, "SCV");
    add(LV_OBJ_FLAG_SCROLL_ON_FOCUS, "SOF");
    add(LV_OBJ_FLAG_SCROLL_WITH_ARROW, "SWA");
    add(LV_OBJ_FLAG_SNAPPABLE, "SNP");
    add(LV_OBJ_FLAG_PRESS_LOCK, "PRL");
    add(LV_OBJ_FLAG_EVENT_BUBBLE, "EVB");
    add(LV_OBJ_FLAG_GESTURE_BUBBLE, "GSB");
    add(LV_OBJ_FLAG_ADV_HITTEST, "AHT");
    add(LV_OBJ_FLAG_IGNORE_LAYOUT, "IGL");
    add(LV_OBJ_FLAG_FLOATING, "FLT");
    add(LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS, "DTE");
    add(LV_OBJ_FLAG_OVERFLOW_VISIBLE, "OFV");
#if LV_USE_FLEX
    add(LV_OBJ_FLAG_FLEX_IN_NEW_TRACK, "FNT");
#endif
    add(LV_OBJ_FLAG_LAYOUT_1, "LY1");
    add(LV_OBJ_FLAG_LAYOUT_2, "LY2");
    add(LV_OBJ_FLAG_WIDGET_1, "WG1");
    add(LV_OBJ_FLAG_WIDGET_2, "WG2");
    add(LV_OBJ_FLAG_USER_1, "U1");
    add(LV_OBJ_FLAG_USER_2, "U2");
    add(LV_OBJ_FLAG_USER_3, "U3");
    add(LV_OBJ_FLAG_USER_4, "U4");

    if (out.empty()) {
        return "-";
    }
    return out;
}

void TFTView_Debug::logObjectDetails320x240(const char *tag, lv_obj_t *obj)
{
    if (!obj) {
        ILOG_INFO("%s: <null>", tag ? tag : "obj");
        return;
    }

    const char *name = lookupObjectName320x240(obj);
    const lv_obj_class_t *klass = lv_obj_get_class(obj);
    const char *className = (klass && klass->name) ? klass->name : "<no-class>";
    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);
    lv_state_t state = lv_obj_get_state(obj);
    uint32_t flags = getObjFlagsMask320x240(obj);
    std::string flagCompact = flagsMaskToCompactString320x240(flags);
    bool visible = lv_obj_is_visible(obj);
    bool hidden = lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN);
    bool focusable = lv_obj_has_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    bool inMessagesPanel = isDescendantOf(obj, objects.messages_panel);
    bool inChatsPanel = isDescendantOf(obj, objects.chats_panel);
    bool inMessagesContainer = isDescendantOf(obj, objects.messages_container);
    bool inKeyboard = isDescendantOf(obj, objects.keyboard);

    ILOG_INFO("%s: %p name=%s class=%s vis=%d hidden=%d focusable=%d focused=%d idx=%ld xy=(%ld,%ld)-(%ld,%ld) wh=%ldx%ld "
              "state=0x%08lx flags=0x%08lx [%s] in[msg=%d chats=%d msgcont=%d kb=%d]",
              tag ? tag : "obj", (void *)obj, name ? name : "<unknown>", className, visible, hidden, focusable,
              lv_obj_has_state(obj, LV_STATE_FOCUSED), (long)lv_obj_get_index(obj), (long)coords.x1, (long)coords.y1,
              (long)coords.x2, (long)coords.y2, (long)(coords.x2 - coords.x1 + 1), (long)(coords.y2 - coords.y1 + 1),
              (unsigned long)state, (unsigned long)flags, flagCompact.c_str(), inMessagesPanel, inChatsPanel, inMessagesContainer,
              inKeyboard);
}

void TFTView_Debug::logObjectChain320x240(lv_obj_t *obj)
{
    lv_obj_t *node = obj;
    int depth = 0;
    while (node && depth < 8) {
        const char *name = lookupObjectName320x240(node);
        const lv_obj_class_t *klass = lv_obj_get_class(node);
        const char *className = (klass && klass->name) ? klass->name : "<no-class>";
        lv_state_t state = lv_obj_get_state(node);
        uint32_t flags = getObjFlagsMask320x240(node);
        std::string flagCompact = flagsMaskToCompactString320x240(flags);
        ILOG_INFO("chain[%d]: %p name=%s class=%s hidden=%d state=0x%08lx flags=0x%08lx [%s]", depth, (void *)node,
                  name ? name : "<unknown>", className, lv_obj_has_flag(node, LV_OBJ_FLAG_HIDDEN), (unsigned long)state,
                  (unsigned long)flags, flagCompact.c_str());
        node = lv_obj_get_parent(node);
        depth++;
    }
}

void TFTView_Debug::printObjectName(TFTView_320x240 *view, lv_obj_t *obj)
{
    if (!obj) {
        ILOG_ERROR("Unknown object (null)");
        return;
    }

    const char *name = lookupObjectName320x240(obj);
    if (name) {
        ILOG_DEBUG("%s", name);
        return;
    }

    for (uint8_t i = 0; i < c_max_channels; ++i) {
        if (view->channelGroup[i] == obj) {
            ILOG_DEBUG("channelGroup[%u]", i);
            return;
        }
    }

    for (const auto &it : view->chats) {
        if (it.second == obj) {
            ILOG_DEBUG("chat_button[%lu]", (unsigned long)it.first);
            return;
        }
    }

    for (const auto &it : view->messages) {
        if (it.second == obj) {
            ILOG_DEBUG("message_container[%lu]", (unsigned long)it.first);
            return;
        }
    }

    lv_obj_t *parent = lv_obj_get_parent(obj);
    if (parent == objects.chats_panel) {
        ILOG_DEBUG("chats_panel.child[%ld]", (long)lv_obj_get_index(obj));
        return;
    }
    if (parent == objects.messages_panel) {
        ILOG_DEBUG("messages_panel.child[%ld]", (long)lv_obj_get_index(obj));
        return;
    }

    ILOG_ERROR("Unknown object: %p", (void *)obj);
}

void TFTView_Debug::ui_group_focus_debug_cb(lv_group_t *group)
{
    TFTView_320x240 *view = TFTView_320x240::instance();
    lv_obj_t *focused = group ? lv_group_get_focused(group) : nullptr;
    lv_indev_t *indev = findIndevForGroup(group);
    lv_obj_t *activeObj = lv_indev_get_active_obj();

    ILOG_INFO(">> GROUP_FOCUS_CB: group=%p focused=%p indev=%p activeObj=%p", (void *)group, (void *)focused, (void *)indev,
              (void *)activeObj);

    if (!view || !focused) {
        return;
    }

    printObjectName(view, focused);
    logObjectDetails320x240("groupCb", focused);

    if (activeObj && activeObj != focused) {
        ILOG_INFO(">> active object:");
        printObjectName(view, activeObj);
        logObjectDetails320x240("activeObj", activeObj);
    }

    if (!lookupObjectName320x240(focused)) {
        ILOG_INFO(">> unknown focus target details:");
        logObjectChain320x240(focused);
    }
}

#endif