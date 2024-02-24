#pragma once
#include "DisplayDriver.h"


/**
 * @brief For simulation on pc/raspberry
 * This class provides an X11 GUI on the local desktop; dimensions are defined in lv_drv_conf.h 
 * Usage: define USE_X11=1 for the rasbian/portduino target and link with -lX11
 */
class X11Driver : public DisplayDriver {
public:
    static X11Driver& create(void);
    virtual void init(void);
    virtual ~X11Driver();

private:
    X11Driver();

    static void* tick_thread(void *data);

    void init_hal(void);
    void hal_deinit(void);

    static X11Driver* x11driver;
};