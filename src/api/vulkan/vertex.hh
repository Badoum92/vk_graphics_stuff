#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

template <typename VERTEX>
class VertexDescription
{
public:
    VertexDescription(uint32_t binding)
    {
        binding_.binding = binding;
        binding_.stride = sizeof(VERTEX);
        binding_.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }

    template <typename T>
    void add_attribute()
    {
        uint32_t location = attributes_.size();
        attributes_.emplace_back();
        attributes_[location].binding = binding_.binding;
        attributes_[location].location = location;
        attributes_[location].format = format_from_type<T>();
        attributes_[location].offset = offset_;
        offset_ += sizeof(T);
    }

    const VkVertexInputBindingDescription& binding() const
    {
        return binding_;
    }

    const std::vector<VkVertexInputAttributeDescription>& attributes() const
    {
        return attributes_;
    }

private:
    VkVertexInputBindingDescription binding_{};
    std::vector<VkVertexInputAttributeDescription> attributes_;

    uint32_t offset_{0};

    template <typename T>
    constexpr VkFormat format_from_type()
    {
        if constexpr (std::is_same_v<float, T>)
            return VK_FORMAT_R32_SFLOAT;
        if constexpr (std::is_same_v<glm::vec2, T>)
            return VK_FORMAT_R32G32_SFLOAT;
        if constexpr (std::is_same_v<glm::vec3, T>)
            return VK_FORMAT_R32G32B32_SFLOAT;
        if constexpr (std::is_same_v<glm::vec4, T>)
            return VK_FORMAT_R32G32B32A32_SFLOAT;
    }
};

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VertexDescription<Vertex> get_description(uint32_t binding)
    {
        VertexDescription<Vertex> desc(binding);
        desc.add_attribute<glm::vec2>();
        desc.add_attribute<glm::vec3>();
        return desc;
    }
};
