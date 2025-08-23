#include "test_anims.h"

#include "camera.h"

#include "vk/vk_pipeline.h"
#include "vk/vk_image.h"
#include "vk/vk_buffer.h"

#include "core/math/math.h"
#include "core/math/transform.h"
#include "core/log.h"
#include "core/time.h"

#include "ufbx/ufbx.h"

#include "debug_draw.h"
#include "imgui.h"

struct push_constant
{
    mat4f view_proj;
    VkDeviceAddress vertex_buffer;
};

struct vertex
{
    vec4f position;
    vec4f normal;
    vec2f uv;
    vec2f _padding;
};

struct uniform_buffer_data
{};

test_anims test_anims::create(vk::context* _context, uint32_t _width, uint32_t _height)
{
    test_anims test_anims = {};
    test_anims.context = _context;

    test_anims.resize(_width, _height);
    test_anims.reload_shaders();

    ufbx_load_opts opts = {};
    ufbx_error error;
    ufbx_scene* scene = ufbx_load_file("resources/adventurer.fbx", &opts, &error);
    ASSERT(scene, "Failed to load fbx: %s", error.description.data);

    // Let's just list all objects within the scene for example:
    for (uint32_t i = 0; i < scene->nodes.count; i++)
    {
        ufbx_node* node = scene->nodes.data[i];
        if (node->is_root || !node->mesh)
            continue;

        ufbx_mesh* mesh = node->mesh;

        // Count the number of needed parts and temporary buffers
        size_t max_triangles = 0;

        // We need to render each material of the mesh in a separate part, so let's
        // count the number of parts and maximum number of triangles needed.
        for (uint32_t pi = 0; pi < mesh->material_parts.count; pi++)
        {
            ufbx_mesh_part* part = &mesh->material_parts.data[pi];
            max_triangles = MAX(max_triangles, part->num_triangles);
        }

        size_t num_tri_indices = mesh->max_face_triangles * 3;
        uint32_t* tri_indices = (uint32_t*)malloc(num_tri_indices * sizeof(uint32_t));
        vertex* vertices = (vertex*)malloc(max_triangles * 3 * sizeof(vertex));
        uint32_t* indices = (uint32_t*)malloc(max_triangles * 3 * sizeof(uint32_t));

        for (uint32_t pi = 0; pi < mesh->material_parts.count; pi++)
        {
            ufbx_mesh_part* mesh_part = &mesh->material_parts.data[pi];
            if (mesh_part->num_triangles == 0)
                continue;

            uint32_t num_indices = 0;

            for (uint32_t fi = 0; fi < mesh_part->num_faces; fi++)
            {
                ufbx_face face = mesh->faces.data[mesh_part->face_indices.data[fi]];
                uint32_t num_tris = ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);

                ufbx_vec2 default_uv = {};

                // Iterate through every vertex of every triangle in the triangulated result
                for (uint32_t vi = 0; vi < num_tris * 3; vi++)
                {
                    uint32_t ix = tri_indices[vi];
                    vertex* vert = &vertices[num_indices];

                    ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, ix);
                    ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, ix);
                    ufbx_vec2 uv = mesh->vertex_uv.exists ? ufbx_get_vertex_vec2(&mesh->vertex_uv, ix) : default_uv;

                    vert->position = {pos.x, pos.z, pos.y, 1.0f};
                    vert->normal = {normal.x, normal.z, normal.y, 1.0f};
                    vert->uv = {uv.x, uv.y};

                    num_indices++;
                }
            }

            ufbx_vertex_stream streams[1];
            uint32_t num_streams = 1;

            streams[0].data = vertices;
            streams[0].vertex_count = num_indices;
            streams[0].vertex_size = sizeof(vertex);

            uint32_t num_vertices =
                (uint32_t)ufbx_generate_indices(streams, num_streams, indices, num_indices, NULL, &error);
            ASSERT(error.type == UFBX_ERROR_NONE);

            vk::buffer_description buffer_description = {};
            buffer_description.size = num_indices * sizeof(uint32_t);
            buffer_description.usage = vk::index_buffer_usage;
            buffer_description.name = "index buffer";
            vk::buffer* index_buffer = test_anims.index_buffers.push_back(_context->create_buffer(buffer_description));

            buffer_description.size = num_vertices * sizeof(vertex);
            buffer_description.usage = vk::storage_buffer_usage;
            buffer_description.name = "vertex buffer";
            vk::buffer* vertex_buffer =
                test_anims.vertex_buffers.push_back(_context->create_buffer(buffer_description));

            buffer_description.size = num_indices * sizeof(uint32_t);
            buffer_description.usage = vk::transfer_buffer_usage;
            buffer_description.name = "staging buffer";
            vk::buffer* index_staging_buffer = _context->create_buffer(buffer_description);

            buffer_description.size = num_vertices * sizeof(vertex);
            buffer_description.usage = vk::transfer_buffer_usage;
            buffer_description.name = "staging buffer";
            vk::buffer* vertex_staging_buffer = _context->create_buffer(buffer_description);

            vk::command_buffer* cmd = _context->transfer_commands.get_command_buffer();
            cmd->upload_buffer(index_buffer, index_staging_buffer, indices, num_indices * sizeof(uint32_t));
            cmd->upload_buffer(vertex_buffer, vertex_staging_buffer, vertices, num_vertices * sizeof(vertex));
            _context->submit(cmd);
            _context->wait_idle();
            _context->destroy_buffer(index_staging_buffer);
            _context->destroy_buffer(vertex_staging_buffer);
        }
    }

    ufbx_free_scene(scene);

    vk::buffer_description buffer_description = {};
    buffer_description.size = sizeof(uniform_buffer_data);
    buffer_description.usage = vk::uniform_buffer_usage;
    buffer_description.name = "uniform buffer";
    test_anims.uniform_buffer = _context->create_buffer(buffer_description);

    vk::command_buffer* cmd = _context->transfer_commands.get_command_buffer();
    cmd->barrier(test_anims.depth, vk::image_usage::depth_attachment);
    _context->submit(cmd);
    _context->wait_idle();

    return test_anims;
}

