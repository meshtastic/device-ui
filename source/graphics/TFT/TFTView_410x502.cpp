#if defined(VIEW_410x502)

#include "graphics/view/TFT/TFTView_410x502.h"
#include "graphics/common/ViewController.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverFactory.h"
#include "graphics/map/MapPanel.h"
#include "images.h"
#include "input/InputDriver.h"
#include "lv_i18n.h"
#include "lvgl.h"
#include "lvgl_private.h"
#include "ui.h"
#include "util/ILog.h"

// TODO: children index of nodepanel lv objects (see addNode)
enum NodePanelIdx { node_img_idx, node_lbs_idx, node_lbl_idx };

#define THIS TFTView_410x502::instance() // need to use THIS in all static methods

TFTView_410x502 *TFTView_410x502::instance(void)
{
    if (!gui)
        gui = new TFTView_410x502(nullptr, DisplayDriverFactory::create(480, 222));
    return static_cast<TFTView_410x502 *>(gui);
}

TFTView_410x502 *TFTView_410x502::instance(const DisplayDriverConfig &cfg)
{
    if (!gui)
        gui = new TFTView_410x502(&cfg, DisplayDriverFactory::create(cfg));
    return static_cast<TFTView_410x502 *>(gui);
}

TFTView_410x502::TFTView_410x502(const DisplayDriverConfig *cfg, DisplayDriver *driver) : PluggableView(cfg, driver) {}

void TFTView_410x502::init(IClientBase *client)
{
    ILOG_DEBUG("TFTView_410x502 init...");
    PluggableView::init(client);
}

void TFTView_410x502::ui_events_init(void)
{
    PluggableView::ui_events_init();
}

/**
 * @brief initialize UI with persistent data
 */
bool TFTView_410x502::setupUIConfig(const meshtastic_DeviceUIConfig &uiconfig)
{
    return PluggableView::setupUIConfig(uiconfig);
}

/**
 * @brief Initialize all screens and apply customizations
 *
 */
void TFTView_410x502::init_screens(void)
{
    PluggableView::init_screens();
}

/**
 * Overwrite the default generated function ui_init()
 * Wire plugin callbacks into existing view actions
 */

void TFTView_410x502::ui_init(void)
{
    PluggableView::ui_init();
}

lv_obj_t *TFTView_410x502::createAddMessageWidget(lv_obj_t *parent, uint32_t msgTime, uint32_t requestId, const char *msg)
{
    ILOG_DEBUG("--> createAddMessageWidget %s", msg);
    char buf[20];
    std::tm date_tm{};
    time_t local = msgTime;
    localtime_r(&local, &date_tm);
    strftime(buf, 20, "%H:%M", &date_tm); // TODO: add short name

    int startWidgetIndex = 17; // check out screen.c:tick_screen_widgets()
    create_user_widget_add_message_entry(parent, startWidgetIndex);
    lv_obj_t *msgBtn = ((lv_obj_t **)&objects)[startWidgetIndex + 0];
    lv_obj_t *timeLabel = ((lv_obj_t **)&objects)[startWidgetIndex + 1];
    lv_obj_t *msgLabel = ((lv_obj_t **)&objects)[startWidgetIndex + 2];
    lv_label_set_text(timeLabel, buf);
    // TODO: set on result: lv_obj_set_style_text_color(timeLabel, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(msgLabel, msg);
    lv_obj_scroll_to_view(msgBtn, LV_ANIM_ON); // TODO: scroll only when not restoring
    return msgBtn;
}

lv_obj_t *TFTView_410x502::createNewMessageWidget(lv_obj_t *parent, uint32_t msgTime, uint32_t nodeNum, uint8_t channel,
                                                  const char *msg)
{
    char buf[20];
    std::tm date_tm{};
    time_t local = msgTime;
    localtime_r(&local, &date_tm);
    strftime(buf, 20, "%H:%M", &date_tm); // TODO: add short name

    int startWidgetIndex = 21; // check out screen.c:tick_screen_widgets()
    create_user_widget_new_message_entry(parent, startWidgetIndex);
    lv_obj_t *msgBtn = ((lv_obj_t **)&objects)[startWidgetIndex + 0];
    lv_obj_t *timeLabel = ((lv_obj_t **)&objects)[startWidgetIndex + 1];
    lv_obj_t *msgLabel = ((lv_obj_t **)&objects)[startWidgetIndex + 2];
    lv_label_set_text(timeLabel, buf);
    lv_label_set_text(msgLabel, msg);
    lv_obj_scroll_to_view(msgBtn, LV_ANIM_OFF); // TODO scroll anim when view is active
    return msgBtn;
}

lv_obj_t *TFTView_410x502::createChatWidget(lv_obj_t *parent, uint32_t index)
{
    int startWidgetIndex = 12; // check out screen.c:tick_screen_widgets()
    create_user_widget_add_chat_entry(parent, startWidgetIndex);
    lv_obj_t *chatBtn = ((lv_obj_t **)&objects)[startWidgetIndex + 0];
    lv_obj_t *chImage = ((lv_obj_t **)&objects)[startWidgetIndex + 1];
    lv_obj_t *chIdLabel = ((lv_obj_t **)&objects)[startWidgetIndex + 2];
    lv_obj_t *chGroupNameLabel = ((lv_obj_t **)&objects)[startWidgetIndex + 3];

    if (index < 8) {
        lv_label_set_text_fmt(chIdLabel, "%d", index);
        lv_label_set_text(chGroupNameLabel, db.channel[index].settings.name);
    } else {
        auto it = nodes.find(index);
        if (it != nodes.end()) {
            lv_label_set_text(chIdLabel, lv_label_get_text(it->second->LV_OBJ_IDX(node_lbs_idx)));
            lv_label_set_text(chGroupNameLabel, lv_label_get_text(it->second->LV_OBJ_IDX(node_lbl_idx)));
        }
    }
    return chatBtn;
}

lv_obj_t *TFTView_410x502::createMessageContainerWidget(lv_obj_t *parent)
{
    int startWidgetIndex = 25; // check out screen.c:tick_screen_widgets()
    create_user_widget_message_container(parent, startWidgetIndex);
    lv_obj_t *obj = ((lv_obj_t **)&objects)[startWidgetIndex + 0];
    return obj;
}

void TFTView_410x502::task_handler(void)
{
    PluggableView::task_handler();
}

#endif