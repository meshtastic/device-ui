#if defined(VIEW_480x222)

#include "graphics/view/TFT/TFTView_480x222.h"
#include "graphics/common/ViewController.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverFactory.h"
#include "graphics/map/MapPanel.h"
#include "images.h"
#include "input/InputDriver.h"
#include "lv_i18n.h"
#include "lvgl.h"
#include "ui.h"
#include "util/ILog.h"

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

lv_obj_t *TFTView_480x222::createAddMessageWidget(lv_obj_t *parent, uint32_t msgTime, uint32_t requestId, const char *msg)
{
    char buf[20];
    std::tm date_tm{};
    time_t local = msgTime;
    localtime_r(&local, &date_tm);
    strftime(buf, 20, "%H:%M", &date_tm); // TODO: add short name

    int startWidgetIndex = 12;
    create_user_widget_add_message_entry(parent, startWidgetIndex);
    lv_obj_t *msgBtn = ((lv_obj_t **)&objects)[startWidgetIndex + 0];
    lv_obj_t *timeLabel = ((lv_obj_t **)&objects)[startWidgetIndex + 1];
    lv_obj_t *msgLabel = ((lv_obj_t **)&objects)[startWidgetIndex + 2];
    lv_label_set_text(timeLabel, buf);
    // TODO: set on result: lv_obj_set_style_text_color(timeLabel, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(msgLabel, msg);
    lv_obj_scroll_to_view(msgBtn, LV_ANIM_ON); // TODO: scroll only when not restoring
    return parent;
}

lv_obj_t *TFTView_480x222::createNewMessageWidget(lv_obj_t *parent, uint32_t nodeNum, uint8_t channel, const char *msg)
{
    lv_obj_t *obj = lv_btn_create(parent);
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 80, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "18:41");
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 455, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 40, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, msg);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404b5c), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff939393), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_width(obj, 430, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
    return obj;
}

lv_obj_t *TFTView_480x222::createChatWidget(lv_obj_t *parent, uint32_t from, uint32_t to, uint8_t ch)
{
    return parent;
}

void TFTView_480x222::task_handler(void)
{
    PluggableView::task_handler();
}

#endif