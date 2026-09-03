/*
 * Copyright 2021 Protomaps LLC
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its 
 *    contributors may be used to endorse or promote products derived from 
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * *****************************************************************************
 * Origin: https://github.com/protomaps/PMTiles/blob/main/cpp/pmtiles.hpp
 * 
 * Adaptations for embedded use (author: mverch67):
 *  - reader path only (no directory/header serialization, no std::stringstream)
 *  - exceptions replaced by ILOG_ERROR + empty/zero return values
 *  - directory entries are allocated through lv_malloc so they land in PSRAM
 */

#ifndef PMTILES_HPP
#define PMTILES_HPP

#include "lvgl.h"
#include "util/ILog.h"
#include <cstdint>
#include <cstring>
#include <limits>
#include <new>
#include <vector>

namespace pmtiles
{

const uint8_t TILETYPE_UNKNOWN = 0x0;
const uint8_t TILETYPE_MVT = 0x1;
const uint8_t TILETYPE_PNG = 0x2;
const uint8_t TILETYPE_JPEG = 0x3;
const uint8_t TILETYPE_WEBP = 0x4;
const uint8_t TILETYPE_AVIF = 0x5;

const uint8_t COMPRESSION_UNKNOWN = 0x0;
const uint8_t COMPRESSION_NONE = 0x1;
const uint8_t COMPRESSION_GZIP = 0x2;
const uint8_t COMPRESSION_BROTLI = 0x3;
const uint8_t COMPRESSION_ZSTD = 0x4;

// on-disk size of the v3 header; sizeof(headerv3) differs due to padding
const size_t HEADER_BYTES = 127;

/**
 * Allocator routing container storage to lv_malloc (PSRAM on ESP32-S3);
 * a root directory of a large archive does not fit the internal heap.
 */
template <class T> struct PsramAllocator {
    using value_type = T;

    PsramAllocator() = default;
    template <class U> constexpr PsramAllocator(const PsramAllocator<U> &) noexcept {}

    T *allocate(size_t n)
    {
        if (n > std::numeric_limits<size_t>::max() / sizeof(T))
            throw std::bad_alloc();
        void *m = lv_malloc(n * sizeof(T));
        if (!m) {
            throw std::bad_alloc();
        }
        return static_cast<T *>(m);
    }

    void deallocate(T *p, size_t) noexcept { lv_free(p); }

    template <class U> bool operator==(const PsramAllocator<U> &) const noexcept { return true; }
    template <class U> bool operator!=(const PsramAllocator<U> &) const noexcept { return false; }
};

struct headerv3 {
    uint64_t root_dir_offset;
    uint64_t root_dir_bytes;
    uint64_t json_metadata_offset;
    uint64_t json_metadata_bytes;
    uint64_t leaf_dirs_offset;
    uint64_t leaf_dirs_bytes;
    uint64_t tile_data_offset;
    uint64_t tile_data_bytes;
    uint64_t addressed_tiles_count;
    uint64_t tile_entries_count;
    uint64_t tile_contents_count;
    bool clustered;
    uint8_t internal_compression;
    uint8_t tile_compression;
    uint8_t tile_type;
    uint8_t min_zoom;
    uint8_t max_zoom;
    int32_t min_lon_e7;
    int32_t min_lat_e7;
    int32_t max_lon_e7;
    int32_t max_lat_e7;
    uint8_t center_zoom;
    int32_t center_lon_e7;
    int32_t center_lat_e7;
};

struct entryv3 {
    uint64_t tile_id;
    uint64_t offset;
    uint32_t length;
    uint32_t run_length;
};

using directory = std::vector<entryv3, PsramAllocator<entryv3>>;

// WARNING: this is limited to little-endian
template <class T> inline void copy_from_lsb(T *ptr, const uint8_t *data, size_t offset)
{
    memcpy(ptr, data + offset, sizeof(T));
}

inline bool deserialize_header(const uint8_t *data, size_t len, headerv3 &h)
{
    if (len < HEADER_BYTES || memcmp(data, "PMTiles", 7) != 0) {
        ILOG_ERROR("Invalid .pmtiles file");
        return false;
    }
    if (data[7] != 0x3) {
        ILOG_ERROR("Unsupported PMTiles version: %d", (uint16_t)data[7]);
        return false;
    }
    copy_from_lsb(&h.root_dir_offset, data, 8);
    copy_from_lsb(&h.root_dir_bytes, data, 16);
    copy_from_lsb(&h.json_metadata_offset, data, 24);
    copy_from_lsb(&h.json_metadata_bytes, data, 32);
    copy_from_lsb(&h.leaf_dirs_offset, data, 40);
    copy_from_lsb(&h.leaf_dirs_bytes, data, 48);
    copy_from_lsb(&h.tile_data_offset, data, 56);
    copy_from_lsb(&h.tile_data_bytes, data, 64);
    copy_from_lsb(&h.addressed_tiles_count, data, 72);
    copy_from_lsb(&h.tile_entries_count, data, 80);
    copy_from_lsb(&h.tile_contents_count, data, 88);
    h.clustered = (data[96] == 0x1);
    h.internal_compression = data[97];
    h.tile_compression = data[98];
    h.tile_type = data[99];
    h.min_zoom = data[100];
    h.max_zoom = data[101];
    copy_from_lsb(&h.min_lon_e7, data, 102);
    copy_from_lsb(&h.min_lat_e7, data, 106);
    copy_from_lsb(&h.max_lon_e7, data, 110);
    copy_from_lsb(&h.max_lat_e7, data, 114);
    h.center_zoom = data[118];
    copy_from_lsb(&h.center_lon_e7, data, 119);
    copy_from_lsb(&h.center_lat_e7, data, 123);
    return true;
}

namespace
{
constexpr const int8_t max_varint_length = sizeof(uint64_t) * 8 / 7 + 1;

// from https://github.com/mapbox/protozero/blob/master/include/protozero/varint.hpp
inline uint64_t decode_varint_impl(const char **data, const char *end)
{
    const auto *begin = reinterpret_cast<const int8_t *>(*data);
    const auto *iend = reinterpret_cast<const int8_t *>(end);
    const int8_t *p = begin;
    uint64_t val = 0;

    if (iend - begin >= max_varint_length) { // fast path
        do {
            int64_t b = *p++;
            val = ((uint64_t(b) & 0x7fU));
            if (b >= 0) {
                break;
            }
            b = *p++;
            val |= ((uint64_t(b) & 0x7fU) << 7U);
            if (b >= 0) {
                break;
            }
            b = *p++;
            val |= ((uint64_t(b) & 0x7fU) << 14U);
            if (b >= 0) {
                break;
            }
            b = *p++;
            val |= ((uint64_t(b) & 0x7fU) << 21U);
            if (b >= 0) {
                break;
            }
            b = *p++;
            val |= ((uint64_t(b) & 0x7fU) << 28U);
            if (b >= 0) {
                break;
            }
            b = *p++;
            val |= ((uint64_t(b) & 0x7fU) << 35U);
            if (b >= 0) {
                break;
            }
            b = *p++;
            val |= ((uint64_t(b) & 0x7fU) << 42U);
            if (b >= 0) {
                break;
            }
            b = *p++;
            val |= ((uint64_t(b) & 0x7fU) << 49U);
            if (b >= 0) {
                break;
            }
            b = *p++;
            val |= ((uint64_t(b) & 0x7fU) << 56U);
            if (b >= 0) {
                break;
            }
            b = *p++;
            val |= ((uint64_t(b) & 0x01U) << 63U);
            if (b >= 0) {
                break;
            }
            ILOG_ERROR("Varint too long");
            return 0;
        } while (false);
    } else {
        unsigned int shift = 0;
        while (p != iend && *p < 0) {
            val |= (uint64_t(*p++) & 0x7fU) << shift;
            shift += 7;
        }
        if (p == iend) {
            ILOG_ERROR("end of buffer");
            return 0;
        }
        val |= uint64_t(*p++) << shift;
    }

    *data = reinterpret_cast<const char *>(p);
    return val;
}

inline uint64_t decode_varint(const char **data, const char *end)
{
    // If this is a one-byte varint, decode it here.
    if (end != *data && ((static_cast<uint64_t>(**data) & 0x80U) == 0)) {
        const auto val = static_cast<uint64_t>(**data);
        ++(*data);
        return val;
    }
    // If this varint is more than one byte, defer to complete implementation.
    return decode_varint_impl(data, end);
}

inline void rotate(int64_t n, uint32_t &x, uint32_t &y, uint32_t rx, uint32_t ry)
{
    if (ry == 0) {
        if (rx != 0) {
            x = n - 1 - x;
            y = n - 1 - y;
        }
        uint32_t t = x;
        x = y;
        y = t;
    }
}
} // end anonymous namespace

// use a 0 length entry as a null value.
inline entryv3 find_tile(const directory &entries, uint64_t tile_id)
{
    int m = 0;
    int n = static_cast<int>(entries.size()) - 1;
    while (m <= n) {
        int k = (n + m) >> 1;
        if (tile_id > entries[k].tile_id) {
            m = k + 1;
        } else if (tile_id < entries[k].tile_id) {
            n = k - 1;
        } else {
            return entries[k];
        }
    }

    if (n >= 0) {
        if (entries[n].run_length == 0) {
            return entries[n];
        }
        if (tile_id - entries[n].tile_id < entries[n].run_length) {
            return entries[n];
        }
    }

    return entryv3{0, 0, 0, 0};
}

inline uint64_t zxy_to_tileid(uint8_t z, uint32_t x, uint32_t y)
{
    if (z > 31) {
        ILOG_ERROR("tile zoom exceeds 64-bit limit");
        return 0;
    }
    if (x > (1U << z) - 1U || y > (1U << z) - 1U) {
        ILOG_ERROR("tile x/y outside zoom level bounds");
        return 0;
    }
    uint64_t acc = ((1LL << (z * 2U)) - 1) / 3;
    if (z == 0) {
        return acc;
    }
    uint32_t tx = x, ty = y;
    int a = z - 1;
    for (uint32_t s = 1LL << a; s > 0; s >>= 1) {
        uint32_t rx = s & tx;
        uint32_t ry = s & ty;
        rotate(s, tx, ty, rx, ry);
        acc += ((3LL * rx) ^ ry) << a;
        a--;
    }
    return acc;
}

// takes an uncompressed byte buffer
inline directory deserialize_directory(const char *decompressed, size_t decompressedSize)
{
    const char *t = decompressed;
    const char *end = t + decompressedSize;

    const uint64_t num_entries_64bit = decode_varint(&t, end);
    // Sanity check to avoid excessive memory allocation attempt:
    // each directory entry takes at least 4 bytes
    if (num_entries_64bit / 4U > decompressedSize) {
        ILOG_ERROR("malformed directory 1");
        return {};
    }
    if (num_entries_64bit > std::numeric_limits<size_t>::max()) {
        ILOG_ERROR("malformed directory 2");
        return {};
    }
    const size_t num_entries = static_cast<size_t>(num_entries_64bit);

    directory result;
    try {
        result.resize(num_entries);
    } catch (const std::bad_alloc &) {
        ILOG_ERROR("failed to allocate %u directory entries", (unsigned int)num_entries);
        return {};
    }

    uint64_t last_id = 0;
    for (size_t i = 0; i < num_entries; i++) {
        const uint64_t val = decode_varint(&t, end);
        if (val > std::numeric_limits<uint64_t>::max() - last_id) {
            ILOG_ERROR("malformed directory 3");
            return {};
        }
        const uint64_t tile_id = last_id + val;
        result[i].tile_id = tile_id;
        last_id = tile_id;
    }

    for (size_t i = 0; i < num_entries; i++) {
        const uint64_t val = decode_varint(&t, end);
        if (val > std::numeric_limits<uint32_t>::max()) {
            ILOG_ERROR("malformed directory 4");
            return {};
        }
        result[i].run_length = static_cast<uint32_t>(val);
    }

    for (size_t i = 0; i < num_entries; i++) {
        const uint64_t val = decode_varint(&t, end);
        if (val > std::numeric_limits<uint32_t>::max()) {
            ILOG_ERROR("malformed directory 5");
            return {};
        }
        result[i].length = static_cast<uint32_t>(val);
    }

    for (size_t i = 0; i < num_entries; i++) {
        uint64_t tmp = decode_varint(&t, end);

        if (i > 0 && tmp == 0) {
            if (result[i - 1].offset > std::numeric_limits<uint64_t>::max() - result[i - 1].length) {
                ILOG_ERROR("malformed directory 6");
                return {};
            }
            result[i].offset = result[i - 1].offset + result[i - 1].length;
        } else {
            result[i].offset = tmp - 1;
        }
    }

    // assert the directory has been fully consumed
    if (t != end) {
        ILOG_ERROR("malformed directory 7");
        return {};
    }

    return result;
}

} // namespace pmtiles

#endif
