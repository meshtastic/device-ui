#include "graphics/map/TileProvider.h"
#include "graphics/map/MapTileSettings.h"
#include "util/ILog.h"
#include <algorithm>

std::vector<std::string> TileProvider::urlTemplates = {
    "https://mt0.google.com/vt?lyrs=m&x={x}&s=&y={y}&z={z}",
    "http://services.arcgisonline.com/arcgis/rest/services/World_Topo_Map/MapServer/tile/{z}/{y}/{x}"};

std::string TileProvider::url(const char *filename)
{
    // try to match /z/x/y.png at the end of file path
    int x, y, z;
    int matched = sscanf(filename, "%*[^0-9]%d/%d/%d.png", &z, &x, &y);
    if (matched != 3) {
        ILOG_ERROR("failed to extract z/x/y from %s", filename);
        x = y = z = 0;
    }
    return url(z, x, y);
}

std::string TileProvider::url(int z, int x, int y)
{
    std::string tmpl = urlTemplates[MapTileSettings::getTileProvider()];
    size_t pos;
    while ((pos = tmpl.find("{z}")) != std::string::npos)
        tmpl.replace(pos, 3, std::to_string(z));
    while ((pos = tmpl.find("{x}")) != std::string::npos)
        tmpl.replace(pos, 3, std::to_string(x));
    while ((pos = tmpl.find("{y}")) != std::string::npos)
        tmpl.replace(pos, 3, std::to_string(y));
    return tmpl;
}

const std::vector<std::string> &TileProvider::templates()
{
    return urlTemplates;
}

void TileProvider::addTemplate(const std::string &tmpl)
{
    urlTemplates.push_back(tmpl);
}

void TileProvider::selectTemplate(int idx)
{
    if (idx >= 0 && idx < (int)urlTemplates.size()) {
        MapTileSettings::setTileProvider(idx);
    }
}

int TileProvider::selectedTemplate(void)
{
    return MapTileSettings::getTileProvider();
}
