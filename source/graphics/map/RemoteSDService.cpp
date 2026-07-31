#include "graphics/map/RemoteSDService.h"
#include "graphics/map/MapTileSettings.h"
#include "util/ILog.h"
#include "util/PNGDecoder.h"
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
    // No LVGL-level cache: it would service small header reads by pulling
    // cache_size bytes over the UART link, roughly doubling the transfer
    // per tile. fs_read keeps its own chunk-sized read-ahead instead.
    drv.cache_size = 0;
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
    uint32_t start = lv_tick_get();
    if (!remoteFS)
        return false;

    // Fetch offset-0 chunk to learn the file size; also warms the chunk cache
    // so the first fs_read after a load hit skips the link round-trip.
    uint32_t fileSize = 0;
    if (chunkAt(name, 0, &fileSize) == 0 || fileSize == 0) {
        ILOG_DEBUG("Failed to open tile %s from remote SD", name);
        return false;
    }

    uint8_t *pngData = (uint8_t *)lv_malloc(fileSize);
    if (!pngData) {
        ILOG_ERROR("lv_malloc failed for %s (%u bytes)", name, (unsigned int)fileSize);
        return false;
    }

    // Copy the chunk we already have in the cache
    uint32_t copied = (cachedLen < fileSize) ? cachedLen : fileSize;
    memcpy(pngData, cachedChunk, copied);

    // Read remaining chunks directly into the flat buffer
    uint32_t offset = copied;
    while (offset < fileSize) {
        uint32_t n = 0, sz = 0;
        uint32_t req = (fileSize - offset < CHUNK_SIZE) ? (fileSize - offset) : CHUNK_SIZE;
        if (!remoteFS->readChunk(name, offset, pngData + offset, req, &n, &sz) || n == 0) {
            lv_free(pngData);
            ILOG_ERROR("read error for tile %s at offset %u", name, offset);
            return false;
        }
        offset += n;
    }

    lv_img_dsc_t *img_dsc = nullptr;
    bool decoded = MapTileSettings::color() ? decodeImgColor(pngData, fileSize, &img_dsc)
                                            : decodeImgGrey(pngData, fileSize, &img_dsc);
    lv_free(pngData);

    if (!decoded) {
        ILOG_ERROR("Failed to decode tile %s", name);
        return false;
    }

    lv_obj_t *img_obj = (lv_obj_t *)img;
    lv_image_set_src(img_obj, img_dsc);
    if (lv_image_get_src(img_obj) != img_dsc) {
        ILOG_ERROR("lv_image_set_src failed for tile %s", name);
        if (img_dsc->data && img_dsc->data_size > 0)
            lv_free((void *)img_dsc->data);
        lv_free(img_dsc);
        return false;
    }

    ILOG_DEBUG("Tile %s loaded in %d ms.", name, lv_tick_get() - start);
    return true;
}

bool RemoteSDService::save(const char *name, void *img, size_t len)
{
    if (!remoteFS)
        return false;
    ILOG_DEBUG("RemoteSDService::save(%s): %d", name, len);
    cachedLen = 0; // whatever we hold of this file is about to be stale
    const uint8_t *data = static_cast<const uint8_t *>(img);
    uint32_t offset = 0;
    while (offset < len) {
        uint32_t chunk = (len - offset > CHUNK_SIZE) ? CHUNK_SIZE : len - offset;
        if (!remoteFS->writeChunk(name, offset, data + offset, chunk, offset == 0)) {
            ILOG_ERROR("failed to write %s at offset %u", name, offset);
            // don't leave a truncated tile behind: it would satisfy the
            // existence check on the next load and never be re-fetched
            if (offset > 0)
                remoteFS->remove(name);
            return false;
        }
        offset += chunk;
    }
    return true;
}

// One chunk, shared by all open files. The PNG decoder opens each tile three
// times (header, size, content) and reads it start to end, so a chunk fetched
// for one of those opens is what the next one asks for: keeping it here rather
// than per file saves fetching it again over the link.
uint8_t RemoteSDService::cachedChunk[RemoteSDService::CHUNK_SIZE];
char RemoteSDService::cachedPath[256] = "";
uint32_t RemoteSDService::cachedOffset = 0;
uint32_t RemoteSDService::cachedLen = 0;
uint32_t RemoteSDService::cachedSize = 0;

// Fetches the chunk that holds `pos` unless it is the one we already have.
// Returns the number of bytes of it that follow pos, 0 on failure.
uint32_t RemoteSDService::chunkAt(const char *path, uint32_t pos, uint32_t *fileSize)
{
    uint32_t off = pos - (pos % CHUNK_SIZE);
    if (cachedLen == 0 || off != cachedOffset || strcmp(cachedPath, path) != 0) {
        uint32_t got = 0, size = 0;
        if (!remoteFS || !remoteFS->readChunk(path, off, cachedChunk, CHUNK_SIZE, &got, &size) || got == 0) {
            cachedLen = 0;
            return 0;
        }
        strncpy(cachedPath, path, sizeof(cachedPath) - 1);
        cachedPath[sizeof(cachedPath) - 1] = '\0';
        cachedOffset = off;
        cachedLen = got;
        cachedSize = size;
    }
    if (fileSize)
        *fileSize = cachedSize;
    if (pos >= cachedOffset + cachedLen)
        return 0; // the chunk does not reach pos: file shrank under us
    return cachedOffset + cachedLen - pos;
}

void *RemoteSDService::fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    if (!remoteFS || mode != LV_FS_MODE_RD)
        return nullptr;

    RemoteFile *rf = new (std::nothrow) RemoteFile;
    if (!rf)
        return nullptr;
    memset(rf, 0, sizeof(*rf));
    strncpy(rf->path, path, sizeof(rf->path) - 1);

    // The open has to fail for a file that is not there, and LVGL wants the
    // size. Both come from the chunk holding offset 0, which the decoder is
    // about to read anyway and which the next open of the same tile reuses.
    if (chunkAt(rf->path, 0, &rf->size) == 0) {
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
        // avail is 0 when the chunk cannot be fetched, or does not reach pos
        // because the file shrank since it was opened
        uint32_t avail = chunkAt(rf->path, rf->pos, &rf->size);
        if (avail == 0)
            return LV_FS_RES_UNKNOWN;
        uint32_t n = (btr < avail) ? btr : avail;
        memcpy(dst, cachedChunk + (rf->pos - cachedOffset), n);
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
        // LVGL convention (matching its stdio/FATFS drivers): the position
        // is size + pos; reads past the end return zero bytes
        rf->pos = rf->size + pos;
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
