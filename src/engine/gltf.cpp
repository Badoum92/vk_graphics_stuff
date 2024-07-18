#if 0
#include "gltf.h"

#include "bul/bul.h"
#include "bul/file.h"

#include <iostream>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace gltf
{
using rapidjson_document = rapidjson::Document;
using rapidjson_array = rapidjson::GenericArray<true, rapidjson::Value>;
using rapidjson_size = rapidjson::SizeType;
using Buffer = std::vector<uint8_t>;

enum AccessorComponentType
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

struct BufferView
{
    const uint8_t* data = nullptr;
    uint32_t byte_length = 0;
    uint32_t byte_stride = 0;
};

struct Accessor
{
    const uint8_t* data = nullptr;
    uint32_t count;
    AccessorComponentType component_type;
    AccessorType type;
    BufferView buffer_view;
};

template <typename T>
static T get_value(const void* data)
{
    return *(const T*)data;
}

static AccessorComponentType uint_to_accessor_component_type(uint32_t i)
{
    switch (i)
    {
    case 5120:
        return AccessorComponentType::BYTE;
    case 5121:
        return AccessorComponentType::UNSIGNED_BYTE;
    case 5122:
        return AccessorComponentType::SHORT;
    case 5123:
        return AccessorComponentType::UNSIGNED_SHORT;
    case 5125:
        return AccessorComponentType::UNSIGNED_INT;
    case 5126:
        return AccessorComponentType::FLOAT;
    }
    ASSERT(!"Unknown AccessorComponentType");
    return AccessorComponentType::BYTE;
}

static AccessorType string_to_accessor_type(std::string_view s)
{
    if (s == "SCALAR")
        return AccessorType::SCALAR;
    else if (s == "VEC2")
        return AccessorType::VEC2;
    else if (s == "VEC3")
        return AccessorType::VEC3;
    else if (s == "VEC4")
        return AccessorType::VEC4;
    else if (s == "MAT2")
        return AccessorType::MAT2;
    else if (s == "MAT3")
        return AccessorType::MAT3;
    else if (s == "MAT4")
        return AccessorType::MAT4;
    ASSERT(!"Unknown AccessorType");
    return AccessorType::SCALAR;
}

static BufferView get_buffer_view(rapidjson_document& json, uint64_t buffer_view_index,
                                  const std::vector<Buffer>& buffers)
{
    const auto& buffer_view_json = json["bufferViews"].GetArray()[(rapidjson_size)buffer_view_index];
    uint64_t buffer_index = buffer_view_json["buffer"].GetUint64();

    uint32_t byte_offset = 0;
    if (buffer_view_json.HasMember("byteOffset"))
    {
        byte_offset = buffer_view_json["byteOffset"].GetUint();
    }

    uint32_t byte_length = buffer_view_json["byteLength"].GetUint();

    uint32_t byte_stride = 0;
    if (buffer_view_json.HasMember("byteStride"))
    {
        byte_stride = buffer_view_json["byteStride"].GetUint();
    }

    return {buffers[buffer_index].data() + byte_offset, byte_length, byte_stride};
}

static Accessor get_accessor(rapidjson_document& json, uint64_t accessor_index, const std::vector<Buffer>& buffers)
{
    Accessor accessor;

    if (accessor_index == (uint64_t)-1)
    {
        return accessor;
    }

    const auto& json_accessor = json["accessors"].GetArray()[(rapidjson_size)accessor_index];
    uint64_t buffer_view_index = json_accessor["bufferView"].GetUint64();
    uint64_t byte_offset = 0;
    if (json_accessor.HasMember("byteOffset"))
    {
        byte_offset = json_accessor["byteOffset"].GetUint64();
    }

    accessor.buffer_view = get_buffer_view(json, buffer_view_index, buffers);
    accessor.data = accessor.buffer_view.data + byte_offset;
    accessor.count = json_accessor["count"].GetUint();
    accessor.component_type = uint_to_accessor_component_type(json_accessor["componentType"].GetUint());
    accessor.type = string_to_accessor_type(json_accessor["type"].GetString());
    return accessor;
}

static std::vector<Buffer> load_buffers(const std::string& dir_path, rapidjson_document& json)
{
    std::vector<Buffer> buffers;
    const auto& json_buffers = json["buffers"].GetArray();
    buffers.reserve(json_buffers.Size());
    for (const auto& json_buffer : json_buffers)
    {
        std::string_view uri = json_buffer["uri"].GetString();
        std::string buffer_path = dir_path + uri.data();
        Buffer& buffer = buffers.emplace_back();
        // bul::read_file(buffer_path.c_str(), buffer);
    }
    return buffers;
}

static std::vector<Image> load_images(const std::string& dir_path, rapidjson_document& json)
{
    std::vector<Image> images;
    const auto& json_images = json["images"].GetArray();
    images.reserve(json_images.Size());
    for (const auto& json_image : json_images)
    {
        std::string_view uri = json_image["uri"].GetString();
        images.emplace_back(Image{dir_path + uri.data()});
    }
    return images;
}

static std::vector<Texture> load_textures(rapidjson_document& json)
{
    std::vector<Texture> textures;
    const auto& json_textures = json["textures"].GetArray();
    textures.reserve(json_textures.Size());
    for (const auto& json_texture : json_textures)
    {
        uint32_t source_image = json_texture["source"].GetUint();
        textures.emplace_back(Texture{source_image});
    }
    return textures;
}

static std::vector<Material> load_materials(rapidjson_document& json)
{
    std::vector<Material> materials;
    const auto& json_materials = json["materials"].GetArray();
    materials.reserve(json_materials.Size());
    for (const auto& json_material : json_materials)
    {
        Material& material = materials.emplace_back();
        const auto& pbr = json_material["pbrMetallicRoughness"];
        const auto& base_color_factor = pbr["baseColorFactor"].GetArray();
        for (size_t i = 0; i < 4; ++i)
        {
            material.base_color_factor[i] = base_color_factor[i].GetFloat();
        }
        material.base_color_tex = pbr["baseColorTexture"]["index"].GetUint();
    }
    return materials;
}

static std::vector<Primitive> load_primitives(rapidjson_document& json, const rapidjson_array& json_primitives,
                                              const std::vector<Buffer>& buffers, std::vector<Vertex>& vertices,
                                              std::vector<uint32_t>& indices)
{
    std::vector<Primitive> primitives;
    primitives.reserve(json_primitives.Size());
    for (const auto& json_primitive : json_primitives)
    {
        Primitive& primitive = primitives.emplace_back();
        primitive.vertex_start = (uint32_t)vertices.size();
        primitive.index_start = (uint32_t)indices.size();

        if (json_primitive.HasMember("material"))
        {
            primitive.material = json_primitive["material"].GetUint();
        }
        if (json_primitive.HasMember("mode"))
        {
            primitive.mode = (PrimitiveMode)json_primitive["mode"].GetUint();
        }

        uint64_t indices_index = json_primitive["indices"].GetUint64();
        Accessor indices_accessor = get_accessor(json, indices_index, buffers);
        ASSERT(indices_accessor.type == AccessorType::SCALAR);
        primitive.index_count = indices_accessor.count;

        const uint8_t* indices_data = indices_accessor.data;
        uint32_t step = indices_accessor.component_type;
        if (indices_accessor.buffer_view.byte_stride)
        {
            step = indices_accessor.buffer_view.byte_stride;
        }

        if (indices_accessor.component_type == AccessorComponentType::BYTE)
        {
            for (size_t i = 0; i < indices_accessor.count; ++i, indices_data += step)
            {
                indices.push_back(primitive.vertex_start + get_value<uint8_t>(indices_data));
            }
        }
        else if (indices_accessor.component_type == AccessorComponentType::UNSIGNED_SHORT)
        {
            for (size_t i = 0; i < indices_accessor.count; ++i, indices_data += step)
            {
                indices.push_back(primitive.vertex_start + get_value<uint16_t>(indices_data));
            }
        }
        else if (indices_accessor.component_type == AccessorComponentType::UNSIGNED_INT)
        {
            for (size_t i = 0; i < indices_accessor.count; ++i, indices_data += step)
            {
                indices.push_back(primitive.vertex_start + get_value<uint32_t>(indices_data));
            }
        }
        else
        {
            ASSERT_FMT(false, "Invalid gltf index type");
        }

        const auto& json_attributes = json_primitive["attributes"];

        uint64_t position_accessor_index = -1;
        if (json_attributes.HasMember("POSITION"))
        {
            position_accessor_index = json_attributes["POSITION"].GetUint64();
        }
        Accessor position_accessor = get_accessor(json, position_accessor_index, buffers);
        const uint8_t* position_data = position_accessor.data;
        uint32_t position_step = sizeof(bul::vec3f);
        if (position_accessor.buffer_view.byte_stride)
        {
            position_step = position_accessor.buffer_view.byte_stride;
        }

        uint64_t normal_accessor_index = -1;
        if (json_attributes.HasMember("NORMAL"))
        {
            normal_accessor_index = json_attributes["NORMAL"].GetUint64();
        }
        Accessor normal_accessor = get_accessor(json, normal_accessor_index, buffers);
        const uint8_t* normal_data = normal_accessor.data;
        uint32_t normal_step = sizeof(bul::vec3f);
        if (normal_accessor.buffer_view.byte_stride)
        {
            normal_step = normal_accessor.buffer_view.byte_stride;
        }

        uint64_t uv_0_accessor_index = -1;
        if (json_attributes.HasMember("TEXCOORD_0"))
        {
            uv_0_accessor_index = json_attributes["TEXCOORD_0"].GetUint64();
        }
        Accessor uv_0_accessor = get_accessor(json, uv_0_accessor_index, buffers);
        const uint8_t* uv_0_data = uv_0_accessor.data;
        uint32_t uv_0_step = sizeof(bul::vec2f);
        if (uv_0_accessor.buffer_view.byte_stride)
        {
            uv_0_step = uv_0_accessor.buffer_view.byte_stride;
        }

        size_t count = std::max(position_accessor.count, std::max(normal_accessor.count, uv_0_accessor.count));
        for (size_t i = 0; i < count; ++i)
        {
            Vertex& vertex = vertices.emplace_back();
            if (position_data)
            {
                vertex.position = bul::vec4f{get_value<bul::vec3f>(position_data), 1.0f};
                position_data += position_step;
            }
            if (normal_data)
            {
                vertex.normal = bul::vec4f{get_value<bul::vec3f>(normal_data), 1.0f};
                normal_data += normal_step;
            }
            if (uv_0_data)
            {
                vertex.uv_0 = get_value<bul::vec2f>(uv_0_data);
                uv_0_data += uv_0_step;
            }
        }
    }
    return primitives;
}

static std::vector<Mesh> load_meshes(rapidjson_document& json, const std::vector<Buffer>& buffers,
                                     std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    std::vector<Mesh> meshes;
    const auto& json_meshes = json["meshes"].GetArray();
    meshes.reserve(json_meshes.Size());
    for (const auto& json_mesh : json_meshes)
    {
        Mesh& mesh = meshes.emplace_back();
        const auto& json_primitives = json_mesh["primitives"].GetArray();
        mesh.primitives = load_primitives(json, json_primitives, buffers, vertices, indices);
    }
    return meshes;
}

static std::vector<Node> load_nodes(rapidjson_document& json)
{
    std::vector<Node> nodes;
    const auto& json_nodes = json["nodes"].GetArray();
    nodes.reserve(json_nodes.Size());
    for (const auto& json_node : json_nodes)
    {
        Node& node = nodes.emplace_back();
        if (json_node.HasMember("mesh"))
        {
            node.mesh = json_node["mesh"].GetUint();
        }

        if (json_node.HasMember("children"))
        {
            const auto& json_children = json_node["children"].GetArray();
            node.children.reserve(json_children.Size());
            for (const auto& json_child : json_children)
            {
                node.children.push_back(json_child.GetUint());
            }
        }

        if (json_node.HasMember("matrix"))
        {
            const auto& json_matrix = json_node["matrix"].GetArray();
            for (size_t y = 0; y < 4; ++y)
            {
                for (size_t x = 0; x < 4; ++x)
                {
                    node.transform[y][x] = json_matrix[y * 4 + x].GetFloat();
                }
            }
        }
        else
        {
            bul::mat4f translation = bul::mat4f::identity();
            bul::mat4f rotation = bul::mat4f::identity();
            bul::mat4f scale = bul::mat4f::identity();

            if (json_node.HasMember("translation"))
            {
                const auto& json_translation = json_node["translation"];
                float x = json_translation[0].GetFloat();
                float y = json_translation[1].GetFloat();
                float z = json_translation[2].GetFloat();
                translation = bul::translation({x, y, z});
            }
            if (json_node.HasMember("rotation"))
            {
                const auto& json_rotation = json_node["rotation"];
                float x = json_rotation[0].GetFloat();
                float y = json_rotation[1].GetFloat();
                float z = json_rotation[2].GetFloat();
                float w = json_rotation[3].GetFloat();
                bul::rotation({x, y, z, w});
            }
            if (json_node.HasMember("scale"))
            {
                const auto& json_scale = json_node["scale"];
                float x = json_scale[0].GetFloat();
                float y = json_scale[1].GetFloat();
                float z = json_scale[2].GetFloat();
                scale = bul::scale({x, y, z});
            }

            node.transform = translation * rotation * scale;
        }
    }
    return nodes;
}

static std::vector<uint32_t> load_scene(rapidjson_document& json)
{
    std::vector<uint32_t> scene;
    uint32_t scene_index = json["scene"].GetUint();
    const auto& json_scene = json["scenes"].GetArray()[scene_index];
    const auto& json_scene_nodes = json_scene["nodes"].GetArray();
    scene.reserve(json_scene_nodes.Size());
    for (const auto& node_index : json_scene_nodes)
    {
        scene.push_back(node_index.GetUint());
    }
    return scene;
}

static void compute_nodes_transform(Node& node, const bul::mat4f& transform, std::vector<Node>& nodes)
{
    node.transform = transform * node.transform;
    for (uint32_t node_index : node.children)
    {
        compute_nodes_transform(nodes[node_index], node.transform, nodes);
    }
}

static void compute_nodes_transform(const std::vector<uint32_t>& scene, std::vector<Node>& nodes)
{
    for (uint32_t node_index : scene)
    {
        compute_nodes_transform(nodes[node_index], bul::mat4f::identity(), nodes);
    }
}

Model load(std::string_view gltf_path)
{
    std::cout << "Loading model " << gltf_path << "...\n";

    size_t dir_separator_index = gltf_path.find_last_of('/');
    if (dir_separator_index == std::string_view::npos)
    {
        dir_separator_index = gltf_path.find_last_of('\\');
    }
    std::string dir_path{gltf_path.substr(0, dir_separator_index + 1)};

    std::vector<uint8_t> json_data;
    // bul::read_file(gltf_path.data(), json_data);
    rapidjson_document json;
    json.Parse((const char*)json_data.data(), json_data.size());

    const auto& buffers = load_buffers(dir_path, json);

    Model model;
    model.images = load_images(dir_path, json);
    model.textures = load_textures(json);
    model.materials = load_materials(json);
    model.meshes = load_meshes(json, buffers, model.vertices, model.indices);
    model.nodes = load_nodes(json);
    model.scene_nodes = load_scene(json);
    compute_nodes_transform(model.scene_nodes, model.nodes);
    return model;
}
} // namespace gltf
#endif