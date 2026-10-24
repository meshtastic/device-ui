#pragma once

#include "storage/IStorage.h"
#include <set>
#include <string>

/**
 * Map tile layout on a storage volume: <folder>/<style>/{z/x/y.png, <style>.pmtiles, .url}
 */
namespace MapStorage
{
// subdirectories of folder, or {"/map"} when only a plain /map directory exists
std::set<std::string> loadMapStyles(IStorage &storage, const char *folder);
bool hasMapArchive(IStorage &storage, const char *folder, const char *style);
std::string getUrlProvider(IStorage &storage, const char *folder, const char *style);
bool setUrlProvider(IStorage &storage, const char *folder, const char *style, const char *urlTemplate);
} // namespace MapStorage