void test_anims::destroy()
{
    context->destroy_graphics_pipeline(graphics_pipeline);
    context->destroy_shader(fragment_shader);
    context->destroy_shader(vertex_shader);
    context->destroy_buffer(uniform_buffer);
    for (vk::buffer* buffer : index_buffers)
    {
        context->destroy_buffer(buffer);
    }
    for (vk::buffer* buffer : vertex_buffers)
    {
        context->destroy_buffer(buffer);
    }
    context->destroy_image(depth);
    imgui_remove_texture(&render_target);
    context->destroy_image(render_target.image);
}

void test_anims::resize(uint32_t _width, uint32_t _height)
{
    if (_width == width && _height == height)
    {
        return;
    }

    if (depth)
    {
        context->wait_idle();
        context->destroy_image(depth);
        imgui_remove_texture(&render_target);
        context->destroy_image(render_target.image);
    }

    width = _width;
    height = _height;

    vk::image_description image_description = {};
    image_description.width = width;
    image_description.height = height;
    image_description.format = VK_FORMAT_D32_SFLOAT;
    image_description.usage = vk::image_usage_depth_attachment;
    image_description.name = "depth image";
    depth = context->create_image(image_description);

    image_description.format = context->surface.images[0]->description.format;
    image_description.usage = vk::image_usage_color_attachment;
    image_description.name = "imgui render target";
    render_target.image = context->create_image(image_description);
    render_target.sampler = context->default_sampler;
    imgui_add_texture(&render_target);
}

void test_anims::reload_shaders()
{
    if (graphics_pipeline)
    {
        context->wait_idle();
        context->destroy_graphics_pipeline(graphics_pipeline);
        context->destroy_shader(vertex_shader);
        context->destroy_shader(fragment_shader);
    }

    vertex_shader = context->create_shader("shaders/spv/test_anims.vert");
    fragment_shader = context->create_shader("shaders/spv/test_anims.frag");

    vk::image* swapchain_image = context->surface.images[0];

    vk::graphics_pipeline_description pipeline_desc = {};
    pipeline_desc.vertex_shader = vertex_shader;
    pipeline_desc.fragment_shader = fragment_shader;
    pipeline_desc.color_formats[pipeline_desc.num_color_formats++] = swapchain_image->full_view.format;
    pipeline_desc.depth_format = VK_FORMAT_D32_SFLOAT;
    pipeline_desc.push_constant_size = sizeof(push_constant);
    pipeline_desc.name = "test pipeline";
    graphics_pipeline = context->create_graphics_pipeline(pipeline_desc);
}

