#pragma once

#include "graphics/map/TileLoader.h"
#include "graphics/map/TileService.h"

/**
 * Wraps any ITileService that provides loadRaw() and routes it through a background worker.
 * The wrapped service is owned and deleted by this class.
 */
class AsyncTileService : public ITileService
{
  public:
    explicit AsyncTileService(ITileService *wrapped);
    ~AsyncTileService();

    bool load(const char *name, void *img) override { return false; }
    lv_image_dsc_t *loadRaw(const char *name) override;

    bool isAsync() const override { return true; }
    bool hasAsync() const override { return true; }

    void loadAsync(uint32_t hash, uint32_t generation, const char *filename) override;
    void tick(AsyncResultConsumer consumer) override;
    void resetAsync() override;

  private:
    ITileService *wrapped_;
    AsyncTileLoader loader_;
};
