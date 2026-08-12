#pragma once

#include "lvgl.h"
#include <functional>
#include <stddef.h>
#include <stdint.h>

/**
 * Abstract TileService interface; load tile from any source
 */
class ITileService
{
  public:
    // synchronous load: fetches tile and calls lv_image_set_src on img (UI task)
    virtual bool load(const char *name, void *img) = 0;
    virtual bool save(const char *name, void *img, size_t len) { return false; }

    // true when this service performs blocking I/O (HTTP/CURL) and must run off the UI task
    virtual bool isAsync() const { return false; }

    // true when this service accepts loadAsync() calls and delivers via tick()
    virtual bool hasAsync() const { return false; }

    // background-safe: fetch + decode only; returns heap-allocated lv_image_dsc_t or nullptr
    // caller owns the result; no LVGL object calls
    virtual lv_image_dsc_t *loadRaw(const char *name) { return nullptr; }

    // callback type used by tick() to deliver completed async results on the UI task
    using AsyncResultConsumer = std::function<void(uint32_t hash, uint32_t generation, lv_image_dsc_t *img_dsc)>;

    // UI task: enqueue a tile for background loading (no-op on sync services)
    virtual void loadAsync(uint32_t /*hash*/, uint32_t /*generation*/, const char * /*filename*/) {}

    // UI task: drain completed async results via consumer (no-op on sync services)
    virtual void tick(AsyncResultConsumer /*consumer*/) {}

    // UI task: discard all in-flight async work (call on full redraw)
    virtual void resetAsync() {}

    virtual ~ITileService() {}

  protected:
    ITileService(const char *id) : idLetter(id) {}

    const char *idLetter; // LVGL letter for file system drives
};

/**
 * Envelope class to allow runtime configuration of TileService variants
 * Note: This class will delete unused TileService objects.
 */
class TileService : public ITileService
{
  public:
    TileService(ITileService *s) : ITileService(""), service(s) {}
    virtual void setService(ITileService *s);
    virtual void setBackupService(ITileService *s);

    bool load(const char *name, void *img) override
    {
        if (service) {
            if (!service->load(name, img))
                return backup ? backup->load(name, img) : false;
            else
                return true;
        }
        return false;
    }

    // true if any wrapped service is async
    bool isAsync() const override;
    bool hasAsync() const override;

    // only tries non-async (fast) services; safe to call from the UI task
    bool loadSyncOnly(const char *name, void *img);

    // tries async services only; intended for the background worker
    lv_image_dsc_t *loadRaw(const char *name) override;

    void loadAsync(uint32_t hash, uint32_t generation, const char *filename) override;
    void tick(AsyncResultConsumer consumer) override;
    void resetAsync() override;

    virtual ~TileService();

  protected:
    ITileService *service = nullptr;
    ITileService *backup = nullptr;
};
