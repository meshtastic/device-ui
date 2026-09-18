#include "graphics/map/URLService.h"
#include "graphics/map/MapTileSettings.h"

#if 0 // clashes with wifi client in firmware and portduino layer
#include <ArduinoHttpClient.h>
#include <Ethernet.h>
#include <WiFi.h>
#endif

URLService::URLService() : ITileService("U:")
{
    static lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);
    drv.letter = 'U';
    drv.cache_size = MapTileSettings::getCacheSize();
    drv.ready_cb = nullptr;
    drv.open_cb = fs_open;
    drv.close_cb = fs_close;
    drv.read_cb = fs_read;
    drv.write_cb = fs_write;
    drv.seek_cb = fs_seek;
    drv.tell_cb = fs_tell;
    lv_fs_drv_register(&drv);
}

URLService::~URLService() {}

bool URLService::load(const char *name, void *img)
{
    return false; // TODO
}

void *URLService::fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    return nullptr; // TODO
}

lv_fs_res_t URLService::fs_close(lv_fs_drv_t *drv, void *file_p)
{
    return LV_FS_RES_NOT_IMP;
}

lv_fs_res_t URLService::fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    return LV_FS_RES_NOT_IMP;
}

lv_fs_res_t URLService::fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    return LV_FS_RES_NOT_IMP;
}

lv_fs_res_t URLService::fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    return LV_FS_RES_NOT_IMP;
}

lv_fs_res_t URLService::fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    return LV_FS_RES_NOT_IMP;
}
