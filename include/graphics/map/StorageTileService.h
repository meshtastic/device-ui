#pragma once

#include "graphics/map/TileService.h"

class IStorage;

/**
 * Loads z/x/y tiles from, and saves fetched tiles to, any storage volume.
 */
class StorageTileService : public ITileService
{
  public:
    explicit StorageTileService(IStorage &storage);

    bool load(const char *name, void *img) override;
    bool save(const char *name, void *img, size_t len) override;

  private:
    IStorage &storage;
};
