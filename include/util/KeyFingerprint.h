#pragma once

#include <string>

/**
 * Abbreviate a long key for display, e.g. "AbCdEf...UvWxYz".
 * Keys that would not get shorter are returned unchanged.
 */
inline std::string keyFingerprint(const std::string &key, size_t edge = 6)
{
    if (key.size() <= 2 * edge + 3)
        return key;
    return key.substr(0, edge) + "..." + key.substr(key.size() - edge);
}
