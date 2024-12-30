#pragma once

#include <stdint.h>

#define VOX_INDEX(MODEL, X, Y, Z) ((MODEL).x * (MODEL).y * (Z) + (MODEL).x * (Y) + (X))

struct vox_rgba
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

enum vox_mat_type : int32_t
{
    vox_diffuse,
    vox_metal,
    vox_emissive,
    vox_glass,
};

struct vox_matl
{
    vox_mat_type type;
    float rough;
    float metal;
    float specular;
    float emit;
    float flux;
    float ior;
    float trans;
};

struct vox_model
{
    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint8_t* voxels;
};

struct vox
{
    union
    {
        vox_model model;
        vox_model* models;
    };
    uint32_t n_models;
    vox_rgba palette[256];
    vox_matl materials[256];
};

bool vox_load(const char* path, vox* vox);
void vox_unload(vox* vox);

/* class Model
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
}; */
