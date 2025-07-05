#include "debug_draw.h"

#include "camera.h"

#include "core/math/math.h"
#include "core/math/matrix.h"
#include "core/math/vector.h"
#include "core/containers/vector.h"

#include "vk/vk_context.h"
#include "vk/vk_surface.h"
#include "vk/vk_shader.h"
#include "vk/vk_pipeline.h"
#include "vk/vk_image.h"
#include "vk/vk_buffer.h"
#include "vk/vk_commands.h"

static vk::shader* vertex_shader = nullptr;
static vk::shader* fragment_shader = nullptr;
static vk::graphics_pipeline* graphics_pipeline = nullptr;

static vk::buffer* vertex_buffer = nullptr;
static uint32_t max_num_vertices = 0;

struct debug_vertex
{
    vec3f position;
    color color;
};

static bul::vector<debug_vertex> lines_3d;
static bul::vector<debug_vertex> triangles_3d;
static uint32_t line_width = 0;

struct push_constant
{
    mat4f view_proj;
    VkDeviceAddress vertex_buffer;
};

void debug_draw_init(vk::context* vk_context)
{
    vertex_shader = vk_context->create_shader("shaders/spv/debug_draw.vert");
    fragment_shader = vk_context->create_shader("shaders/spv/debug_draw.frag");

    vk::graphics_pipeline_description pipeline_desc = {};
    pipeline_desc.vertex_shader = vertex_shader;
    pipeline_desc.fragment_shader = fragment_shader;
    pipeline_desc.color_formats[pipeline_desc.num_color_formats++] = vk_context->surface.images[0]->full_view.format;
    pipeline_desc.depth_format = VK_FORMAT_D32_SFLOAT;
    pipeline_desc.push_constant_size = sizeof(push_constant);
    pipeline_desc.name = "debug draw pipeline";
    graphics_pipeline = vk_context->create_graphics_pipeline(pipeline_desc);
}

void debug_draw_shutdown(vk::context* vk_context)
{
    vk_context->destroy_shader(vertex_shader);
    vk_context->destroy_shader(fragment_shader);
    vk_context->destroy_graphics_pipeline(graphics_pipeline);
    if (vertex_buffer != nullptr)
    {
        vk_context->destroy_buffer(vertex_buffer);
    }
}

void debug_draw_render(vk::context* vk_context, vk::command_buffer* cmd, vk::image* render_target, camera* camera)
{
    uint32_t num_vertices = lines_3d.size + triangles_3d.size;

    if (num_vertices > max_num_vertices)
    {
        vk_context->wait_idle();
        if (vertex_buffer != nullptr)
        {
            vk_context->destroy_buffer(vertex_buffer);
        }
        max_num_vertices = num_vertices;
        vk::buffer_description buffer_description = {};
        buffer_description.size = max_num_vertices * sizeof(debug_vertex);
        buffer_description.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        buffer_description.name = "debug draw vertex buffer";
        vertex_buffer = vk_context->create_buffer(buffer_description);
    }

    memcpy(vertex_buffer->mapped_data, triangles_3d.data, triangles_3d.size_bytes());
    memcpy(vertex_buffer->mapped_data + triangles_3d.size_bytes(), lines_3d.data, lines_3d.size_bytes());

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = {render_target->description.width, render_target->description.height};
    cmd->set_scissor(scissor);

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)render_target->description.width;
    viewport.height = (float)render_target->description.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    cmd->set_viewport(viewport);

    cmd->barrier(render_target, vk::image_usage::color_attachment);
    cmd->begin_rendering({{render_target}}, {{vk::load_op::load()}}, nullptr, vk::load_op::clear_depth());

    push_constant push_constant;
    push_constant.view_proj = camera->proj * camera->view;
    push_constant.vertex_buffer = vertex_buffer->device_address;
    cmd->push_constant(graphics_pipeline, &push_constant, sizeof(push_constant));

    vk::graphics_state graphics_state = vk::graphics_state::create();

    graphics_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    graphics_state.cull_back_faces = true;
    cmd->bind_graphics_pipeline(graphics_pipeline, graphics_state);
    cmd->draw(triangles_3d.size, 0);

    graphics_state.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    graphics_state.line_width = line_width;
    graphics_state.cull_back_faces = false;
    cmd->bind_graphics_pipeline(graphics_pipeline, graphics_state);
    cmd->draw(lines_3d.size, triangles_3d.size);

    cmd->end_rendering();

    lines_3d.clear();
    triangles_3d.clear();
}

void debug_draw_set_line_width(uint32_t width)
{
    line_width = CLAMP(width, 1, 4) - 1;
}

void debug_draw_line_3d(vec3f a, vec3f b, color color)
{
    lines_3d.push_back(debug_vertex{a, color});
    lines_3d.push_back(debug_vertex{b, color});
}

void debug_draw_triangle_3d(vec3f a, vec3f b, vec3f c, color color)
{
    triangles_3d.push_back(debug_vertex{a, color});
    triangles_3d.push_back(debug_vertex{b, color});
    triangles_3d.push_back(debug_vertex{c, color});
}

void debug_draw_aabb_wire(vec3f a, vec3f b, color color)
{
    // clang-format off
    vec3f v[8] = {
        {a.x, a.y, a.z},
        {b.x, a.y, a.z},
        {a.x, b.y, a.z},
        {a.x, a.y, b.z},

        {b.x, b.y, b.z},
        {a.x, b.y, b.z},
        {b.x, a.y, b.z},
        {b.x, b.y, a.z},
    };
    // clang-format on

    debug_draw_line_3d(v[0], v[1], color);
    debug_draw_line_3d(v[0], v[2], color);
    debug_draw_line_3d(v[0], v[3], color);

    debug_draw_line_3d(v[4], v[5], color);
    debug_draw_line_3d(v[4], v[6], color);
    debug_draw_line_3d(v[4], v[7], color);

    debug_draw_line_3d(v[1], v[6], color);
    debug_draw_line_3d(v[1], v[7], color);

    debug_draw_line_3d(v[2], v[5], color);
    debug_draw_line_3d(v[2], v[7], color);

    debug_draw_line_3d(v[3], v[5], color);
    debug_draw_line_3d(v[3], v[6], color);
}

void debug_draw_aabb_full(vec3f a, vec3f b, color color)
{
    // clang-format off
    vec3f v[8] = {
        {a.x, a.y, a.z},
        {b.x, a.y, a.z},
        {b.x, b.y, a.z},
        {a.x, b.y, a.z},
        {a.x, a.y, b.z},
        {b.x, a.y, b.z},
        {b.x, b.y, b.z},
        {a.x, b.y, b.z},
    };
    // clang-format on

    debug_draw_triangle_3d(v[0], v[1], v[2], color);
    debug_draw_triangle_3d(v[0], v[2], v[3], color);
    debug_draw_triangle_3d(v[4], v[5], v[1], color);
    debug_draw_triangle_3d(v[4], v[1], v[0], color);
    debug_draw_triangle_3d(v[4], v[0], v[3], color);
    debug_draw_triangle_3d(v[4], v[3], v[7], color);
    debug_draw_triangle_3d(v[1], v[5], v[6], color);
    debug_draw_triangle_3d(v[1], v[6], v[2], color);
    debug_draw_triangle_3d(v[5], v[4], v[7], color);
    debug_draw_triangle_3d(v[5], v[7], v[6], color);
    debug_draw_triangle_3d(v[3], v[2], v[6], color);
    debug_draw_triangle_3d(v[3], v[6], v[7], color);
}