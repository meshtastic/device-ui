#include "graphics/map/FileSystemService.h"
#include "FS.h"
#include "graphics/map/MapTileSettings.h"
#include "lvgl.h"
#include "screens.h"
#include "util/ILog.h"
#include <cstring>
#include <stdio.h>

#define DRIVE_LETTER "A"

LV_IMAGE_DECLARE(img_no_tile_image);

FileSystemService::FileSystemService() : ITileService(DRIVE_LETTER ":")
{
    static lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);
    drv.letter = DRIVE_LETTER[0];
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

FileSystemService::~FileSystemService() {}

bool FileSystemService::load(uint32_t x, uint32_t y, uint32_t z, void *img)
{
    char buf[128] = DRIVE_LETTER ":";
    sprintf(&buf[2], "%s/%s/%d/%d/%d.%s", MapTileSettings::getPrefix(), MapTileSettings::getTileStyle(), z, x, y,
            MapTileSettings::getTileFormat());
    lv_image_set_src((lv_obj_t *)img, buf);
    if (!lv_image_get_src((lv_obj_t *)img)) {
        lv_image_set_src((lv_obj_t *)img, &img_no_tile_image);
        ILOG_WARN("failed to load tile %s", buf);
        return false;
    }
    ILOG_DEBUG("FileSystemService: tile %s loaded", buf);
    return true;
}

void *FileSystemService::fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    ILOG_DEBUG("fs_open %s", path);
    return fopen(path, mode == LV_FS_MODE_RD ? "rb" : "wb");
}

lv_fs_res_t FileSystemService::fs_close(lv_fs_drv_t *drv, void *file_p)
{
    ILOG_DEBUG("fs_close()");
    return fclose((FILE *)file_p) != 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t FileSystemService::fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    *br = fread(buf, 1, btr, (FILE *)file_p);
    ILOG_DEBUG("fs_read(): %d/%d bytes", *br, btr);
    return (*br <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t FileSystemService::fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    *bw = fwrite(buf, 1, btw, (FILE *)file_p);
    ILOG_DEBUG("fs_write(): %d/%d bytes", *bw, btw);
    return (*bw <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t FileSystemService::fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    ILOG_DEBUG("fs_seek(): pos %d", pos);
    return fseek((FILE *)file_p, pos, whence) != 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t FileSystemService::fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    *pos_p = ftell((FILE *)file_p);
    ILOG_DEBUG("fs_tell(): pos %d", *pos_p);
    return LV_FS_RES_OK;
}
