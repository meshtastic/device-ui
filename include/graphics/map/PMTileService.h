#pragma once

#include "graphics/map/TileService.h"
#include "graphics/map/pmtiles.hpp"
#include "graphics/common/SdCard.h"
#include "lvgl.h"
#include <vector>
#include <string>

#if defined(ARCH_PORTDUINO)
#include "PortduinoFS.h"
extern fs::FS &SDFs;
typedef fs::File FsFile;
#endif

class PMTileService : public ITileService
{
  public:
    PMTileService();

    virtual ~PMTileService();

    bool load(uint32_t x, uint32_t y, uint32_t z, void *img) override;

  protected:
    std::string decompress(const std::string &data, uint8_t compression);
    bool decodeImg(const void *data, size_t size, lv_img_dsc_t* &img);
    void convertRGB888ToRGB565(uint8_t *src, uint16_t *dst, int width, int height);

  private:
    // pmtiles directory cache
    uint64_t cachedDirOffset[3] = { 0, 0, 0 };
    std::vector<pmtiles::entryv3> cachedDirEntries[3];
    // pmtiles file and header
    FsFile pmTiles;
    pmtiles::headerv3 pmHeader;
};
