#pragma once

#include <string_view>
#include <vector>

#include <bul/math/vector.h>
#include <bul/math/matrix.h>

namespace gltf
{
enum PrimitiveMode : uint32_t
{
    POINTS = 0,
    LINES = 1,
    LINE_LOOP = 2,
    LINE_STRIP = 3,
    TRIANGLES = 4,
    TRIANGLE_STRIP = 5,
    TRIANGLE_FAN = 6
};

struct Vertex
{
    bul::vec4f position{0, 0, 0, 0};
    bul::vec4f normal{0, 0, 0, 0};
    bul::vec2f uv_0{0, 0};
    bul::vec2f uv_1{0, 0};
};

struct Image
{
    std::string uri;
};

struct Texture
{
    uint32_t source_image;
};

struct Material
{
    bul::vec4f base_color_factor{1, 1, 1, 1};
    uint32_t base_color_tex;
};

struct Primitive
{
    uint32_t vertex_start;
    uint32_t index_start;
    uint32_t index_count;
    uint32_t material;
    PrimitiveMode mode = TRIANGLES;
};

struct Mesh
{
    std::vector<Primitive> primitives;
};

struct Node
{
    std::vector<uint32_t> children;
    uint32_t mesh = -1;
    bul::mat4f transform = bul::mat4f::identity();
};

struct Model
{
    std::vector<Image> images;
    std::vector<Texture> textures;
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    std::vector<Node> nodes;
    std::vector<uint32_t> scene_nodes;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

Model load(std::string_view gltf_path);
} // namespace gltf
