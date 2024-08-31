#include "Themes.h"
#include "stdint.h"

static enum Themes::Theme theme = Themes::eDark;

Themes::Theme Themes::get(void)
{
    return theme;
}

void Themes::set(enum Theme th)
{
    theme = th;
}


enum ThemeColor {
    eMainScreenStyle,
    eTopPanelBg,
    eTopPanelText,
    eTopImageBg,
    eTopImageRecolor,
    eTopImageRecolorOpa,
    ePanelBg,
    ePanelText,
    ePanelBorder,
    eNodePanelBg,
    eNodePanelBorder,
    eNodePanelText,
    eNodeButtonBg,
    eNodeButtonBgOpa,
    eButtonPanelBg,
    eMainButtonBg,
    eMainButtonText,
    eMainButtonBorder,
    eMainButtonShadow,
    eMainButtonImageRecolor,
    eMainButtonImageRecolorOpa,
    eHomeContainerBg,
    eHomeContainerBorder,
    eHomeContainerShadow,
    eHomeContainerText,
    eHomeButtonBg,
    eHomeButtonText,
    eHomeButtonBorder,
    eHomeButtonImageRecolor,
    eHomeButtonImageRecolorOpa,
    eChannelButtonBg,
    eChannelButtonBorder,
    eChannelButtonText,
    eSettingsPanelBg,
    eSettingsPanelText,
    eSettingsPanelBorder,
    eSettingsPanelShadow,
    eSettingsPanelBgOpa,
    eSettingsButtonBg,
    eSettingsButtonText,
    eSettingsButtonBorder,
    eSettingsButtonImageRecolor,
    eSettingsButtonImageRecolorOpa,
    eSettingsLabelBg,
    eSettingsLabelBorder,
    eTabViewBg,
    eTabViewText,
    eTabButtonDefaultBg,
    eTabButtonActiveBg,
    eTabButtonPressedBg,
    eTabButtonDefaultText,
    eTabButtonActiveText,
    eTabButtonPressedText,
    eTabButtonDefaultBorder,
    eChatMessageBg,
    eChatMessageBgOpa,
    eChatMessageText,
    eChatMessageBorder,
    eNewMessageBg,
    eNewMessageBgOpa,
    eNewMessageText,
    eNewMessageBorder,
    eAlertPanelBg,
    eBtnMatrixBorderMain,
    eBtnMatrixBorderItems,
    eBtnMatrixBgItems,
    eBtnMatrixTextItems,
    eColorTextLabel,
    eSpinnerMainArc,
    eSpinnerIndicatorArc
};

