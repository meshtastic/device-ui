#if defined(HAS_SDCARD) && not defined(HAS_SD_MMC) && not defined(ARCH_PORTDUINO) && not defined(SDCARD_SHARE_SPI)

#include "lvgl.h"
#include "util/ISpiLock.h"

#include "graphics/common/SdCard.h"
#include "graphics/map/MapTileSettings.h"
#include "graphics/map/SdFatService.h"
#include "util/ILog.h"
#include "util/PNGDecoder.h"
#include <cstring>
#include <string>
#include <utility>

#define DRIVE_LETTER "S"

static bool ensureParentDirectories(const char *path)
{
    std::string fullPath(path);
    const size_t lastSlash = fullPath.rfind('/');
    if (lastSlash == std::string::npos || lastSlash == 0) {
        return true;
    }

    const std::string directory = fullPath.substr(0, lastSlash);
    size_t searchPos = 1;
    while (searchPos <= directory.size()) {
        const size_t slashPos = directory.find('/', searchPos);
        const std::string currentDir = slashPos == std::string::npos ? directory : directory.substr(0, slashPos);
        if (!currentDir.empty() && !SDFs.exists(currentDir.c_str()) && !SDFs.mkdir(currentDir.c_str())) {
            ILOG_ERROR("failed to create directory %s", currentDir.c_str());
            return false;
        }
        if (slashPos == std::string::npos) {
            break;
        }
        searchPos = slashPos + 1;
    }

    return true;
}

SdFatService::SdFatService() : ITileService(DRIVE_LETTER ":")
{
#if defined(LV_USE_LODEPNG) && LV_USE_LODEPNG
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
#else
    initPNGDecoder();
#endif
}

SdFatService::~SdFatService()
{
    ISpiLock::Guard bus;
    SDFs.end();
}

bool SdFatService::load(const char *name, void *img)
{
    uint32_t start = millis();
#if defined(LV_USE_LODEPNG) && LV_USE_LODEPNG
    char tilePath[128] = DRIVE_LETTER ":";
    strncat(&tilePath[2], name, sizeof(tilePath) - 3);
    // ILOG_DEBUG("SdFatService::load(): %s", tilePath);
    lv_image_set_src((lv_obj_t *)img, tilePath);
    if (!lv_image_get_src((lv_obj_t *)img)) {
        ILOG_DEBUG("Failed to load tile %s from SD", tilePath);
        return false;
    }
#else
    // optimized PNGdec decoding
    FsFile file = SDFs.open(name, O_RDONLY);
    if (!file) {
        ILOG_DEBUG("Failed to open tile %s from SD", name);
        return false;
    }

    size_t len = (size_t)file.size();
    if (len == 0) {
        ILOG_DEBUG("Tile %s is empty", name);
        file.close();
        return false;
    }

    uint8_t *pngImage = (uint8_t *)lv_malloc(len);
    if (!pngImage) {
        ILOG_ERROR("lv_malloc failed for %s (%u bytes)", name, (unsigned int)len);
        file.close();
        return false;
    }

    size_t bytesRead = file.read(pngImage, len);
    file.close();
    if (bytesRead != len) {
        ILOG_ERROR("read error %s : %u != %u", name, (unsigned int)bytesRead, (unsigned int)len);
        lv_free(pngImage);
        return false;
    }

    lv_img_dsc_t *img_dsc = nullptr;
    bool decoded = MapTileSettings::color() ? decodeImgColor(pngImage, len, &img_dsc) : decodeImgGrey(pngImage, len, &img_dsc);
    lv_free(pngImage);

    if (decoded) {
        lv_obj_t *img_obj = (lv_obj_t *)img;
        lv_image_set_src(img_obj, img_dsc);
        if (lv_image_get_src(img_obj) != img_dsc) {
            ILOG_ERROR("lv_image_set_src failed for tile %s", name);
            if (img_dsc->data && img_dsc->data_size > 0) {
                lv_free((void *)img_dsc->data);
            }
            lv_free(img_dsc);
            return false;
        }
    } else {
        ILOG_ERROR("Failed to decode tile image %s", name);
        return false;
    }
#endif
    ILOG_DEBUG("Tile %s loaded in %d ms.", name, millis() - start);
    return true;
}

bool SdFatService::save(const char *name, void *img, size_t len)
{
    ILOG_DEBUG("SdFatService::save(%s): %d", name, len);
    if (!ensureParentDirectories(name)) {
        return false;
    }

    // write image
    FsFile file = SDFs.open(name, O_RDWR | O_CREAT);
    if (file) {
        size_t written = file.write(static_cast<uint8_t *>(img), len);
        file.close();
        return written == len;
    } else {
        ILOG_ERROR("failed to write %s", name);
    }
    return false;
}

void *SdFatService::fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    ISpiLock::Guard bus;
    String s(path);
    SdFile *lf = new SdFile;
    lf->file = SDFs.open(path, mode == LV_FS_MODE_RD ? O_RDONLY : O_WRONLY); // NOTE: O_RDWR
    if (!lf->file) {
        // ILOG_DEBUG("SDFs.open() %s failed!", path);
        delete lf;
        return nullptr;
    } else {
        return static_cast<void *>(lf);
    }
}

lv_fs_res_t SdFatService::fs_close(lv_fs_drv_t *drv, void *file_p)
{
    ISpiLock::Guard bus;
    // ILOG_DEBUG("SDFs.close()");
    SdFile *lf = static_cast<SdFile *>(file_p);
    lf->file.close();
    delete lf;
    return LV_FS_RES_OK;
}

lv_fs_res_t SdFatService::fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    ISpiLock::Guard bus;
    *br = static_cast<SdFile *>(file_p)->file.read((uint8_t *)buf, btr);
    // ILOG_DEBUG("SDFs.read(): %d/%d bytes", *br, btr);
    return (*br <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t SdFatService::fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    ISpiLock::Guard bus;
    *bw = static_cast<SdFile *>(file_p)->file.write((uint8_t *)buf, btw);
    // ILOG_DEBUG("SDFs.write(): %d/btw bytes", *bw, btw);
    return (*bw <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

lv_fs_res_t SdFatService::fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    ISpiLock::Guard bus;
    // ILOG_DEBUG("SDFs.seek(): pos %d", pos);
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
    ISpiLock::Guard bus;
    *pos_p = static_cast<SdFile *>(file_p)->file.position();
    // ILOG_DEBUG("SDFs.tell(): pos %d", *pos_p);
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