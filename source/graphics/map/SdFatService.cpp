#if defined(HAS_SDCARD) && not defined(HAS_SD_MMC) && not defined(ARCH_PORTDUINO)

#include "lvgl.h"

#include "graphics/common/SdCard.h"
#include "graphics/map/MapTileSettings.h"
#include "graphics/map/SdFatService.h"
#include "util/ILog.h"
#include <utility>

#define DRIVE_LETTER "S"

SdFatService::SdFatService() : ITileService(DRIVE_LETTER ":")
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
    drv.dir_open_cb = fs_dir_open;
    drv.dir_read_cb = fs_dir_read;
    drv.dir_close_cb = fs_dir_close;
    lv_fs_drv_register(&drv);
}

SdFatService::~SdFatService()
{
    SDFs.end();
}

bool SdFatService::load(const char *name, void *img)
{
    char buf[128] = DRIVE_LETTER ":";
    strcat(&buf[2], name);
    // ILOG_DEBUG("SdFatService::load(): %s", buf);
    lv_image_set_src((lv_obj_t *)img, buf);
    if (!lv_image_get_src((lv_obj_t *)img)) {
        ILOG_DEBUG("Failed to load tile %s from SD", buf);
        return false;
    }
    // ILOG_INFO("*** Tile %s loaded.", buf);
    return true;
}

void *SdFatService::fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    String s(path);
    SdFile *lf = new SdFile;
    lf->file = SDFs.open(path, mode == LV_FS_MODE_RD ? O_RDONLY : O_WRONLY); // NOTE: O_RDWR
    if (!lf->file) {
        // ILOG_DEBUG("FsSD.open() %s failed!", path);
        delete lf;
        return nullptr;
    } else {
        return static_cast<void *>(lf);
    }
}

lv_fs_res_t SdFatService::fs_close(lv_fs_drv_t *drv, void *file_p)
{
    // ILOG_DEBUG("FsSD.close()");
    SdFile *lf = static_cast<SdFile *>(file_p);
    lf->file.close();
    delete lf;
    return LV_FS_RES_OK;
}

lv_fs_res_t SdFatService::fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    *br = static_cast<SdFile *>(file_p)->file.read((uint8_t *)buf, btr);
    // ILOG_DEBUG("FsSD.read(): %d/%d bytes", *br, btr);
    return (*br <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t SdFatService::fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    *bw = static_cast<SdFile *>(file_p)->file.write((uint8_t *)buf, btw);
    // ILOG_DEBUG("FsSD.write(): %d/btw bytes", *bw, btw);
    return (*bw <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t SdFatService::fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    // ILOG_DEBUG("FsSD.seek(): pos %d", pos);
    if (whence == LV_FS_SEEK_SET) {
        return static_cast<SdFile *>(file_p)->file.seekSet(pos) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
    } else if (whence == LV_FS_SEEK_END) {
        return static_cast<SdFile *>(file_p)->file.seekEnd() ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
    } else {
        return static_cast<SdFile *>(file_p)->file.seekCur(pos) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
    }
}

lv_fs_res_t SdFatService::fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    *pos_p = static_cast<SdFile *>(file_p)->file.position();
    // ILOG_DEBUG("FsSD.tell(): pos %d", *pos_p);
    return (int32_t)(*pos_p) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

void *SdFatService::fs_dir_open(lv_fs_drv_t *drv, const char *path)
{
    return nullptr; // TODO
}

lv_fs_res_t SdFatService::fs_dir_read(lv_fs_drv_t *drv, void *rddir_p, char *fn, uint32_t fn_len)
{
    return LV_FS_RES_NOT_IMP; // TODO
}

lv_fs_res_t SdFatService::fs_dir_close(lv_fs_drv_t *drv, void *rddir_p)
{
    return LV_FS_RES_NOT_IMP; // TODO
}

#endif