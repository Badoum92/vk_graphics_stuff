#include "shader.hh"

#include <vector>
#include <SPIRV-Cross/spirv_cross.hpp>

#include "vk_context/vk_context.hh"
#include "tools.hh"

static VkShaderStageFlagBits to_vk_stage(Shader::Stage stage)
{
    switch (stage)
    {
    case Shader::Stage::VERTEX:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case Shader::Stage::FRAGMENT:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    case Shader::Stage::GEOMETRY:
        return VK_SHADER_STAGE_GEOMETRY_BIT;
    case Shader::Stage::COMPUTE:
        return VK_SHADER_STAGE_COMPUTE_BIT;
    }
}

void Shader::create(const std::string& vertex, const std::string& fragment)
{
    handles_.reserve(2);
    stages_info_.reserve(2);
    process(vertex, Stage::VERTEX);
    process(fragment, Stage::FRAGMENT);
    create_descriptor_sets();
}

void Shader::create(const std::string& compute)
{
    handles_.reserve(1);
    stages_info_.reserve(1);
    process(compute, Stage::COMPUTE);
    create_descriptor_sets();
}

#include <iostream>
void Shader::process(const std::string& file_path, Stage stage)
{
    auto code = Tools::read_file<std::vector<uint32_t>>(file_path);

    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size() * sizeof(uint32_t);
    create_info.pCode = code.data();
    handles_.emplace_back();
    VK_CHECK(vkCreateShaderModule(VkContext::device, &create_info, nullptr, &handles_.back()));

    stages_info_.emplace_back();
    stages_info_.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages_info_.back().stage = to_vk_stage(stage);
    stages_info_.back().module = handles_.back();
    stages_info_.back().pName = "main";

    spirv_cross::Compiler compiler(std::move(code));
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    auto process_resource_type = [&](const auto& resources_type, VkDescriptorType descriptor_type) {
        VkDescriptorSetLayoutBinding layout_binding{};
        layout_binding.stageFlags = VK_SHADER_STAGE_ALL;
        layout_binding.pImmutableSamplers = nullptr;
        layout_binding.descriptorType = descriptor_type;
        layout_binding.descriptorCount = 1;
        for (const auto& resource : resources_type)
        {
            const spirv_cross::SPIRType& type = compiler.get_type(resource.type_id);

            layout_binding.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            layout_binding.descriptorCount = type.array.size() == 0 ? 1 : type.array[0];

            uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            tmp_set_layouts_.resize(std::max(size_t(set + 1), tmp_set_layouts_.size()));
            tmp_set_layouts_[set].resize(std::max(size_t(layout_binding.binding + 1), tmp_set_layouts_[set].size()));
            tmp_set_layouts_[set][layout_binding.binding] = layout_binding;
        }
    };

    process_resource_type(resources.uniform_buffers, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
    process_resource_type(resources.sampled_images, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    process_resource_type(resources.storage_images, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    process_resource_type(resources.storage_buffers, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC);
}

void Shader::create_descriptor_sets()
{
    set_layouts_.resize(tmp_set_layouts_.size());
    for (uint32_t i = 0; i < tmp_set_layouts_.size(); ++i)
    {
        const auto& layout = tmp_set_layouts_[i];
        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = layout.size();
        layout_info.pBindings = layout.data();

        VK_CHECK(vkCreateDescriptorSetLayout(VkContext::device, &layout_info, nullptr, &set_layouts_[i]));

        if (i == 0)
        {
            DescriptorSet::create_global_set(set_layouts_[i], layout);
        }
        else
        {
            descriptor_sets_.emplace_back();
            descriptor_sets_.back().create(set_layouts_[i], layout);
        }
    }
}

void Shader::destroy()
{
    DescriptorSet::destroy_global_set();

    for (const auto& handle : handles_)
        vkDestroyShaderModule(VkContext::device, handle, nullptr);

    for (const auto& layout : set_layouts_)
        vkDestroyDescriptorSetLayout(VkContext::device, layout, nullptr);

    handles_.clear();
    stages_info_.clear();
    set_layouts_.clear();
    tmp_set_layouts_.clear();
    descriptor_sets_.clear();
}

DescriptorSet& Shader::descriptor_set(size_t n)
{
    if (n == 0)
        return DescriptorSet::global_set;
    else
        return descriptor_sets_[n - 1];
}

const DescriptorSet& Shader::descriptor_set(size_t n) const
{
    if (n == 0)
        return DescriptorSet::global_set;
    else
        return descriptor_sets_[n - 1];
}

const std::vector<VkPipelineShaderStageCreateInfo>& Shader::stages_info() const
{
    return stages_info_;
}

const std::vector<VkDescriptorSetLayout>& Shader::set_layouts() const
{
    return set_layouts_;
}
