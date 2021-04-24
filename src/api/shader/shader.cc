#include "shader.hh"

#include <vector>

#include "vk_context/vk_context.hh"
#include "tools.hh"

Shader::Shader(const std::string& file_path, Stage stage)
{
    auto code = Tools::read_file<std::vector<char>>(file_path);

    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());
    VK_CHECK(vkCreateShaderModule(VkContext::device, &create_info, nullptr, &handle_));

    auto vk_stage = [stage]() {
        switch (stage)
        {
        case Stage::VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case Stage::FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case Stage::GEOMETRY:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case Stage::COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        }
    }();

    stage_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_info_.stage = vk_stage;
    stage_info_.module = handle_;
    stage_info_.pName = "main";
}

Shader::~Shader()
{
    vkDestroyShaderModule(VkContext::device, handle_, nullptr);
}

const VkPipelineShaderStageCreateInfo& Shader::stage_info() const
{
    return stage_info_;
}
