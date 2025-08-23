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

static vk::shader* vertex_shader_3d = nullptr;
static vk::shader* fragment_shader_3d = nullptr;
static vk::graphics_pipeline* pipeline_3d = nullptr;

static vk::shader* vertex_shader_2d = nullptr;
static vk::shader* fragment_shader_2d = nullptr;
static vk::graphics_pipeline* pipeline_2d = nullptr;

static vk::buffer* vertex_buffer = nullptr;
static uint32_t max_num_vertices = 0;

struct debug_vertex
{
    vec3f position;
    color color;
};

static bul::vector<debug_vertex> lines_3d;
static bul::vector<debug_vertex> triangles_3d;
static bul::vector<debug_vertex> lines_2d;
static bul::vector<debug_vertex> triangles_2d;
static uint32_t line_width = 0;

struct push_constant
{
    mat4f view_proj;
    vec2u resolution;
    VkDeviceAddress vertex_buffer;
};

void debug_draw_init(vk::context* vk_context)
{
    vertex_shader_3d = vk_context->create_shader("shaders/spv/debug_draw_3d.vert");
    fragment_shader_3d = vk_context->create_shader("shaders/spv/debug_draw_3d.frag");
    vk::graphics_pipeline_description pipeline_desc = {};
    pipeline_desc.vertex_shader = vertex_shader_3d;
    pipeline_desc.fragment_shader = fragment_shader_3d;
    pipeline_desc.color_formats[pipeline_desc.num_color_formats++] = vk_context->surface.images[0]->full_view.format;
    pipeline_desc.depth_format = VK_FORMAT_D32_SFLOAT;
    pipeline_desc.push_constant_size = sizeof(push_constant);
    pipeline_desc.name = "debug draw pipeline 3d";
    pipeline_3d = vk_context->create_graphics_pipeline(pipeline_desc);

    vertex_shader_2d = vk_context->create_shader("shaders/spv/debug_draw_2d.vert");
    fragment_shader_2d = vk_context->create_shader("shaders/spv/debug_draw_2d.frag");
    pipeline_desc.vertex_shader = vertex_shader_2d;
    pipeline_desc.fragment_shader = fragment_shader_2d;
    pipeline_desc.name = "debug draw pipeline 2d";
    pipeline_2d = vk_context->create_graphics_pipeline(pipeline_desc);
}

void debug_draw_shutdown(vk::context* vk_context)
{
    vk_context->destroy_shader(vertex_shader_3d);
    vk_context->destroy_shader(fragment_shader_3d);
    vk_context->destroy_graphics_pipeline(pipeline_3d);
    vk_context->destroy_shader(vertex_shader_2d);
    vk_context->destroy_shader(fragment_shader_2d);
    vk_context->destroy_graphics_pipeline(pipeline_2d);
    if (vertex_buffer != nullptr)
    {
        vk_context->destroy_buffer(vertex_buffer);
    }
}

void debug_draw_render(camera* camera, vk::context* vk_context, vk::command_buffer* cmd, vk::image* color,
                       vk::image* depth)
{
    uint32_t num_vertices = triangles_3d.size + lines_3d.size + triangles_2d.size + lines_2d.size;

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

    uint32_t offset = 0;
    memcpy(vertex_buffer->mapped_data, triangles_3d.data, triangles_3d.size_bytes());
    offset += triangles_3d.size_bytes();
    memcpy(vertex_buffer->mapped_data + offset, lines_3d.data, lines_3d.size_bytes());
    offset += lines_3d.size_bytes();
    memcpy(vertex_buffer->mapped_data + offset, triangles_2d.data, triangles_2d.size_bytes());
    offset += triangles_2d.size_bytes();
    memcpy(vertex_buffer->mapped_data + offset, lines_2d.data, lines_2d.size_bytes());
    offset += lines_2d.size_bytes();

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = {color->description.width, color->description.height};
    cmd->set_scissor(scissor);

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)color->description.width;
    viewport.height = (float)color->description.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    cmd->set_viewport(viewport);

    cmd->barrier(color, vk::image_usage::color_attachment);
    push_constant push_constant;
    push_constant.view_proj = camera->view_proj;
    push_constant.resolution = {color->description.width, color->description.height};
    push_constant.vertex_buffer = vertex_buffer->device_address;
    offset = 0;
    {
        cmd->begin_rendering({{color}}, {{vk::load_op::load()}}, depth, vk::load_op::load());
        cmd->push_constant(pipeline_3d, &push_constant, sizeof(push_constant));

        vk::graphics_state graphics_state = vk::graphics_state::create();

        cmd->bind_graphics_pipeline(pipeline_3d, graphics_state);
        cmd->draw(triangles_3d.size, offset);
        offset += triangles_3d.size;

        graphics_state.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        graphics_state.line_width = line_width;
        graphics_state.cull_back_faces = false;
        cmd->bind_graphics_pipeline(pipeline_3d, graphics_state);
        cmd->draw(lines_3d.size, offset);
        offset += lines_3d.size;

        graphics_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        graphics_state.depth_test_enabled = false;
        graphics_state.depth_write_enabled = false;
        graphics_state.depth_compare_op = VK_COMPARE_OP_ALWAYS;
        cmd->bind_graphics_pipeline(pipeline_2d, graphics_state);
        cmd->draw(triangles_2d.size, offset);
        offset += triangles_2d.size;

        graphics_state.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        cmd->bind_graphics_pipeline(pipeline_2d, graphics_state);
        cmd->draw(lines_2d.size, offset);
        offset += lines_2d.size;

        cmd->end_rendering();
    }
    lines_3d.clear();
    triangles_3d.clear();
    lines_2d.clear();
    triangles_2d.clear();
}

void debug_draw_set_line_width(uint32_t width)
{
    line_width = CLAMP(width, 1, 4) - 1;
}

void debug_draw_line_2d(vec2f a, vec2f b, color color)
{
    lines_2d.push_back(debug_vertex{{a.x, a.y, 0}, color});
    lines_2d.push_back(debug_vertex{{b.x, b.y, 0}, color});
}

void debug_draw_triangle_2d(vec2f a, vec2f b, vec2f c, color color)
{
    triangles_2d.push_back(debug_vertex{{a.x, a.y}, color});
    triangles_2d.push_back(debug_vertex{{b.x, b.y}, color});
    triangles_2d.push_back(debug_vertex{{c.x, c.y}, color});
}

void debug_draw_aabox_2d_full(vec2f min, vec2f max, color color)
{
    debug_draw_triangle_2d({min.x, min.y}, {max.x, min.y}, {max.x, max.y}, color);
    debug_draw_triangle_2d({min.x, min.y}, {max.x, max.y}, {min.x, max.y}, color);
}

void debug_draw_aabox_2d_wire(vec2f min, vec2f max, color color)
{
    debug_draw_line_2d({min.x, min.y}, {max.x, min.y}, color);
    debug_draw_line_2d({max.x, min.y}, {max.x, max.y}, color);
    debug_draw_line_2d({max.x, max.y}, {min.x, max.y}, color);
    debug_draw_line_2d({min.x, max.y}, {min.x, min.y}, color);
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

void debug_draw_aabox_wire(vec3f a, vec3f b, color color)
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

void debug_draw_aabox_full(vec3f a, vec3f b, color color)
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