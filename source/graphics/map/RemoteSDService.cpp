#include "graphics/map/RemoteSDService.h"
#include "graphics/map/MapTileSettings.h"
#include "util/ILog.h"
#include <cstring>
#include <new>

#define DRIVE_LETTER "S"

IRemoteFS *RemoteSDService::remoteFS = nullptr;

void RemoteSDService::setBackend(IRemoteFS *fs)
{
    remoteFS = fs;
}

IRemoteFS *RemoteSDService::backend(void)
{
    return remoteFS;
}

RemoteSDService::RemoteSDService() : ITileService(DRIVE_LETTER ":")
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

RemoteSDService::~RemoteSDService() {}

bool RemoteSDService::load(const char *name, void *img)
{
    char buf[128];
    snprintf(buf, sizeof(buf), DRIVE_LETTER ":%s", name);
    ILOG_DEBUG("RemoteSDService::load(): %s", buf);
    lv_image_set_src((lv_obj_t *)img, buf);
    // a failed set_src may keep the previous source, so verify the source
    // now matches what was requested
    const void *src = lv_image_get_src((lv_obj_t *)img);
    if (!src || lv_image_src_get_type(src) != LV_IMAGE_SRC_FILE || strcmp((const char *)src, buf) != 0) {
        ILOG_DEBUG("Failed to load tile %s from remote SD", buf);
        return false;
    }
    return true;
}

bool RemoteSDService::save(const char *name, void *img, size_t len)
{
    if (!remoteFS)
        return false;
    ILOG_DEBUG("RemoteSDService::save(%s): %d", name, len);
    const uint8_t *data = static_cast<const uint8_t *>(img);
    uint32_t offset = 0;
    while (offset < len) {
        uint32_t chunk = (len - offset > CHUNK_SIZE) ? CHUNK_SIZE : len - offset;
        if (!remoteFS->writeChunk(name, offset, data + offset, chunk, offset == 0)) {
            ILOG_ERROR("failed to write %s at offset %u", name, offset);
            return false;
        }
        offset += chunk;
    }
    return true;
}

void *RemoteSDService::fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    if (!remoteFS || mode != LV_FS_MODE_RD)
        return nullptr;

    RemoteFile *rf = new (std::nothrow) RemoteFile;
    if (!rf)
        return nullptr;
    memset(rf, 0, offsetof(RemoteFile, chunk));
    strncpy(rf->path, path, sizeof(rf->path) - 1);

    // fetch the first chunk right away: validates existence and returns the file size
    if (!remoteFS->readChunk(rf->path, 0, rf->chunk, CHUNK_SIZE, &rf->chunkLen, &rf->size)) {
        delete rf;
        return nullptr;
    }
    return static_cast<void *>(rf);
}

lv_fs_res_t RemoteSDService::fs_close(lv_fs_drv_t *drv, void *file_p)
{
    delete static_cast<RemoteFile *>(file_p);
    return LV_FS_RES_OK;
}

lv_fs_res_t RemoteSDService::fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    RemoteFile *rf = static_cast<RemoteFile *>(file_p);
    uint8_t *dst = static_cast<uint8_t *>(buf);
    *br = 0;

    while (btr > 0 && rf->pos < rf->size) {
        if (rf->pos < rf->chunkOffset || rf->pos >= rf->chunkOffset + rf->chunkLen) {
            // fetch the chunk containing pos, aligned for cache friendliness on both ends
            uint32_t off = rf->pos - (rf->pos % CHUNK_SIZE);
            uint32_t got = 0, fsize = 0;
            if (!remoteFS || !remoteFS->readChunk(rf->path, off, rf->chunk, CHUNK_SIZE, &got, &fsize) || got == 0)
                return LV_FS_RES_UNKNOWN; // transport or backend error mid-file
            rf->chunkOffset = off;
            rf->chunkLen = got;
        }
        uint32_t avail = rf->chunkOffset + rf->chunkLen - rf->pos;
        uint32_t n = (btr < avail) ? btr : avail;
        memcpy(dst, rf->chunk + (rf->pos - rf->chunkOffset), n);
        dst += n;
        rf->pos += n;
        *br += n;
        btr -= n;
    }
    // reading zero bytes at end of file is a normal short read
    return LV_FS_RES_OK;
}

lv_fs_res_t RemoteSDService::fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    *bw = 0;
    return LV_FS_RES_NOT_IMP;
}

lv_fs_res_t RemoteSDService::fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    RemoteFile *rf = static_cast<RemoteFile *>(file_p);
    switch (whence) {
    case LV_FS_SEEK_SET:
        rf->pos = pos;
        break;
    case LV_FS_SEEK_CUR:
        rf->pos += pos;
        break;
    case LV_FS_SEEK_END:
        rf->pos = (pos <= rf->size) ? rf->size - pos : 0;
        break;
    default:
        return LV_FS_RES_INV_PARAM;
    }
    return LV_FS_RES_OK;
}

lv_fs_res_t RemoteSDService::fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    *pos_p = static_cast<RemoteFile *>(file_p)->pos;
    return LV_FS_RES_OK;
}
