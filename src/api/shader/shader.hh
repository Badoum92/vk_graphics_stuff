#pragma once

#include <vulkan/vulkan.h>
#include <string>

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

    Shader(const std::string& file_path, Stage stage);
    ~Shader();

    const VkPipelineShaderStageCreateInfo& stage_info() const;

private:
    VkShaderModule handle_;
    VkPipelineShaderStageCreateInfo stage_info_{};
};