uint32_t themeColor[][2] = {
    // light,      dark
    { 0xfff4f4f4, 0xff303030 }, // eMainScreenStyle
    { 0xff67ea94, 0xff436C70 }, // eTopPanelBg
    { 0xff212121, 0xffE0E0E0 }, // eTopPanelText
    { 0xff67ea94, 0xff436C70 }, // eTopImageBg
    { 0xff212121, 0xffffffff }, // eTopImageRecolor
    { 255,        255        }, // eTopImageRecolorOpa
    { 0xfff4f4f4, 0xff303030 }, // ePanelBg
    { 0xff212121, 0xfff0f0f0 }, // ePanelText
    { 0xff67ea94, 0xff67ea94 }, // ePanelBorder
    { 0xfffafaf4, 0xff404040 }, // eNodePanelBg
    { 0xff979797, 0xff808080 }, // eNodePanelBorder
    { 0xff212121, 0xfff0f0f0 }, // eNodePanelText
    { 0xfffffff8, 0xff404040 }, // eNodeButtonBg
    { 0,          0          }, // eNodeButtonBgOpa
    { 0xffffffff, 0xff585858 }, // eButtonPanelBg
    { 0xffeaeae0, 0xff585858 }, // eMainButtonBg
    { 0xff101010, 0xffaafbff }, // eMainButtonText
    { 0xff67ea94, 0xff67ea94 }, // eMainButtonBorder
    { 0xffc0c0c0, 0xff9e9e9e }, // eMainButtonShadow
    { 0xff757575, 0xff67ea94 }, // eMainButtonImageRecolor
    { 255,        0          }, // eMainButtonImageRecolorOpa
    { 0xfffafaf4, 0xff303030 }, // eHomeContainerBg
    { 0xffaaaaaa, 0xff67EA94 }, // eHomeContainerBorder
    { 0xff999999, 0xff2B824A }, // eHomeContainerShadow
    { 0xff294337, 0xffaafbff }, // eHomeContainerText
    { 0xfff4f4ee, 0xff303030 }, // eHomeButtonBg
    { 0xff101010, 0xffffffff }, // eHomeButtonText
    { 0xffd0d0d0, 0xff303030 }, // eHomeButtonBorder
    { 0xff33a2ac, 0xff606060 }, // eHomeButtonImageRecolor
    { 255,        0          }, // eHomeButtonImageRecolorOpa
    { 0xfffafaf4, 0xff404040 }, // eChannelButtonBg
    { 0xffD0D0D0, 0xffA0A0A0 }, // eChannelButtonBorder
    { 0xff101010, 0xffffffff }, // eChannelButtonText
    { 0xfff0f0f0, 0xff303030 }, // eSettingsPanelBg
    { 0xff003c9f, 0xffaafbff }, // eSettingsPanelText
    { 0xff979797, 0          }, // eSettingsPanelBorder
    { 0xff7e7e7e, 0          }, // eSettingsPanelShadow
    { 250,        250        }, // eSettingsPanelBgOpa
    { 0xffeaeae0, 0xff505050 }, // eSettingsButtonBg
    { 0xff294337, 0xffaafbff }, // eSettingsButtonText
    { 0xffd0d0d0, 0xff303030 }, // eSettingsButtonBorder
    { 0xff67ea94, 0          }, // eSettingsButtonImageRecolor
    { 255,        0          }, // eSettingsButtonImageRecolorOpa
    { 0xffffffff, 0xff404040 }, // eSettingsLabelBg
    { 0xff808080, 0xff404040 }, // eSettingsLabelBorder
    { 0xfff4f4f4, 0xff303030 }, // eTabViewBg
    { 0xff003c9f, 0xffaafbff }, // eTabViewText
    { 0xffe0e0e0, 0xff303030 }, // eTabButtonDefaultBg
    { 0xffffffff, 0xff303030 }, // eTabButtonActiveBg
    { 0xffaafbff, 0xff67ea94 }, // eTabButtonPressedBg
    { 0xff606060, 0xffA0A0A0 }, // eTabButtonDefaultText
    { 0xff101010, 0xffffffff }, // eTabButtonActiveText
    { 0xffffffff, 0xffffffff }, // eTabButtonPressedText
    { 0xffb0b0b0, 0xff505050 }, // eTabButtonDefaultBorder
    { 0xfffbfce9, 0xff303030 }, // eChatMessageBg
    { 255,        255,       }, // eChatMessageBgOpa
    { 0xff294337, 0xffffffff }, // eChatMessageText
    { 0xff888888, 0xff707070 }, // eChatMessageBorder
    { 0xffffffff, 0xff404040 }, // eNewMessageBg
    { 255,        255        }, // eNewMessageBgOpa
    { 0xff294337, 0xffd0d0d0 }, // eNewMessageText
    { 0xff888888, 0xff808080 }, // eNewMessageBorder
    { 0xfffbfbfb, 0xff303030 }, // eAlertPanelBg
    { 0xfff4f4f4, 0xff303030 }, // eBtnMatrixBorderMain
    { 0xff67ea94, 0xff67ea94 }, // eBtnMatrixBorderItems
    { 0xfffffff8, 0xff606060 }, // eBtnMatrixBgItems
    { 0xff212121, 0xffaafbff }, // eBtnMatrixTextItems
    { 0xff003c9f, 0xffaafbff }, // eColorTextLabel
    { 0xffe0e0e0, 0xff404040 }, // eSpinnerMainArc
    { 0xff67ea94, 0xff67ea94 }, // eSpinnerIndicatorArc
};


