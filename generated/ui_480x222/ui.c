#if defined(EEZ_FOR_LVGL)
#include <eez/core/vars.h>
#endif

#include "ui.h"
#include "screens.h"
#include "images.h"
#include "actions.h"
#include "vars.h"







#if defined(EEZ_FOR_LVGL)

void ui_init() {
    eez_flow_init(assets, sizeof(assets), (lv_obj_t **)&objects, sizeof(objects), images, sizeof(images), actions);
}

void ui_tick() {
    eez_flow_tick();
    tick_screen(g_currentScreen);
}

#else

#include <string.h>

static int16_t currentScreen = -1;

static lv_obj_t *getLvglObjectFromIndex(int32_t index) {
    if (index == -1) {
        return 0;
    }
    return ((lv_obj_t **)&objects)[index];
}

void loadScreen(enum ScreensEnum screenId) {
    currentScreen = screenId - 1;
    lv_obj_t *screen = getLvglObjectFromIndex(currentScreen);
    lv_scr_load_anim(screen, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
}

void ui_init_boot() {
    //lv_disp_t *dispp = lv_disp_get_default();
    //lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    //lv_disp_set_theme(dispp, theme);
    
    create_screen_boot();
    //create_screen_blank();
    loadScreen(SCREEN_ID_BOOT);
}

void ui_init() {
    //create_screens();
    create_screen_menu();
    create_screen_home();
    create_screen_nodes();
    create_screen_groups();
    create_screen_chats();
    create_screen_map();
    create_screen_clock();
    loadScreen(SCREEN_ID_MENU);
}

void ui_tick() {
    tick_screen(currentScreen);
}

#endif
