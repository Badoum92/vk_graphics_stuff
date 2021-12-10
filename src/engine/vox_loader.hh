#pragma once

#include <filesystem>
#include <vector>
#include <array>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc99-extensions"

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

enum MatType : int
{
    DIFFUSE = 0,
    METAL = 1,
    GLASS = 2,
    EMISSIVE = 3
};

enum MatPropBit : int
{
    PLASTIC = 1 << 0,
    ROUGHNESS = 1 << 1,
    SPECULAR = 1 << 2,
    IOR = 1 << 3,
    ATTENUATION = 1 << 4,
    POWER = 1 << 5,
    GLOW = 1 << 6,
    IS_TOTAL_POWER = 1 << 7,
};

struct MATT
{
    int id;
    MatType type;
    float weight;
    MatPropBit prop_bits;
    float prop_value[];
};

#pragma clang diagnostic pop

struct Chunk
{
    SIZE* size = nullptr;
    uint32_t n_voxels = 0;
    XYZI* xyzi = nullptr;
    MATT* material = nullptr;
};

class Model
{
public:
    Model(const std::filesystem::path& path);
    Model() = default;

    void load(const std::filesystem::path& path);

    std::vector<Chunk> chunks;
    std::array<RGBA, 256> palette;

private:
    std::vector<uint8_t> bytes_;
};
} // namespace Vox
