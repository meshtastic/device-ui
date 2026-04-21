#include "graphics/map/SDMMCCardService.h"

#if defined(HAS_SD_MMC) && defined(CONFIG_IDF_TARGET_ESP32P4)

#include "graphics/map/MapTileSettings.h"
#include "util/ILog.h"
#include <cerrno>
#include <cstring>
#include <string>
#include <sys/stat.h>

#define DRIVE_LETTER "S"

static std::string toMountedPath(const char *path)
{
    if (!path || !*path) {
        return std::string("/sdcard");
    }
    if (path[0] == '/') {
        return std::string("/sdcard") + path;
    }
    return std::string("/sdcard/") + path;
}

static bool ensureDirectoryExists(const char *dir)
{
    if (!dir || !*dir) {
        return false;
    }

    std::string path = toMountedPath(dir);
    if (path.empty()) {
        return false;
    }

    size_t pos = strlen("/sdcard");
    while ((pos = path.find('/', pos + 1)) != std::string::npos) {
        std::string partial = path.substr(0, pos);
        if (::mkdir(partial.c_str(), 0775) != 0 && errno != EEXIST) {
            return false;
        }
    }

    if (::mkdir(path.c_str(), 0775) != 0 && errno != EEXIST) {
        return false;
    }

    return true;
}

SDMMCCardService::SDMMCCardService() : ITileService(DRIVE_LETTER ":")
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

SDMMCCardService::~SDMMCCardService() {}

bool SDMMCCardService::load(const char *name, void *img)
{
    char buf[128] = DRIVE_LETTER ":";
    strcat(&buf[2], name);
    ILOG_DEBUG("SDMMCCardService::load(): %s", buf);
    lv_image_set_src((lv_obj_t *)img, buf);
    if (!lv_image_get_src((lv_obj_t *)img)) {
        ILOG_DEBUG("Failed to load tile %s from SD", buf);
        return false;
    }
    return true;
}

bool SDMMCCardService::save(const char *name, void *img, size_t len)
{
    ILOG_DEBUG("SDMMCCardService::save(%s): %d", name, len);
    std::string directory;
    std::string filename(name);
    const size_t last_slash_idx = filename.rfind('/');
    if (std::string::npos == last_slash_idx) {
        return false;
    }
    directory = filename.substr(0, last_slash_idx);

    if (!ensureDirectoryExists(directory.c_str())) {
        ILOG_ERROR("failed to create directory %s", directory.c_str());
        return false;
    }

    std::string mounted = toMountedPath(name);
    FILE *file = fopen(mounted.c_str(), "wb");
    if (file) {
        size_t written = fwrite(static_cast<uint8_t *>(img), 1, len, file);
        fclose(file);
        return written == len;
    }
    ILOG_ERROR("failed to write %s", mounted.c_str());
    return false;
}

void *SDMMCCardService::fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    std::string mounted = toMountedPath(path);
    FILE *file = fopen(mounted.c_str(), mode == LV_FS_MODE_RD ? "rb" : "wb");
    if (!file) {
        return nullptr;
    }
    SdFile *lf = new SdFile{file};
    return static_cast<void *>(lf);
}

lv_fs_res_t SDMMCCardService::fs_close(lv_fs_drv_t *drv, void *file_p)
{
    SdFile *lf = static_cast<SdFile *>(file_p);
    fclose(lf->file);
    delete lf;
    return LV_FS_RES_OK;
}

lv_fs_res_t SDMMCCardService::fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    *br = fread(buf, 1, btr, static_cast<SdFile *>(file_p)->file);
    return (*br <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t SDMMCCardService::fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    *bw = fwrite(buf, 1, btw, static_cast<SdFile *>(file_p)->file);
    return (*bw <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t SDMMCCardService::fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    return fseek(static_cast<SdFile *>(file_p)->file, pos, whence) == 0 ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

lv_fs_res_t SDMMCCardService::fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    long pos = ftell(static_cast<SdFile *>(file_p)->file);
    if (pos < 0) {
        return LV_FS_RES_UNKNOWN;
    }
    *pos_p = static_cast<uint32_t>(pos);
    return (int32_t)(*pos_p) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

void *SDMMCCardService::fs_dir_open(lv_fs_drv_t *drv, const char *path)
{
    return nullptr; // TODO
}

lv_fs_res_t SDMMCCardService::fs_dir_read(lv_fs_drv_t *drv, void *rddir_p, char *fn, uint32_t fn_len)
{
    return LV_FS_RES_NOT_IMP; // TODO
}

lv_fs_res_t SDMMCCardService::fs_dir_close(lv_fs_drv_t *drv, void *rddir_p)
{
    return LV_FS_RES_NOT_IMP; // TODO
}

#endif
