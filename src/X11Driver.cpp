#ifdef USE_X11
#include "X11Driver.h"
#include "lvgl.h"
//#include "lv_drv_conf.h"
#include "x11/x11.h"  // lvgl/lv_drivers repository
#include <unistd.h>
#include <pthread.h>


const uint32_t screenWidth = DISP_HOR_RES;
const uint32_t screenHeight = DISP_VER_RES;

static pthread_t thr_tick;     /* thread */
static bool end_tick = false;  /* flag to terminate thread */

X11Driver* X11Driver::x11driver = nullptr;

X11Driver& X11Driver::create(void) {
    if (!x11driver)
        x11driver = new X11Driver;
    return *x11driver;
}

X11Driver::X11Driver() : DisplayDriver (screenWidth, screenHeight) {

}

void X11Driver::init(void) {
  // Initialize LVGL 
  lv_init();

  // Initialize the HAL (display, input devices, tick) for LVGL
  init_hal();
  
}


void X11Driver::init_hal(void) {
  /* mouse input device */
  static lv_indev_drv_t indev_drv_1;
  lv_indev_drv_init(&indev_drv_1);
  indev_drv_1.type = LV_INDEV_TYPE_POINTER;

  /* keyboard input device */
  static lv_indev_drv_t indev_drv_2;
  lv_indev_drv_init(&indev_drv_2);
  indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;

  /* mouse scroll wheel input device */
  static lv_indev_drv_t indev_drv_3;
  lv_indev_drv_init(&indev_drv_3);
  indev_drv_3.type = LV_INDEV_TYPE_ENCODER;

  lv_group_t *g = lv_group_create();
  lv_group_set_default(g);

  lv_disp_t *disp = NULL;


  lv_x11_init("Meshtastic", DISP_HOR_RES, DISP_VER_RES);

  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[DISP_HOR_RES * 100];
  static lv_color_t buf1_2[DISP_HOR_RES * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, DISP_HOR_RES * 100);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = lv_x11_flush;
  disp_drv.hor_res = DISP_HOR_RES;
  disp_drv.ver_res = DISP_VER_RES;
  disp_drv.antialiasing = 1;

  disp = lv_disp_drv_register(&disp_drv);

  /* Add the input device driver */
  indev_drv_1.read_cb = lv_x11_get_pointer;
  indev_drv_2.read_cb = lv_x11_get_keyboard;
  indev_drv_3.read_cb = lv_x11_get_mousewheel;

  /* Set diplay theme */
  lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);

  /* Tick init */
  end_tick = false;
  pthread_create(&thr_tick, NULL, tick_thread, NULL);

  /* register input devices */
  lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);
  lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
  lv_indev_t *enc_indev = lv_indev_drv_register(&indev_drv_3);
  lv_indev_set_group(kb_indev, g);
  lv_indev_set_group(enc_indev, g);

  /* Set a cursor for the mouse */
  LV_IMG_DECLARE(mouse_cursor_icon);                   /*Declare the image file.*/
  lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor*/
  lv_img_set_src(cursor_obj, &mouse_cursor_icon);      /*Set the image source*/
  lv_indev_set_cursor(mouse_indev, cursor_obj);        /*Connect the image  object to the driver*/
}


void X11Driver::hal_deinit(void) {
  end_tick = true;
  pthread_join(thr_tick, NULL);
  lv_x11_deinit();
}


X11Driver::~X11Driver() {
  hal_deinit();
}


/**
 * A task to measure the elapsed time for LVGL
 * Note: needs LV_TICK_CUSTOM=0 in lv_conf.h
 * @param data unused
 * @return never return
 */
void* X11Driver::tick_thread(void *data) {
  (void)data;

  while(!end_tick) {
    usleep(5000);
#if !LV_TICK_CUSTOM
    lv_tick_inc(5); // tell LittelvGL that 5 milliseconds were elapsed
#else
  // TODO
#endif
  }
  return NULL;
}


#endif