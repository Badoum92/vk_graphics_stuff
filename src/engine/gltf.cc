#include "gltf.hh"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cassert>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>

#include "tools.hh"
#include "device.hh"

namespace gltf
{
template <typename T>
static T get_value(void* data)
{
    return *reinterpret_cast<T*>(data);
}

static AccessorCompType int_to_accessor_comp_type(int i)
{
    switch (i)
    {
    case 5120:
        return AccessorCompType::BYTE;
    case 5121:
        return AccessorCompType::UNSIGNED_BYTE;
    case 5122:
        return AccessorCompType::SHORT;
    case 5123:
        return AccessorCompType::UNSIGNED_SHORT;
    case 5125:
        return AccessorCompType::UNSIGNED_INT;
    case 5126:
        return AccessorCompType::FLOAT;
    }
    assert(!"Unknown AccessorCompType");
    return AccessorCompType::BYTE;
}

static AccessorType string_to_accessor_type(const std::string& s)
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
    assert(!"Unknown AccessorType");
    return AccessorType::SCALAR;
}

Model load_model(const std::filesystem::path& path, vk::Device& device)
{
    Loader loader{};
    return loader.load(path, device);
}

Model Loader::load(const std::filesystem::path& path, vk::Device& device)
{
    std::cout << "Loading model " << path.filename() << " ...\n";
    buffers_.clear();

    p_device_ = &device;
    dir_ = path;
    dir_.remove_filename();
    std::ifstream gltf_file(path);
    gltf_ = nlohmann::json::parse(gltf_file);

    Model model;
    model_ = &model;

    load_buffers();
    load_images();
    load_textures();
    load_materials();
    load_meshes();
    load_nodes();

    int scene_idx = gltf_["scene"];
    auto gltf_scene = gltf_["scenes"][scene_idx];
    for (int node_idx : gltf_scene["nodes"])
    {
        model.scene.push_back(node_idx);
    }

    compute_nodes_transform();
    gpu_upload();

    std::cout << "Done\n";

    return model;
}

void Loader::load_buffers()
{
    auto buffers = gltf_["buffers"];
    for (const auto& buf : buffers)
    {
        std::string uri = buf["uri"];
        auto path = dir_ / uri;
        buffers_.push_back(tools::read_file<std::vector<uint8_t>>(path));
    }
}

void Loader::load_images()
{
    auto images = gltf_["images"];
    model_->images.reserve(images.size());
    for (const auto& img : images)
    {
        std::string uri = img["uri"];
        model_->images.push_back({(dir_ / uri).string()});
    }
}

void Loader::load_textures()
{
    auto textures = gltf_["textures"];
    model_->textures.reserve(textures.size());
    for (const auto& tex : textures)
    {
        auto source = tex["source"];
        model_->textures.push_back({source});
    }
}

void Loader::load_materials()
{
    auto materials = gltf_["materials"];
    model_->materials.reserve(materials.size());
    for (const auto& mat : materials)
    {
        Material material;
        auto pbr = mat["pbrMetallicRoughness"];
        for (size_t i = 0; i < 4; ++i)
            material.base_color_factor[i] = pbr["baseColorFactor"][i];
        material.base_color_tex = pbr["baseColorTexture"]["index"];

        model_->materials.push_back(material);
    }
}

void Loader::load_meshes()
{
    auto meshes = gltf_["meshes"];
    model_->meshes.reserve(meshes.size());
    for (const auto& m : meshes)
    {
        Mesh mesh;
        auto primitives = m["primitives"];
        for (const auto& p : primitives)
        {
            mesh.primitives.push_back(model_->primitives.size());
            load_primitive(p);
        }
        model_->meshes.push_back(mesh);
    }
}

void Loader::load_primitive(const json& p)
{
    Primitive primitive;

    primitive.vertex_start = model_->vertices.size();
    primitive.index_start = model_->indices.size();
    if (p.count("material"))
    {
        primitive.material = p["material"];
    }
    if (p.count("mode"))
    {
        primitive.mode = p["mode"];
    }

    // indices
    int indices_idx = p["indices"];
    auto indices_accessor = get_accessor_data(indices_idx);
    assert(indices_accessor.type == AccessorType::SCALAR);
    primitive.index_count = indices_accessor.count;

    uint8_t* indices_data = indices_accessor.data;
    int step = indices_accessor.comp_type;
    if (indices_accessor.buf_view.byte_stride)
        step = indices_accessor.buf_view.byte_stride;

    if (indices_accessor.comp_type == AccessorCompType::BYTE)
    {
        for (unsigned i = 0; i < indices_accessor.count; ++i, indices_data += step)
        {
            model_->indices.push_back(primitive.vertex_start + get_value<uint8_t>(indices_data));
        }
    }
    else if (indices_accessor.comp_type == AccessorCompType::UNSIGNED_SHORT)
    {
        for (unsigned i = 0; i < indices_accessor.count; ++i, indices_data += step)
        {
            model_->indices.push_back(primitive.vertex_start + get_value<unsigned short>(indices_data));
        }
    }
    else if (indices_accessor.comp_type == AccessorCompType::UNSIGNED_INT)
    {
        for (unsigned i = 0; i < indices_accessor.count; ++i, indices_data += step)
        {
            model_->indices.push_back(primitive.vertex_start + get_value<unsigned int>(indices_data));
        }
    }
    else
    {
        assert(!"Invalid gltf index type");
    }

    // attributes
    auto attributes = p["attributes"];
    int pos_idx = attributes.value("POSITION", -1);
    auto pos = get_accessor_data(pos_idx);
    auto pos_data = pos.data;
    int pos_step = sizeof(glm::vec3);
    if (pos.buf_view.byte_stride)
        pos_step = pos.buf_view.byte_stride;

    int normal_idx = attributes.value("NORMAL", -1);
    auto normal = get_accessor_data(normal_idx);
    auto normal_data = normal.data;
    int normal_step = sizeof(glm::vec3);
    if (normal.buf_view.byte_stride)
        normal_step = normal.buf_view.byte_stride;

    int uv_0_idx = attributes.value("TEXCOORD_0", -1);
    auto uv_0 = get_accessor_data(uv_0_idx);
    auto texcoords_0_data = uv_0.data;
    int uv_0_step = sizeof(glm::vec2);
    if (uv_0.buf_view.byte_stride)
        uv_0_step = uv_0.buf_view.byte_stride;

    for (unsigned i = 0; i < pos.count; ++i)
    {
        Vertex v;
        if (pos_data)
        {
            v.position = glm::vec4{get_value<glm::vec3>(pos_data), 1.0f};
            pos_data += pos_step;
        }
        if (normal_data)
        {
            v.normal = glm::vec4{get_value<glm::vec3>(normal_data), 1.0f};
            normal_data += normal_step;
        }
        if (texcoords_0_data)
        {
            v.uv_0 = get_value<glm::vec2>(texcoords_0_data);
            texcoords_0_data += uv_0_step;
        }
        model_->vertices.push_back(v);
    }

    model_->primitives.push_back(primitive);
}

