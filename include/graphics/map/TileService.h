#pragma once

#include <stdint.h>

/**
 * Abstract TileService interface; load tile from any source
 */
class ITileService
{
  public:
    // lvgl lv_fs_drv callbacks
    virtual bool load(const char *name, void *img) = 0;
    virtual ~ITileService() {}

  protected:
    ITileService(const char *id) : idLetter(id){};

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
        return service ? service->load(name, img) : backup ? backup->load(name, img) : false;
    }

    virtual ~TileService();

  protected:
    ITileService *service = nullptr;
    ITileService *backup = nullptr;
};
