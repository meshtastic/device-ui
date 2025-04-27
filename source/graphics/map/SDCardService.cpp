#include "lvgl.h"

#include "graphics/map/MapTileSettings.h"
#include "graphics/map/SDCardService.h"
#include "util/ILog.h"

#ifdef ARCH_PORTDUINO
#include "PortduinoFS.h"
static fs::FS &SD = PortduinoFS; // Portduino does not (yet) support SD device, use normal file system
#elif defined(HAS_SD_MMC)
#include "SD_MMC.h"
static fs::SDMMCFS &SD = SD_MMC;
#else
#include "SD.h"
#endif

#define DRIVE_LETTER "S"

SDCardService::SDCardService() : ITileService(DRIVE_LETTER ":")
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

SDCardService::~SDCardService()
{
#ifndef ARCH_PORTDUINO
    SD.end();
#endif
}

bool SDCardService::load(uint32_t x, uint32_t y, uint32_t z, void *img)
{
    char buf[128] = DRIVE_LETTER ":";
    sprintf(&buf[2], "%s/%s/%d/%d/%d.%s", MapTileSettings::getPrefix(), MapTileSettings::getTileStyle(), z, x, y,
            MapTileSettings::getTileFormat());
    ILOG_DEBUG("SDCardService::load(): %s", buf);
    lv_image_set_src((lv_obj_t *)img, buf);
    if (!lv_image_get_src((lv_obj_t *)img)) {
        ILOG_DEBUG("Failed to load tile %s from SD", buf);
        return false;
    }
    ILOG_INFO("SDCardService: Tile %s loaded.", buf);
    return true;
}

void *SDCardService::fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    String s(path);
    File file = SD.open(path, mode == LV_FS_MODE_RD ? FILE_READ : FILE_WRITE);
    if (!file) {
        // ILOG_WARN("SD.open() %s failed!", path);
        return nullptr;
    } else {
        // ILOG_DEBUG("SD.open() %s ok", path);
        SdFile *lf = new SdFile{file};
        return static_cast<void *>(lf);
    }
}

lv_fs_res_t SDCardService::fs_close(lv_fs_drv_t *drv, void *file_p)
{
    // ILOG_DEBUG("SD.close()");
    SdFile *lf = static_cast<SdFile *>(file_p);
    lf->file.close();
    delete lf;
    return LV_FS_RES_OK;
}

lv_fs_res_t SDCardService::fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    *br = static_cast<SdFile *>(file_p)->file.read((uint8_t *)buf, btr);
    // ILOG_DEBUG("SD.read(): %d/%d bytes", *br, btr);
    return (*br <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t SDCardService::fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    *bw = static_cast<SdFile *>(file_p)->file.write((uint8_t *)buf, btw);
    // ILOG_DEBUG("SD.write(): %d/btw bytes", *bw, btw);
    return (*bw <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t SDCardService::fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    // ILOG_DEBUG("SD.seek(): pos %d", pos);
    return static_cast<SdFile *>(file_p)->file.seek(pos, (SeekMode)whence) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

lv_fs_res_t SDCardService::fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    *pos_p = static_cast<SdFile *>(file_p)->file.position();
    // ILOG_DEBUG("SD.tell(): pos %d", *pos_p);
    return (int32_t)(*pos_p) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

void *SDCardService::fs_dir_open(lv_fs_drv_t *drv, const char *path)
{
    return nullptr; // TODO
}

lv_fs_res_t SDCardService::fs_dir_read(lv_fs_drv_t *drv, void *rddir_p, char *fn, uint32_t fn_len)
{
    return LV_FS_RES_NOT_IMP; // TODO
}

lv_fs_res_t SDCardService::fs_dir_close(lv_fs_drv_t *drv, void *rddir_p)
{
    return LV_FS_RES_NOT_IMP; // TODO
}
