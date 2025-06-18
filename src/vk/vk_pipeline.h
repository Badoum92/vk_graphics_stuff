#pragma once

#include <volk.h>

#include "vk/vk_constants.h"

namespace vk
{
struct shader;

struct graphics_state
{
    static graphics_state create();

    uint32_t polygon_mode : 2;
    uint32_t cull_back_faces : 1;
    uint32_t front_face_clockwise : 1;
    uint32_t depth_test_enabled : 1;
    uint32_t depth_write_enabled : 1;
    uint32_t depth_compare_op : 3;
    uint32_t padding : 23;
};
static_assert(sizeof(graphics_state) == sizeof(uint32_t));
bool operator==(const graphics_state& a, const graphics_state& b);

struct graphics_pipeline_description
{
    shader* vertex_shader;
    shader* fragment_shader;
    VkFormat color_formats[max_color_attachments];
    uint32_t num_color_formats;
    VkFormat depth_format;
    uint32_t push_constant_size;
    const char* name;
};

struct graphics_pipeline
{
    VkPipelineLayout layout;
    VkPipeline vk_handles[max_graphics_states];
    graphics_state graphics_states[max_graphics_states];
    uint32_t num_graphics_states;
    graphics_pipeline_description description;
};

struct compute_pipeline_description
{
    shader* shader;
    uint32_t push_constant_size;
    const char* name;
};

struct compute_pipeline
{
    VkPipelineLayout layout;
    VkPipeline vk_handle = VK_NULL_HANDLE;
    compute_pipeline_description description;
};
} // namespace vk
