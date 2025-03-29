#pragma once

#include "FS.h"
#include "lvgl.h"

#define FL_DRIVE_LETTER "L:"

class FileLoader
{
  public:
    FileLoader(){};
    static void init(fs::FS *fs);
    static bool loadImage(lv_obj_t *img, const char *path);
    static bool loadBootImage(lv_obj_t *img);

  private:
    static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode);
    static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p);
    static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);
    static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw);
    static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence);
    static lv_fs_res_t fs_size(lv_fs_drv_t *drv, void *file_p, uint32_t *size_p);
    static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);

    typedef struct FileHandle {
        File file;
    } FileHandle;

    static fs::FS *_fs;
};
