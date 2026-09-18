#if defined(VIEW_480x222)

#include "graphics/view/TFT/TFTView_480x222.h"
#include "graphics/common/ViewController.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverFactory.h"
#include "graphics/map/MapPanel.h"
#include "images.h"
#include "input/InputDriver.h"
#include "graphics/plugin/ListRowStyle.h"
#include "lv_i18n.h"
#include "lvgl.h"
#include "lvgl_private.h"
#include "ui.h"
#include "util/ILog.h"
#include <algorithm>

// TODO: children index of nodepanel lv objects (see addNode)
enum NodePanelIdx { node_img_idx, node_lbs_idx, node_lbl_idx };

#define THIS TFTView_480x222::instance() // need to use THIS in all static methods

TFTView_480x222 *TFTView_480x222::instance(void)
{
    if (!gui)
        gui = new TFTView_480x222(nullptr, DisplayDriverFactory::create(480, 222));
    return static_cast<TFTView_480x222 *>(gui);
}

TFTView_480x222 *TFTView_480x222::instance(const DisplayDriverConfig &cfg)
{
    if (!gui)
        gui = new TFTView_480x222(&cfg, DisplayDriverFactory::create(cfg));
    return static_cast<TFTView_480x222 *>(gui);
}

TFTView_480x222::TFTView_480x222(const DisplayDriverConfig *cfg, DisplayDriver *driver) : PluggableView(cfg, driver) {}

void TFTView_480x222::init(IClientBase *client)
{
    ILOG_DEBUG("TFTView_480x222 init...");
    PluggableView::init(client);
}

void TFTView_480x222::ui_events_init(void)
{
    PluggableView::ui_events_init();
}

/**
 * @brief initialize UI with persistent data
 */
bool TFTView_480x222::setupUIConfig(const meshtastic_DeviceUIConfig &uiconfig)
{
    return PluggableView::setupUIConfig(uiconfig);
}

/**
 * @brief Initialize all screens and apply customizations
 *
 */
void TFTView_480x222::init_screens(void)
{
    PluggableView::init_screens();
}

/**
 * Overwrite the default generated function ui_init()
 * Wire plugin callbacks into existing view actions
 */

void TFTView_480x222::ui_init(void)
{
    PluggableView::ui_init();
}

namespace
{
void layoutMessageBubble(lv_obj_t *bubble, lv_obj_t *timeLabel, lv_obj_t *textLabel, const char *text, bool sent)
{
    // The native generated widget nests text under the time label. Reparent it
    // before hiding the timestamp so received, sent and restored messages match.
    lv_obj_set_parent(textLabel, bubble);
    lv_obj_add_flag(timeLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(bubble, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                                                          LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS));
    lv_group_remove_obj(bubble);
    lv_obj_set_style_pad_hor(bubble, 6, 0);
    lv_obj_set_style_pad_ver(bubble, 4, 0);
    lv_obj_set_style_border_width(bubble, 1, 0);
    lv_obj_set_style_radius(bubble, 6, 0);
    lv_obj_set_style_shadow_width(bubble, 0, 0);
    lv_obj_set_style_min_width(bubble, 0, 0);
    lv_obj_set_style_max_width(bubble, LV_COORD_MAX, 0);
    lv_obj_set_style_bg_opa(bubble, LV_OPA_COVER, 0);
    lv_obj_set_pos(textLabel, 0, 0);
    lv_obj_set_style_max_width(textLabel, LV_COORD_MAX, 0);
    lv_obj_set_style_pad_all(textLabel, 0, 0);
    lv_label_set_long_mode(textLabel, LV_LABEL_LONG_WRAP);
    ListRowStyle::text(textLabel);
    lv_obj_set_style_text_color(textLabel, lv_color_white(), 0);

    lv_obj_t *container = lv_obj_get_parent(bubble);
    lv_obj_update_layout(container);
    const int32_t available = std::max<int32_t>(1, lv_obj_get_content_width(container));
    const int32_t frame =
        lv_obj_get_style_space_left(bubble, LV_PART_MAIN) + lv_obj_get_style_space_right(bubble, LV_PART_MAIN);
    const int32_t maxTextWidth = std::max<int32_t>(1, available * 9 / 10 - frame);
    lv_point_t measured;
    lv_text_get_size(&measured, text, lv_obj_get_style_text_font(textLabel, LV_PART_MAIN),
                     lv_obj_get_style_text_letter_space(textLabel, LV_PART_MAIN),
                     lv_obj_get_style_text_line_space(textLabel, LV_PART_MAIN), maxTextWidth, LV_TEXT_FLAG_NONE);
    const int32_t textWidth = std::min<int32_t>(maxTextWidth, std::max<int32_t>(24, measured.x));
    lv_obj_set_width(bubble, textWidth + frame);
    lv_obj_set_height(bubble, LV_SIZE_CONTENT);
    lv_obj_set_content_width(textLabel, textWidth);
    lv_obj_set_height(textLabel, LV_SIZE_CONTENT);
    lv_label_set_text(textLabel, text);
    // The message container uses a column flex layout. Offset our own bubbles
    // within its content width without narrowing their text a second time.
    lv_obj_set_style_translate_x(bubble, sent ? available - textWidth - frame : 0, 0);
}
} // namespace