#include "styles.h"
#include "images.h"
#include "fonts.h"

#define THEME(COLOR) (themeColor[COLOR][theme])

// TODO: the following styles are copied from eez-studio generated styles and parametrized
//       because eez-studio currently does not support lv_style
extern "C" {
void apply_style_top_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eTopPanelBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eTopPanelText)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(ePanelBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(ePanelText)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(ePanelBorder)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_home_container_style(lv_obj_t *obj) {
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(eHomeContainerBorder)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eHomeContainerBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(obj, lv_color_hex(THEME(eHomeContainerShadow)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eHomeContainerText)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_settings_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eSettingsPanelBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eSettingsPanelText)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(obj, lv_color_hex(THEME(eSettingsPanelShadow)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(eSettingsPanelBorder)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, THEME(eSettingsPanelBgOpa), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_node_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eNodePanelBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(eNodePanelBorder)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eNodePanelText)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_node_button_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eNodeButtonBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, THEME(eNodeButtonBgOpa), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_button_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eButtonPanelBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_home_button_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eHomeButtonBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(obj, THEME(eHomeButtonImageRecolorOpa), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(THEME(eHomeButtonImageRecolor)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(eHomeButtonBorder)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eHomeButtonText)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_settings_button_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eSettingsButtonBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(obj, THEME(eSettingsButtonImageRecolorOpa), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(THEME(eSettingsButtonImageRecolor)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(eSettingsButtonBorder)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eSettingsButtonText)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_main_button_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_image_recolor_opa(obj, THEME(eMainButtonImageRecolorOpa), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(THEME(eMainButtonImageRecolor)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(eMainButtonBorder)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eMainButtonBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eMainButtonText)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(obj, lv_color_hex(THEME(eMainButtonShadow)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_new_message_style(lv_obj_t *obj) {
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(eNewMessageBorder)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eNewMessageBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eNewMessageText)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, THEME(eNewMessageBgOpa), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_chat_message_style(lv_obj_t *obj) {
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(eChatMessageBorder)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eChatMessageBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eChatMessageText)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, THEME(eChatMessageBgOpa), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_tab_view_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eTabViewBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eTabViewText)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_drop_down_style(lv_obj_t *obj) {
};
void apply_style_bw_label_style(lv_obj_t *obj) {
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_color_label_style(lv_obj_t *obj) {
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eColorTextLabel)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_top_image_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(THEME(eTopImageRecolor)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(obj, THEME(eTopImageRecolorOpa), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(obj, lv_color_hex(THEME(eTopImageRecolor)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor_opa(obj, THEME(eTopImageRecolorOpa), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eTopImageBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_alert_panel_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eAlertPanelBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(ePanelText)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_main_screen_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eMainScreenStyle)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_channel_button_style(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eChannelButtonBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(eChannelButtonBorder)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eChannelButtonText)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_button_matrix_style(lv_obj_t *obj) {
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(eBtnMatrixBorderItems)), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eBtnMatrixBgItems)), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(THEME(eBtnMatrixTextItems)), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eBtnMatrixBorderMain)), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_spinner_style(lv_obj_t *obj) {
    lv_obj_set_style_arc_color(obj, lv_color_hex(THEME(eSpinnerMainArc)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(obj, lv_color_hex(THEME(eSpinnerIndicatorArc)), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(obj, 4, LV_PART_INDICATOR | LV_STATE_DEFAULT);
};
void apply_style_settings_label_style(lv_obj_t *obj) {
    lv_obj_set_style_border_color(obj, lv_color_hex(THEME(eSettingsLabelBorder)), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(THEME(eSettingsLabelBg)), LV_PART_MAIN | LV_STATE_DEFAULT);
};

}

void Themes::initStyles(void)
{
    // lvgl v9 tabview buttons are not btn-matrix anymore but array of buttons
    // see https://forum.lvgl.io/t/style-a-tabview-widget-in-v9-0-0/14747
    lv_style_init(&style_btn_default);
    lv_style_set_text_color(&style_btn_default, lv_color_hex(THEME(eTabButtonDefaultText)));
    lv_style_set_bg_color(&style_btn_default, lv_color_hex(THEME(eTabButtonDefaultBg)));
    lv_style_set_bg_opa(&style_btn_default, LV_OPA_COVER);
    lv_style_set_border_color(&style_btn_default, lv_color_hex(THEME(eTabButtonDefaultBorder)));
    lv_style_set_border_opa(&style_btn_default, LV_OPA_COVER);
    lv_style_set_border_width(&style_btn_default, 1);
    lv_style_set_border_side(&style_btn_default, LV_BORDER_SIDE_FULL);

    lv_style_init(&style_btn_active);
    lv_style_set_text_color(&style_btn_active, lv_color_hex(THEME(eTabButtonActiveText)));
    lv_style_set_bg_color(&style_btn_active, lv_color_hex(THEME(eTabButtonActiveBg)));
    lv_style_set_bg_opa(&style_btn_active, LV_OPA_COVER);
    lv_style_set_border_color(&style_btn_active, lv_color_hex(0xff67ea94));
    lv_style_set_border_opa(&style_btn_active, LV_OPA_COVER);
    lv_style_set_border_width(&style_btn_active, 3);
    lv_style_set_border_side(&style_btn_active, LV_BORDER_SIDE_BOTTOM);

    lv_style_init(&style_btn_pressed);
    lv_style_set_text_color(&style_btn_pressed, lv_color_hex(THEME(eTabButtonPressedText)));
    lv_style_set_bg_color(&style_btn_pressed, lv_color_hex(THEME(eTabButtonPressedBg)));
    lv_style_set_bg_opa(&style_btn_pressed, LV_OPA_COVER);
    lv_style_set_border_color(&style_btn_pressed, lv_color_hex(0xff67ea94));
    lv_style_set_border_opa(&style_btn_pressed, LV_OPA_COVER);
    lv_style_set_border_width(&style_btn_pressed, 3);
    lv_style_set_border_side(&style_btn_pressed, LV_BORDER_SIDE_BOTTOM);
}

void Themes::recolorButton(lv_obj_t* obj, bool enabled, lv_opa_t opa)
{
    lv_color_t color;
    switch(theme) {
    case eLight:
        color = enabled ? lv_color_hex(THEME(eHomeButtonImageRecolor)) : lv_color_hex(0xffc0c0c0);
        break;
    case eDark:
        color = enabled ? lv_color_hex(0xffe0e0e0) : lv_color_hex(0xff606060);
        break;
    default:
        break;
    }
    lv_obj_set_style_bg_image_recolor(obj, color, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(obj, opa, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void Themes::recolorText(lv_obj_t* obj, bool enabled)
{
    lv_color_t color;
    switch(theme) {
    case eLight:
        color = enabled ? lv_color_hex(THEME(eHomeContainerText)) : lv_color_hex(0xffc0c0c0);
        break;
    case eDark:
        color = enabled ? lv_color_hex(THEME(eHomeContainerText)) : lv_color_hex(0xff606060);
        break;
    default:
        break;
    }
    lv_obj_set_style_text_color(obj, color, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void Themes::recolorTopLabel(lv_obj_t* obj, bool alert)
{
    lv_color_t color;
    switch(theme) {
    case eLight:
        color = alert ? lv_color_hex(0xfff72b2b) : lv_color_hex(THEME(eTopPanelText));
        break;
    case eDark:
        color = alert ? lv_color_hex(0xfff72b2b) : lv_color_hex(THEME(eTopPanelText));
        break;
    default:
        break;
    }
    lv_obj_set_style_text_color(obj, color, LV_PART_MAIN | LV_STATE_DEFAULT);
}
