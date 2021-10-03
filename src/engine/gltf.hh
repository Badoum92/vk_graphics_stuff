#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <cassert>
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include "handle.hh"

namespace vk
{
struct Device;
struct Buffer;
struct Image;
}

namespace gltf
{
enum AccessorCompType
{
    BYTE = 1,
    UNSIGNED_BYTE = 1,
    SHORT = 2,
    UNSIGNED_SHORT = 2,
    UNSIGNED_INT = 4,
    FLOAT = 4
};

enum AccessorType
{
    SCALAR = 1,
    VEC2 = 2,
    VEC3 = 3,
    VEC4 = 4,
    MAT2 = 4,
    MAT3 = 9,
    MAT4 = 16
};

enum PrimitiveMode
{
    POINTS = 0,
    LINES = 1,
    LINE_LOOP = 2,
    LINE_STRIP = 3,
    TRIANGLES = 4,
    TRIANGLE_STRIP = 5,
    TRIANGLE_FAN = 6
};

struct BufViewData
{
    uint8_t* data = nullptr;
    uint32_t byte_length = 0;
    uint32_t byte_stride = 0;
};

struct AccessorData
{
    uint8_t* data = nullptr;
    uint32_t count;
    AccessorCompType comp_type;
    AccessorType type;
    BufViewData buf_view;
};

struct Vertex
{
    glm::vec4 position{0, 0, 0, 0};
    glm::vec4 normal{0, 0, 0, 0};
    glm::vec2 uv_0{0, 0};
    glm::vec2 uv_1{0, 0};
};

struct Image
{
    std::string uri;
    Handle<vk::Image> handle = Handle<vk::Image>::invalid();
};

struct Texture
{
    uint32_t source;
};

struct Material
{
    glm::vec4 base_color_factor{1, 1, 1, 1};
    uint32_t base_color_tex;
};

struct Primitive
{
    uint32_t vertex_start;
    uint32_t index_start;
    uint32_t index_count;
    uint32_t material;
    uint32_t mode = TRIANGLES;
};

struct Mesh
{
    std::vector<uint32_t> primitives;
};

struct Node
{
    std::vector<uint32_t> children;
    std::optional<uint32_t> mesh;
    glm::mat4 transform{1.0f};
};

struct Model
{
    std::vector<uint32_t> scene;
    std::vector<Node> nodes;
    std::vector<Mesh> meshes;
    std::vector<Primitive> primitives;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Material> materials;
    std::vector<Texture> textures;
    std::vector<Image> images;

    Handle<vk::Buffer> vertex_buffer;
    Handle<vk::Buffer> index_buffer;
};

class Loader
{
public:
    Model load(const std::filesystem::path& path, vk::Device& device);

private:
    using json = nlohmann::json;

    vk::Device* p_device_;
    Model* model_;
    std::filesystem::path dir_;
    json gltf_;
    std::vector<std::vector<uint8_t>> buffers_;

    void load_buffers();
    void load_images();
    void load_textures();
    void load_materials();
    void load_meshes();
    void load_primitive(const json& p);
    void load_nodes();

    void compute_nodes_transform();
    void compute_node_transform(Node& node, const glm::mat4& transform);

    AccessorData get_accessor_data(int accessor_idx);
    BufViewData get_buf_view_data(int buf_view_idx);

    void gpu_upload();
};

Model load_model(const std::filesystem::path& path, vk::Device& device);
} // namespace gltf
