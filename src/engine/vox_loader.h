#pragma once

#include <vector>
#include <array>
#include <string_view>

namespace Vox
{
struct Header
{
    char magic[4];
    int32_t version;
};

struct ChunkId
{
    char id[4];
    int32_t n_bytes;
    int32_t n_children_bytes;
};

struct SIZE
{
    int32_t x = 0;
    int32_t z = 0;
    int32_t y = 0;
};

struct XYZI
{
    uint8_t x;
    uint8_t z;
    uint8_t y; // in vox models the "gravity" axis is z so we need to swap z and y
    uint8_t color_index;
};

struct RGBA
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;
};

enum MatType : int32_t
{
    DIFFUSE = 0,
    METAL = 1,
    GLASS = 2,
    EMISSIVE = 3
};

struct MATL
{
    MatType type = DIFFUSE;
    float emit = 0;
    float flux = 0;
    float metal = 0;
    float rough = 0;
    float ior = 0;
    float trans = 0;
};

struct Chunk
{
    SIZE* size = nullptr;
    uint32_t n_voxels = 0;
    XYZI* xyzi = nullptr;
};

class Model
{
public:
    Model(const std::string_view path);
    Model() = default;

    void load(const std::string_view path);

    std::vector<Chunk> chunks;
    std::array<RGBA, 256> palette;
    std::array<MATL, 256> materials;

private:
    std::vector<uint8_t> bytes_;

    void parse_pack(const ChunkId* chunk);
    void parse_size(const ChunkId* chunk);
    void parse_xyzi(const ChunkId* chunk);
    void parse_ntrn(const ChunkId* chunk);
    void parse_ngrp(const ChunkId* chunk);
    void parse_nshp(const ChunkId* chunk);
    void parse_layr(const ChunkId* chunk);
    void parse_rgba(const ChunkId* chunk);
    void parse_matl(const ChunkId* chunk);
    void parse_robj(const ChunkId* chunk);
    void parse_rcam(const ChunkId* chunk);
    void parse_note(const ChunkId* chunk);
};
} // namespace Vox
