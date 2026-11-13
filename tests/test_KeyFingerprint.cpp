#include "util/KeyFingerprint.h"
#include <doctest/doctest.h>

TEST_CASE("keyFingerprint abbreviates a base64 public key")
{
    const std::string key = "WQZ3iQVjFEwDMX5a0w5ljtoUBpMf1FC/F5mIS3c4JBM=";
    CHECK(key.size() == 44);
    CHECK(keyFingerprint(key) == "WQZ3iQ...c4JBM=");
    CHECK(keyFingerprint(key, 4) == "WQZ3...JBM=");
}

TEST_CASE("keyFingerprint leaves short keys untouched")
{
    CHECK(keyFingerprint("") == "");
    CHECK(keyFingerprint("short") == "short");
    CHECK(keyFingerprint("exactly15chars!") == "exactly15chars!");
    CHECK(keyFingerprint("sixteen chars!!!") == "sixtee...ars!!!");
}
