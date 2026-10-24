#include "storage/MapStorage.h"
#include "storage/PosixStorage.h"
#include "storage/RemoteStorage.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <doctest/doctest.h>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace
{
struct TempDir {
    TempDir()
    {
        char tmpl[] = "/tmp/device-ui-storage-XXXXXX";
        path = mkdtemp(tmpl);
    }
    ~TempDir() { std::filesystem::remove_all(path); }
    std::string path;
};

// in-memory co-processor: files and directories keyed by full path
class FakeRemoteFS : public IRemoteFS
{
  public:
    std::map<std::string, std::string> files;
    uint32_t maxChunk = 4096;
    uint32_t reads = 0;

    bool readChunk(const char *path, uint32_t offset, uint8_t *buf, uint32_t len, uint32_t *bytesRead, uint32_t *fileSize) override
    {
        reads++;
        auto it = files.find(path);
        if (it == files.end())
            return false;
        const std::string &data = it->second;
        uint32_t n = offset < data.size() ? std::min<uint32_t>({len, maxChunk, (uint32_t)(data.size() - offset)}) : 0;
        memcpy(buf, data.data() + offset, n);
        *bytesRead = n;
        *fileSize = data.size();
        return true;
    }

    bool writeChunk(const char *path, uint32_t offset, const uint8_t *buf, uint32_t len, bool create) override
    {
        std::string &data = files[path];
        if (create)
            data.clear();
        if (offset != data.size())
            return false;
        data.append((const char *)buf, len);
        return true;
    }

    bool remove(const char *path) override { return files.erase(path) > 0; }

    bool listDir(const char *path, std::set<std::string> &entries) override
    {
        std::string prefix = std::string(path) + "/";
        bool found = false;
        for (auto &f : files) {
            if (f.first.compare(0, prefix.size(), prefix) != 0)
                continue;
            found = true;
            std::string rest = f.first.substr(prefix.size());
            size_t slash = rest.find('/');
            entries.insert(slash == std::string::npos ? rest : rest.substr(0, slash + 1));
        }
        return found;
    }

    bool sdInfo(RemoteSdInfo &info) override
    {
        info.present = true;
        info.cardType = 3;
        info.fatType = 3;
        info.cardSize = 1000;
        info.usedBytes = 400;
        info.freeBytes = 600;
        info.statsValid = true;
        return true;
    }
    bool sdEject(void) override { return true; }
    bool sdMount(void) override { return true; }
    bool sdFormat(void) override { return true; }
};

std::string readAll(IStorage &storage, const char *path)
{
    auto file = storage.open(path, OpenMode::Read);
    if (!file)
        return "<missing>";
    std::string data(file->size(), '\0');
    int32_t n = file->read(&data[0], data.size());
    return n == (int32_t)data.size() ? data : "<short read>";
}
} // namespace

TEST_CASE("PosixStorage")
{
    TempDir tmp;
    PosixStorage storage(tmp.path.c_str());
    CHECK(storage.mount() == IStorage::Status::Ok);
    CHECK_FALSE(storage.hasSlot());

    SUBCASE("write, read back and seek")
    {
        CHECK(storage.mkdir("/maps/osm/1/2"));
        auto file = storage.open("/maps/osm/1/2/3.png", OpenMode::Write);
        REQUIRE(file);
        CHECK(file->print("0123456789"));
        file.reset();

        CHECK(readAll(storage, "/maps/osm/1/2/3.png") == "0123456789");
        auto in = storage.open("/maps/osm/1/2/3.png", OpenMode::Read);
        char buf[3] = {};
        CHECK(in->readAt(7, buf, 3));
        CHECK(std::string(buf, 3) == "789");
        CHECK_FALSE(in->readAt(8, buf, 3));
    }

    SUBCASE("append and readLine")
    {
        storage.open("/a.txt", OpenMode::Write)->print("one\n");
        storage.open("/a.txt", OpenMode::Append)->print("two");
        auto in = storage.open("/a.txt", OpenMode::Read);
        char line[16];
        CHECK(in->readLine(line, sizeof(line)));
        CHECK(std::string(line) == "one");
        CHECK(in->readLine(line, sizeof(line)));
        CHECK(std::string(line) == "two");
        CHECK_FALSE(in->readLine(line, sizeof(line)));
    }

    SUBCASE("exists, list and remove")
    {
        CHECK(storage.mkdir("/dir/sub"));
        storage.open("/dir/file", OpenMode::Write)->print("x");
        std::map<std::string, bool> entries;
        CHECK(storage.list("/dir", [&](const char *name, bool isDir) { entries[name] = isDir; }));
        CHECK(entries.size() == 2);
        CHECK(entries["sub"]);
        CHECK_FALSE(entries["file"]);
        CHECK(storage.remove("/dir/file"));
        CHECK_FALSE(storage.exists("/dir/file"));
        CHECK_FALSE(storage.open("/dir/file", OpenMode::Read));
    }
}

