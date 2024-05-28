#include "ui.h"
#include "images.h"

static int16_t currentScreen = -1;

static lv_obj_t *getLvglObjectFromIndex(int32_t index) {
    if (index == -1) {
        return 0;
    }
    return ((lv_obj_t **)&objects)[index];
}

static const void *getLvglImageByName(const char *name) {
    for (size_t imageIndex = 0; imageIndex < sizeof(images) / sizeof(ext_img_desc_t); imageIndex++) {
        if (strcmp(images[imageIndex].name, name) == 0) {
            return images[imageIndex].img_dsc;
        }
    }
    return 0;
}

void loadScreen(enum ScreensEnum screenId, uint32_t delay) {
    currentScreen = screenId - 1;
    lv_obj_t *screen = getLvglObjectFromIndex(currentScreen);
    lv_screen_load_anim(screen, LV_SCR_LOAD_ANIM_NONE, 200, delay, true);
}

void ui_init() {
    create_screens();
    create_tabview_settings();
#ifdef OPEN_SAUCE
    loadScreen(SCREEN_ID_OPEN_SAUCE_SCREEN, 0);
#else
    loadScreen(SCREEN_ID_BOOT_SCREEN, 0);
#endif
}

void ui_tick() {
    tick_screen(currentScreen);
}
