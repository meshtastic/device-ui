#include "graphics/map/TileProvider.h"
#include "graphics/map/MapTileSettings.h"
#include "util/ILog.h"
#include <algorithm>

std::vector<std::tuple<std::string, std::string>> TileProvider::urlTemplates = {
    {"URL: Google Maps", "https://mt0.google.com/vt?lyrs=m&x={x}&s=&y={y}&z={z}"}};

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
    std::string provider, url;
    std::tie(provider, url) = urlTemplates[MapTileSettings::getTileProvider()];
    size_t pos;
    while ((pos = url.find("{z}")) != std::string::npos)
        url.replace(pos, 3, std::to_string(z));
    while ((pos = url.find("{x}")) != std::string::npos)
        url.replace(pos, 3, std::to_string(x));
    while ((pos = url.find("{y}")) != std::string::npos)
        url.replace(pos, 3, std::to_string(y));
    return url;
}

const std::vector<std::string> TileProvider::templates(void)
{
    std::vector<std::string> templates;
    for (auto &it : urlTemplates) {
        std::string provider, url;
        std::tie(provider, url) = it;
        templates.push_back(url);
    }
    return templates;
}

const std::string TileProvider::providers()
{
    std::string providers;
    for (auto &it : urlTemplates) {
        std::string provider, url;
        std::tie(provider, url) = it;
        if (!providers.empty())
            providers += "\n";
        providers += provider;
    }
    return providers;
}

int TileProvider::addTemplate(const std::string &name, const std::string &tmpl)
{
    auto it = std::find(urlTemplates.begin(), urlTemplates.end(), std::tuple<std::string, std::string>({name, tmpl}));
    if (it == urlTemplates.end()) {
        urlTemplates.push_back({name, tmpl});
        return urlTemplates.size() - 1;
    } else {
        return it - urlTemplates.begin();
    }
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
