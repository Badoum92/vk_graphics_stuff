#pragma once

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>
#include <string>

#include "descriptor_set/descriptor_set.hh"

class Shader
{
public:
    enum class Stage
    {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        COMPUTE
    };

    void create(const std::string& vertex, const std::string& fragment);
    void create(const std::string& compute);
    void destroy();

    DescriptorSet& get_descriptor_set(size_t n);

    const std::vector<VkPipelineShaderStageCreateInfo>& stages_info() const;
    const std::vector<VkDescriptorSetLayout>& set_layouts() const;

private:
    std::vector<VkShaderModule> handles_;
    std::vector<VkPipelineShaderStageCreateInfo> stages_info_;
    std::vector<VkDescriptorSetLayout> set_layouts_;
    std::vector<DescriptorSet::Layout> tmp_set_layouts_;
    std::vector<DescriptorSet> descriptor_sets_;

    void process(const std::string& file_path, Stage stage);
    void create_descriptor_sets();
};
