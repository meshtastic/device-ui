#pragma once

#include "graphics/map/MapFileSystem.h"
#include "graphics/map/MapTileSettings.h"
#include "graphics/map/TileService.h"
#include "graphics/map/pmtiles.hpp"
#include "lvgl.h"

#if defined(ARCH_PORTDUINO) || defined(HAS_SD_MMC) || defined(HAS_SDCARD) || defined(SENSECAP_INDICATOR)

/**
 * Serves raster tiles out of a single .pmtiles archive on the SD card.
 * Tiles the archive does not contain are handed to the fallback service
 * (plain z/x/y files on SD), which is also where fetched tiles are saved.
 */
class PMTileService : public ITileService
{
  public:
    // takes ownership of fallback and archiveFS
    explicit PMTileService(ITileService *fallback = nullptr, IMapFileSystem *archiveFS = nullptr);

    virtual ~PMTileService();

    bool load(const char *name, void *img) override;
    bool save(const char *name, void *img, size_t len) override;

  protected:
    // the archive tracks the selected style; reopens when it changes
    bool openArchive(void);
    void closeArchive(void);
    bool loadFromArchive(uint32_t z, uint32_t x, uint32_t y, void *img);
    static bool parseZXY(const char *name, uint32_t &z, uint32_t &x, uint32_t &y);

  private:
    static constexpr uint8_t MAX_DIR_DEPTH = 4;

    ITileService *fallback;
    IMapFileSystem *archiveFS;
    char openedStyle[MapTileSettings::TILE_STYLE_SIZE];
    bool archiveValid;

    pmtiles::headerv3 pmHeader;

    // directory cache, one entry per traversal depth
    uint64_t cachedDirOffset[MAX_DIR_DEPTH];
    pmtiles::directory cachedDirEntries[MAX_DIR_DEPTH];
};

#endif