void Loader::load_nodes()
{
    auto nodes = gltf_["nodes"];
    model_->nodes.reserve(nodes.size());
    for (const auto& n : nodes)
    {
        Node node;

        if (n.count("mesh"))
        {
            node.mesh = n["mesh"];
        }

        if (n.count("children"))
        {
            for (unsigned child : n["children"])
            {
                node.children.push_back(child);
            }
        }

        if (n.count("matrix"))
        {
            auto matrix = n["matrix"];
            for (int y = 0; y < 4; ++y)
            {
                for (int x = 0; x < 4; ++x)
                {
                    node.transform[y][x] = matrix[y * 4 + x];
                }
            }
        }
        else
        {
            glm::mat4 translation{1.0f};
            glm::mat4 rotation{1.0f};
            glm::mat4 scale{1.0f};

            if (n.count("translation"))
            {
                const auto& t = n["translation"];
                float x = t[0];
                float y = t[1];
                float z = t[2];
                translation = glm::translate(translation, {x, y, z});
            }
            if (n.count("rotation"))
            {
                const auto& r = n["rotation"];
                float x = r[0];
                float y = r[1];
                float z = r[2];
                float w = r[3];
                rotation = glm::toMat4(glm::qua{x, y, z, w});
            }
            if (n.count("scale"))
            {
                const auto& s = n["scale"];
                float x = s[0];
                float y = s[1];
                float z = s[2];
                scale = glm::scale(scale, {x, y, z});
            }

            node.transform = translation * rotation * scale;
        }

        model_->nodes.push_back(node);
    }
}

void Loader::compute_nodes_transform()
{
    for (unsigned node_idx : model_->scene)
    {
        compute_node_transform(model_->nodes[node_idx], glm::mat4(1.0f));
    }
}

void Loader::compute_node_transform(Node& node, const glm::mat4& transform)
{
    node.transform = transform * node.transform;
    for (unsigned node_idx : node.children)
    {
        compute_node_transform(model_->nodes[node_idx], node.transform);
    }
}

BufViewData Loader::get_buf_view_data(int buf_view_idx)
{
    auto buf_view = gltf_["bufferViews"][buf_view_idx];
    int buf_idx = buf_view["buffer"];
    unsigned byte_offset = buf_view.value("byteOffset", 0);
    unsigned byte_length = buf_view["byteLength"];
    unsigned byte_stride = buf_view.value("byteStride", 0);
    return {buffers_[buf_idx].data() + byte_offset, byte_length, byte_stride};
}

AccessorData Loader::get_accessor_data(int accessor_idx)
{
    AccessorData accessor_data;

    if (accessor_idx == -1)
    {
        return accessor_data;
    }

    auto accessor = gltf_["accessors"][accessor_idx];
    int buf_view_idx = accessor["bufferView"];
    int byte_offset = accessor.value("byteOffset", 0);
    accessor_data.buf_view = get_buf_view_data(buf_view_idx);

    int component_type = accessor["componentType"];
    int count = accessor["count"];
    std::string type = accessor["type"];

    accessor_data.data = accessor_data.buf_view.data + byte_offset;
    accessor_data.count = count;
    accessor_data.comp_type = int_to_accessor_comp_type(component_type);
    accessor_data.type = string_to_accessor_type(type);

    return accessor_data;
}

void Loader::gpu_upload()
{
    auto& cmd = p_device_->get_transfer_command();

    model_->vertex_buffer =
        p_device_->create_buffer({.size = static_cast<uint32_t>(model_->vertices.size() * sizeof(Vertex))});
    model_->index_buffer =
        p_device_->create_buffer({.size = static_cast<uint32_t>(model_->indices.size() * sizeof(uint32_t)),
                                  .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT});

    cmd.upload_buffer(model_->vertex_buffer, model_->vertices.data(), model_->vertices.size() * sizeof(Vertex));
    cmd.upload_buffer(model_->index_buffer, model_->indices.data(), model_->indices.size() * sizeof(uint32_t));

    for (auto& image : model_->images)
    {
        image.handle = p_device_->create_image({}, image.uri);
        cmd.upload_image(image.handle, image.uri);
    }

    p_device_->submit_blocking(cmd);
}
} // namespace gltf
