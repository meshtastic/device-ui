#pragma once

#include "graphics/map/MapTileSettings.h"
#include "graphics/map/TileService.h"
#include "graphics/map/pmtiles.hpp"
#include "lvgl.h"
#include "storage/IStorage.h"

/**
 * Serves raster tiles out of a single .pmtiles archive on the storage.
 * Tiles the archive does not contain are handed to the fallback service
 * (plain z/x/y files), which is also where fetched tiles are saved.
 */
class PMTileService : public ITileService
{
  public:
    // takes ownership of fallback
    explicit PMTileService(IStorage &storage, ITileService *fallback = nullptr);

    virtual ~PMTileService();

    bool load(const char *name, void *img) override;
    bool save(const char *name, void *img, size_t len) override;

  protected:
    // the archive tracks the selected style and the storage mount; reopens when either changes
    bool openArchive(void);
    void closeArchive(void);
    bool loadFromArchive(uint32_t z, uint32_t x, uint32_t y, void *img);
    static bool parseZXY(const char *name, uint32_t &z, uint32_t &x, uint32_t &y);

  private:
    static constexpr uint8_t MAX_DIR_DEPTH = 4;

    IStorage &storage;
    ITileService *fallback;
    std::unique_ptr<IFile> archive;
    char openedStyle[MapTileSettings::TILE_STYLE_SIZE];
    uint32_t openedGeneration;
    bool archiveValid;

    pmtiles::headerv3 pmHeader;

    // directory cache, one entry per traversal depth
    uint64_t cachedDirOffset[MAX_DIR_DEPTH];
    pmtiles::directory cachedDirEntries[MAX_DIR_DEPTH];
};
