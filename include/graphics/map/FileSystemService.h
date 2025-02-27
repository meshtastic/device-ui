#pragma once

#include "graphics/map/TileService.h"
#include "lvgl.h"

class FileSystemService : public ITileService
{
  public:
    FileSystemService();

    virtual ~FileSystemService();

    bool load(const char *name, void *img) override;

  protected:
    static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode);
    static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p);
    static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);
    static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw);
    static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence);
    static lv_fs_res_t fs_size(lv_fs_drv_t *drv, void *file_p, uint32_t *size_p);
    static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);
};