lv_obj_t *TFTView_480x222::createAddMessageWidget(lv_obj_t *parent, uint32_t, uint32_t, const char *msg)
{
    const int startWidgetIndex = 17; // generated native 480x222 widget slots
    create_user_widget_add_message_entry(parent, startWidgetIndex);
    lv_obj_t *bubble = ((lv_obj_t **)&objects)[startWidgetIndex];
    layoutMessageBubble(bubble, ((lv_obj_t **)&objects)[startWidgetIndex + 1],
                        ((lv_obj_t **)&objects)[startWidgetIndex + 2], msg, true);
    return bubble;
}

lv_obj_t *TFTView_480x222::createNewMessageWidget(lv_obj_t *parent, uint32_t, uint32_t, uint8_t, const char *msg)
{
    const int startWidgetIndex = 21; // generated native 480x222 widget slots
    create_user_widget_new_message_entry(parent, startWidgetIndex);
    lv_obj_t *bubble = ((lv_obj_t **)&objects)[startWidgetIndex];
    layoutMessageBubble(bubble, ((lv_obj_t **)&objects)[startWidgetIndex + 1],
                        ((lv_obj_t **)&objects)[startWidgetIndex + 2], msg, false);
    return bubble;
}

lv_obj_t *TFTView_480x222::createChatWidget(lv_obj_t *parent, uint32_t index)
{
    int startWidgetIndex = 12; // check out screen.c:tick_screen_widgets()
    create_user_widget_add_chat_entry(parent, startWidgetIndex);
    lv_obj_t *chatBtn = ((lv_obj_t **)&objects)[startWidgetIndex + 0];
    lv_obj_t *chImage = ((lv_obj_t **)&objects)[startWidgetIndex + 1];
    lv_obj_t *chIdLabel = ((lv_obj_t **)&objects)[startWidgetIndex + 2];
    lv_obj_t *chGroupNameLabel = ((lv_obj_t **)&objects)[startWidgetIndex + 3];

    ListRowStyle::row(chatBtn);
    ListRowStyle::text(chIdLabel);
    ListRowStyle::text(chGroupNameLabel);
    lv_obj_set_size(chImage, 24, 24);
    lv_obj_align(chImage, LV_ALIGN_LEFT_MID, 0, 0);
    lv_image_set_src(chImage, &img_chats_icon);
    lv_obj_set_width(chIdLabel, LV_PCT(85));
    lv_obj_set_width(chGroupNameLabel, LV_PCT(85));
    lv_label_set_long_mode(chIdLabel, LV_LABEL_LONG_DOT);
    lv_label_set_long_mode(chGroupNameLabel, LV_LABEL_LONG_DOT);
    lv_obj_align(chIdLabel, LV_ALIGN_TOP_LEFT, 32, 0);
    lv_obj_align(chGroupNameLabel, LV_ALIGN_TOP_LEFT, 32, 19);
    if (index < 8) {
        lv_label_set_text_fmt(chIdLabel, _("Channel %d"), index);
        lv_label_set_text(chGroupNameLabel,
                          db.channel[index].settings.name[0] ? db.channel[index].settings.name : _("<unset>"));
    } else {
        auto it = nodes.find(index);
        if (it != nodes.end()) {
            lv_label_set_text(chIdLabel, lv_label_get_text(it->second->LV_OBJ_IDX(node_lbs_idx)));
            lv_label_set_text(chGroupNameLabel, lv_label_get_text(it->second->LV_OBJ_IDX(node_lbl_idx)));
        } else {
            lv_label_set_text_fmt(chIdLabel, "%04x", index & 0xffff);
            lv_label_set_text_fmt(chGroupNameLabel, "!%08x", index);
        }
    }
    return chatBtn;
}

lv_obj_t *TFTView_480x222::createMessageContainerWidget(lv_obj_t *parent)
{
    int startWidgetIndex = 25; // check out screen.c:tick_screen_widgets()
    create_user_widget_message_container(parent, startWidgetIndex);
    lv_obj_t *obj = ((lv_obj_t **)&objects)[startWidgetIndex + 0];
    lv_obj_set_style_pad_left(obj, 5, 0);
    lv_obj_set_style_pad_right(obj, 5, 0);
    lv_obj_set_style_pad_top(obj, 4, 0);
    lv_obj_set_style_pad_bottom(obj, 4, 0);
    lv_obj_set_style_pad_right(obj, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_width(obj, 3, LV_PART_SCROLLBAR);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_remove_flag(obj, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_CHAIN));
    return obj;
}

void TFTView_480x222::task_handler(void)
{
    PluggableView::task_handler();
}

#endif