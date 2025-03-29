#include "util/FileLoader.h"
#include "lvgl_private.h"
#include "util/ILog.h"

fs::FS *FileLoader::_fs = nullptr;

void FileLoader::init(fs::FS *fs)
{
    _fs = fs;
    static lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);

    drv.letter = FL_DRIVE_LETTER[0];
    drv.cache_size = 32768;
    drv.ready_cb = nullptr;
    drv.open_cb = fs_open;
    drv.close_cb = fs_close;
    drv.read_cb = fs_read;
    drv.write_cb = fs_write;
    drv.seek_cb = fs_seek;
    drv.tell_cb = fs_tell;
    drv.dir_open_cb = nullptr;
    drv.dir_read_cb = nullptr;
    drv.dir_close_cb = nullptr;
    lv_fs_drv_register(&drv);
}

void *FileLoader::fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    File file = _fs->open(path, mode == LV_FS_MODE_RD ? FILE_READ : FILE_WRITE);
    if (!file) {
        return nullptr;
    } else {
        FileHandle *lf = new FileHandle{file};
        return static_cast<void *>(lf);
    }
}

lv_fs_res_t FileLoader::fs_close(lv_fs_drv_t *drv, void *file_p)
{
    if (file_p == nullptr) {
        return LV_FS_RES_INV_PARAM;
    }
    FileHandle *lf = static_cast<FileHandle *>(file_p);
    if (lf->file) {
        lf->file.close();
        delete lf;
    }
    return LV_FS_RES_OK;
}

lv_fs_res_t FileLoader::fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    if (file_p == nullptr) {
        return LV_FS_RES_INV_PARAM;
    }
    *br = static_cast<FileHandle *>(file_p)->file.read((uint8_t *)buf, btr);
    return (*br <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t FileLoader::fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    if (file_p == nullptr) {
        return LV_FS_RES_INV_PARAM;
    }
    *bw = static_cast<FileHandle *>(file_p)->file.write((uint8_t *)buf, btw);
    return (*bw <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t FileLoader::fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    if (file_p == nullptr) {
        return LV_FS_RES_INV_PARAM;
    }
    return static_cast<FileHandle *>(file_p)->file.seek(pos, (SeekMode)whence) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

lv_fs_res_t FileLoader::fs_size(lv_fs_drv_t *drv, void *file_p, uint32_t *size_p)
{
    if (file_p == nullptr) {
        return LV_FS_RES_INV_PARAM;
    }
    *size_p = static_cast<FileHandle *>(file_p)->file.size();
    return LV_FS_RES_OK;
}

lv_fs_res_t FileLoader::fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    if (file_p == nullptr) {
        return LV_FS_RES_INV_PARAM;
    }
    *pos_p = static_cast<FileHandle *>(file_p)->file.position();
    return (int32_t)(*pos_p) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

bool FileLoader::loadImage(lv_obj_t *img, const char *path)
{
    lv_image_set_src(img, path);
    return lv_image_get_src((lv_obj_t *)img) != nullptr;
}

bool FileLoader::loadBootImage(lv_obj_t *img)
{
    lv_image_set_src(img, FL_DRIVE_LETTER "/boot/logo.png");
    return lv_image_get_src((lv_obj_t *)img) != nullptr;
}