void test_anims::draw(vk::frame_context* frame_context, camera* camera)
{
    vk::command_buffer* command_buffer = frame_context->command_buffer;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = {width, height};
    command_buffer->set_scissor(scissor);

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)width;
    viewport.height = (float)height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    command_buffer->set_viewport(viewport);

    command_buffer->barrier(render_target.image, vk::image_usage::color_attachment);
    command_buffer->begin_rendering({{render_target.image}}, {{vk::load_op::clear_color()}}, depth,
                                    vk::load_op::clear_depth());

    vk::graphics_state graphics_state = vk::graphics_state::create();
    command_buffer->bind_graphics_pipeline(graphics_pipeline, graphics_state);
    command_buffer->bind_descriptor_buffer(graphics_pipeline);

    push_constant push_constant;
    push_constant.view_proj = camera->view_proj;
    for (uint32_t i = 0; i < index_buffers.size; ++i)
    {
        push_constant.vertex_buffer = vertex_buffers[i]->device_address;
        command_buffer->push_constant(graphics_pipeline, &push_constant, sizeof(push_constant));

        command_buffer->bind_index_buffer(index_buffers[i]);
        command_buffer->draw_indexed((uint32_t)index_buffers[i]->description.size / sizeof(uint32_t));
    }

    command_buffer->end_rendering();

    debug_draw_line_3d({0.0f, 0.0f, 0.0f}, WORLD_RIGHT * 100.0f, COLOR_X);
    debug_draw_line_3d({0.0f, 0.0f, 0.0f}, WORLD_UP * 100.0f, COLOR_Y);
    debug_draw_line_3d({0.0f, 0.0f, 0.0f}, WORLD_FORWARD * 100.0f, COLOR_Z);

    vec3f o3 = camera->position + camera->forward;
    vec3f xp3 = o3 + WORLD_RIGHT * 0.05f;
    vec3f yp3 = o3 + WORLD_UP * 0.05f;
    vec3f zp3 = o3 + WORLD_FORWARD * 0.05f;
    vec3f xn3 = o3 - WORLD_RIGHT * 0.05f;
    vec3f yn3 = o3 - WORLD_UP * 0.05f;
    vec3f zn3 = o3 - WORLD_FORWARD * 0.05f;
    vec3f o2 = camera_world_to_screen(camera, o3, {width, height}) + vec3f{width * 0.5f, height * 0.5f, 0.0f} - 50.0f;
    vec3f xp2 = camera_world_to_screen(camera, xp3, {width, height}) + vec3f{width * 0.5f, height * 0.5f, 0.0f} - 50.0f;
    vec3f yp2 = camera_world_to_screen(camera, yp3, {width, height}) + vec3f{width * 0.5f, height * 0.5f, 0.0f} - 50.0f;
    vec3f zp2 = camera_world_to_screen(camera, zp3, {width, height}) + vec3f{width * 0.5f, height * 0.5f, 0.0f} - 50.0f;
    vec3f xn2 = camera_world_to_screen(camera, xn3, {width, height}) + vec3f{width * 0.5f, height * 0.5f, 0.0f} - 50.0f;
    vec3f yn2 = camera_world_to_screen(camera, yn3, {width, height}) + vec3f{width * 0.5f, height * 0.5f, 0.0f} - 50.0f;
    vec3f zn2 = camera_world_to_screen(camera, zn3, {width, height}) + vec3f{width * 0.5f, height * 0.5f, 0.0f} - 50.0f;

    float size = 4;
    debug_draw_line_2d(o2, xp2, COLOR_X);
    debug_draw_line_2d(o2, yp2, COLOR_Y);
    debug_draw_line_2d(o2, zp2, COLOR_Z);
    debug_draw_aabox_2d_full(xp2 - size, xp2 + size, COLOR_X);
    debug_draw_aabox_2d_full(yp2 - size, yp2 + size, COLOR_Y);
    debug_draw_aabox_2d_full(zp2 - size, zp2 + size, COLOR_Z);
    debug_draw_aabox_2d_wire(xn2 - size, xn2 + size, COLOR_X);
    debug_draw_aabox_2d_wire(yn2 - size, yn2 + size, COLOR_Y);
    debug_draw_aabox_2d_wire(zn2 - size, zn2 + size, COLOR_Z);

    debug_draw_render(camera, context, command_buffer, render_target.image, depth);

    command_buffer->barrier(render_target.image, vk::image_usage::compute_shader_read);
}