TEST_CASE("MapStorage")
{
    TempDir tmp;
    PosixStorage storage(tmp.path.c_str());

    SUBCASE("styles, archives and url providers")
    {
        CHECK(storage.mkdir("/maps/osm"));
        CHECK(storage.mkdir("/maps/.hidden"));
        storage.open("/maps/readme.txt", OpenMode::Write)->print("x");
        storage.open("/maps/osm/osm.pmtiles", OpenMode::Write)->print("x");

        std::set<std::string> styles = MapStorage::loadMapStyles(storage, "/maps");
        CHECK(styles == std::set<std::string>{"osm"});
        CHECK(MapStorage::hasMapArchive(storage, "/maps", "osm"));
        CHECK_FALSE(MapStorage::hasMapArchive(storage, "/maps", "topo"));

        CHECK(MapStorage::getUrlProvider(storage, "/maps", "topo").empty());
        CHECK(MapStorage::setUrlProvider(storage, "/maps", "topo", "  https://t/{z}/{x}/{y}.png \r\n"));
        CHECK(MapStorage::getUrlProvider(storage, "/maps", "topo") == "https://t/{z}/{x}/{y}.png");
    }

    SUBCASE("plain /map directory as fallback")
    {
        CHECK(storage.mkdir("/map/1"));
        CHECK(MapStorage::loadMapStyles(storage, "/maps") == std::set<std::string>{"/map"});
    }
}

TEST_CASE("RemoteStorage")
{
    FakeRemoteFS fake;
    RemoteStorage::setBackend(&fake);
    RemoteStorage storage;
    CHECK(storage.mount() == IStorage::Status::Ok);

    SUBCASE("info maps the protobuf enums")
    {
        StorageInfo info;
        CHECK(storage.info(info));
        CHECK(info.card == StorageInfo::eSDHC);
        CHECK(info.fat == StorageInfo::eExFat);
        CHECK(info.used == 400);
    }

    SUBCASE("reads across chunk boundaries")
    {
        std::string data;
        for (int i = 0; i < 10000; i++)
            data.push_back((char)('a' + i % 26));
        fake.files["/maps/osm/osm.pmtiles"] = data;
        fake.maxChunk = 1000;

        CHECK(readAll(storage, "/maps/osm/osm.pmtiles") == data);
        auto in = storage.open("/maps/osm/osm.pmtiles", OpenMode::Read);
        char buf[50];
        CHECK(in->readAt(4990, buf, sizeof(buf)));
        CHECK(std::string(buf, sizeof(buf)) == data.substr(4990, 50));
        CHECK_FALSE(in->readAt(9990, buf, sizeof(buf)));
    }

    SUBCASE("writes are appended chunk-wise")
    {
        std::string data(9000, 'z');
        auto out = storage.open("/keys/node.yml", OpenMode::Write);
        CHECK(out->write(data.data(), data.size()) == (int32_t)data.size());
        CHECK_FALSE(out->seek(0));
        out.reset();
        CHECK(fake.files["/keys/node.yml"] == data);

        storage.open("/keys/node.yml", OpenMode::Append)->print("!");
        CHECK(fake.files["/keys/node.yml"] == data + "!");
    }

    SUBCASE("directories carry no trailing slash")
    {
        fake.files["/maps/osm/1/2/3.png"] = "x";
        fake.files["/maps/note.txt"] = "x";
        std::map<std::string, bool> entries;
        CHECK(storage.list("/maps", [&](const char *name, bool isDir) { entries[name] = isDir; }));
        CHECK(entries.size() == 2);
        CHECK(entries["osm"]);
        CHECK_FALSE(entries["note.txt"]);
        CHECK(storage.exists("/maps/osm"));
        CHECK(storage.exists("/maps/note.txt"));
        CHECK_FALSE(storage.exists("/maps/topo"));
    }

    RemoteStorage::setBackend(nullptr);
}
