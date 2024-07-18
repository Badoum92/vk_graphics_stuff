#pragma once

#include <volk.h>

#include "vk/constants.h"

#include "bul/containers/handle.h"
#include "bul/containers/static_vector.h"

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
    bul::handle<shader> vertex_shader;
    bul::handle<shader> fragment_shader;
    bul::static_vector<VkFormat, max_color_attachments> color_attachment_formats;
    VkFormat depth_attachment_format = VK_FORMAT_UNDEFINED;
    uint32_t push_constant_size = 0;
    const char* name;
};

struct graphics_pipeline
{
    VkPipelineLayout layout;
    bul::static_vector<VkPipeline, max_graphics_states> pipelines;
    bul::static_vector<graphics_state, max_graphics_states> graphics_states;
    graphics_pipeline_description description;
};
} // namespace vk