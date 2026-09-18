#include "graphics/map/TileService.h"

void TileService::setService(ITileService *s)
{
    delete service;
    service = s;
}

void TileService::setBackupService(ITileService *s)
{
    delete backup;
    backup = s;
}

TileService::~TileService()
{
    delete service;
    delete backup;
}