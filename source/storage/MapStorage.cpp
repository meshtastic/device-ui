#include "storage/MapStorage.h"
#include "graphics/map/MapTileSettings.h"
#include "util/ILog.h"
#include <cstring>

namespace
{
std::string trim(const std::string &s)
{
    const char *ws = " \t\r\n";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos)
        return {};
    return s.substr(start, s.find_last_not_of(ws) - start + 1);
}

std::string urlFile(const char *folder, const char *style)
{
    return std::string(folder) + "/" + style + "/.url";
}
} // namespace

namespace MapStorage
{

std::set<std::string> loadMapStyles(IStorage &storage, const char *folder)
{
    std::set<std::string> styles;
    if (folder) {
        storage.list(folder, [&styles](const char *name, bool isDir) {
            if (!isDir || name[0] == '.')
                return;
            if (strlen(name) < MapTileSettings::TILE_STYLE_SIZE - 1) {
                ILOG_DEBUG("SD: found map style: %s", name);
                styles.insert(name);
            } else {
                ILOG_WARN("ignored: %s (name too long)", name);
            }
        });
    }
    if (styles.empty()) {
        if (storage.exists("/map")) {
            ILOG_DEBUG("SD: found /map dir");
            styles.insert("/map");
        } else {
            ILOG_INFO("SD: no maps found");
        }
    }
    return styles;
}

bool hasMapArchive(IStorage &storage, const char *folder, const char *style)
{
    char dir[MapTileSettings::TILE_STYLE_SIZE];
    MapTileSettings::styleToDir(style, dir, sizeof(dir));
    if (!folder || !dir[0])
        return false;
    std::string path = std::string(folder) + "/" + dir + "/" + dir + MapTileSettings::PMTILES_EXTENSION;
    return storage.exists(path.c_str());
}

std::string getUrlProvider(IStorage &storage, const char *folder, const char *style)
{
    if (!folder || !style)
        return {};
    auto file = storage.open(urlFile(folder, style).c_str(), OpenMode::Read);
    if (!file)
        return {};
    char line[1024];
    if (!file->readLine(line, sizeof(line)))
        return {};
    return trim(line);
}

bool setUrlProvider(IStorage &storage, const char *folder, const char *style, const char *urlTemplate)
{
    if (!folder || !style || !urlTemplate)
        return false;
    std::string dir = std::string(folder) + "/" + style;
    if (!storage.mkdir(dir.c_str()))
        return false;
    auto file = storage.open(urlFile(folder, style).c_str(), OpenMode::Write);
    if (!file)
        return false;
    std::string content = trim(urlTemplate) + "\n";
    return file->print(content.c_str());
}

} // namespace MapStorage
